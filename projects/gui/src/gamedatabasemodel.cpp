/*
    This file is part of Cute Chess.

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gamedatabasemodel.h"
#include "gamedatabasemanager.h"
#include "treeviewitem.h"
#include "pgndatabase.h"
#include <pgngameentry.h>

#include <QDebug>

const QStringList GameDatabaseModel::m_headers = (QStringList() <<
	tr("White") << tr("Black") << tr("Event") << tr("Site") << tr("Result"));

GameDatabaseModel::GameDatabaseModel(GameDatabaseManager* gameDatabaseManager,
                                     QObject* parent)
	: QAbstractItemModel(parent),
	  m_gameDatabaseManager(gameDatabaseManager)
{
	Q_ASSERT(gameDatabaseManager);

	m_root = new TreeViewItem();

	/* Use a tree structure to adapt the GameDatabaseManager's list of
	 * databases and games to a tree-like format. All databases are children
	 * of 'm_root' while the games are children of their respective databases.
	 * Using a tree structure makes some operations, like index() and
	 * parent() O(1). The penalty is that initially we have to build the
	 * structure and later we have to keep it updated.
	 */
	for (int i = 0; i < m_gameDatabaseManager->databases().count(); i++)
	{
		m_root->addChild(
			buildInternalTree(m_gameDatabaseManager->databases().at(i), i));
	}

	connect(m_gameDatabaseManager, SIGNAL(databaseAdded(int)), this,
		SLOT(onDatabaseAdded(int)));
	connect(m_gameDatabaseManager, SIGNAL(databasesReset()), this,
		SLOT(onDatabasesReset()));
}

GameDatabaseModel::~GameDatabaseModel()
{
	delete m_root;
	m_root = 0;
}

TreeViewItem* GameDatabaseModel::root() const
{
	return m_root;
}

TreeViewItem* GameDatabaseModel::buildInternalTree(PgnDatabase* db, int row)
{
	Q_ASSERT(db != NULL);

	TreeViewItem* dbItem = new TreeViewItem(m_root);
	dbItem->setRow(row);
	dbItem->setData(db);

	TreeViewItem* gameItem;
	for (int i = 0; i < db->entries().count(); i++)
	{
		gameItem = new TreeViewItem(dbItem);
		gameItem->setRow(i);

		gameItem->setData(db->entries().at(i));
		dbItem->addChild(gameItem);
	}
	return dbItem;
}

void GameDatabaseModel::onDatabaseAdded(int index)
{
	qDebug() << "onDatabaseAdded, index:" << index;

	beginInsertRows(QModelIndex(), index, index);

	TreeViewItem* dbItem =
		buildInternalTree(m_gameDatabaseManager->databases().at(index), index);

	if (index < m_gameDatabaseManager->databases().count() - 1)
	{
		qDebug() << "Database was added before back";
		m_root->addChildAt(dbItem, index);
	}
	else
	{
		qDebug() << "Database was added at back";
		m_root->addChild(dbItem);
	}

	endInsertRows();
}

void GameDatabaseModel::onDatabasesReset()
{
	delete m_root;
	m_root = new TreeViewItem();

	for (int i = 0; i < m_gameDatabaseManager->databases().count(); i++)
	{
		m_root->addChild(
			buildInternalTree(m_gameDatabaseManager->databases().at(i), i));
	}

	reset();
}

QModelIndex GameDatabaseModel::index(int row, int column,
                                     const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (!parent.isValid())
		return createIndex(row, column, m_root->children().at(row));

	TreeViewItem* parentItem =
		static_cast<TreeViewItem*>(parent.internalPointer());

	return createIndex(row, column, parentItem->children().at(row));
}

QModelIndex GameDatabaseModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeViewItem* item =
		static_cast<TreeViewItem*>(index.internalPointer());

	if (item->parent() == m_root)
		return QModelIndex();

	return createIndex(item->parent()->row(), 0, item->parent());
}

int GameDatabaseModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return m_root->children().count();

	TreeViewItem* parentItem =
		static_cast<TreeViewItem*>(parent.internalPointer());

	return parentItem->children().count();
}

int GameDatabaseModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return m_headers.count();
}

QVariant GameDatabaseModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeViewItem* item =
		static_cast<TreeViewItem*>(index.internalPointer());

	if (item->parent() == m_root) // item is a database?
	{
		PgnDatabase* db = static_cast<PgnDatabase*>(item->data());

		if (index.column() == 0 && (role == Qt::DisplayRole ||
		    role == Qt::EditRole))
		{
			return db->displayName();
		}
	}
	else // it's a game
	{
		PgnGameEntry* gameEntry = static_cast<PgnGameEntry*>(item->data());

		if (role == Qt::DisplayRole)
		{
			switch (index.column())
			{
				case 0:
					return gameEntry->white();

				case 1:
					return gameEntry->black();

				case 2:
					return gameEntry->event();

				case 3:
					return gameEntry->site();

				case 4:
					return gameEntry->result().toShortString();

				default:
					return QVariant();
			}
		}
	}
	return QVariant();
}

QVariant GameDatabaseModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
			return m_headers.at(section);
		else
			return QString::number(section + 1);
	}

	return QVariant();
}

Qt::ItemFlags GameDatabaseModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlags(Qt::NoItemFlags);

	Qt::ItemFlags defaultFlags =
		Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	TreeViewItem* item =
		static_cast<TreeViewItem*>(index.internalPointer());

	// database's display name is editable
	if (item->parent() == m_root && index.column() == 0)
		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);

	return defaultFlags;  // games are not editable
}

bool GameDatabaseModel::setData(const QModelIndex& index, const QVariant& data,
                                int role)
{
	Q_UNUSED(role);

	if (!index.isValid())
		return false;

	TreeViewItem* item =
		static_cast<TreeViewItem*>(index.internalPointer());

	// only database's display name should be editable
	if (item->parent() == m_root && index.column() == 0)
	{
		PgnDatabase* db = static_cast<PgnDatabase*>(item->data());
		db->setDisplayName(data.toString());

		m_gameDatabaseManager->setModified(true);

		return true;
	}
	return false;
}

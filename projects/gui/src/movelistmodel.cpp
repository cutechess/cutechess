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

#include "movelistmodel.h"
#include <chessgame.h>


const QStringList MoveListModel::m_headers = (QStringList() <<
	QString("#") << tr("White") << tr("Black"));

MoveListModel::MoveListModel(QObject* parent)
	: QAbstractItemModel(parent),
	  m_game(0)
{
}

void MoveListModel::setGame(ChessGame* game)
{
	Q_ASSERT(game != 0);

	if (m_game != 0)
	{
		disconnect(m_game);
		m_game->disconnect(this);
	}
	m_game = game;

	m_moveList.clear();
	foreach (const PgnGame::MoveData& md, m_game->pgn()->moves())
		m_moveList.append(qMakePair(md.moveString, md.comment));

	connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		this, SLOT(onMoveMade(Chess::GenericMove, QString, QString)));
	reset();
}

QModelIndex MoveListModel::index(int row, int column,
                                 const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex MoveListModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int MoveListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	if (m_game)
	{
		int movesCount = m_moveList.size();

		if (movesCount % 2 == 0)
		{
			return movesCount / 2;
		}
		else
		{
			return (int)(movesCount / 2) + 1;
		}
	}
	else
		return 0;
}

int MoveListModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_headers.count();
}

QVariant MoveListModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && role == Qt::DisplayRole)
	{
		if (index.column() == 0)
			return QString::number(index.row() + 1);

		if (m_moveList.size() > ((index.row() * 2) + index.column() - 1))
		{

			int i = (index.row() * 2) + index.column() - 1;
			const QPair<QString, QString> pair(m_moveList.at(i));

			if (pair.second.isEmpty())
				return pair.first;
			else
				return pair.first + " {" + pair.second + " }";
		}
	}

	return QVariant();
}

QVariant MoveListModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
		return m_headers.at(section);

	return QVariant();
}

void MoveListModel::onMoveMade(const Chess::GenericMove& move,
			       const QString& sanString,
			       const QString& comment)
{
	Q_UNUSED(move);

	m_moveList.append(qMakePair(sanString, comment));
	int movesCount = m_moveList.size();

	if (movesCount % 2 == 0)
	{
		QModelIndex indexToUpdate = index(movesCount / 2, 1);
		emit dataChanged(indexToUpdate, indexToUpdate);
	}
	else
	{
		beginInsertRows(QModelIndex(), (int)(movesCount / 2), (int)(movesCount / 2));
		endInsertRows();
	}
}

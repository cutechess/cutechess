#include "engineoptionmodel.h"

#include <engineoption.h>
#include <enginebuttonoption.h>

const QStringList EngineOptionModel::s_headers = (QStringList() <<
	tr("Name") << tr("Value") << tr("Alias"));

EngineOptionModel::EngineOptionModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

EngineOptionModel::EngineOptionModel(QList<EngineOption*> options, QObject* parent)
	: QAbstractItemModel(parent),
	  m_options(options)
{
}

void EngineOptionModel::setOptions(const QList<EngineOption*>& options)
{
	beginResetModel();
	m_options = options;
	endResetModel();
}

QModelIndex EngineOptionModel::index(int row, int column,
                                     const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex EngineOptionModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int EngineOptionModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_options.count();
}

int EngineOptionModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return s_headers.count();
}

QVariant EngineOptionModel::data(const QModelIndex& index, int role) const
{
	const EngineOption* option = m_options.at(index.row());

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
				return option->name();

			case 1:
				// see Qt::CheckStateRole
				if (option->value().type() == QVariant::Bool)
					return QVariant();

				return option->value();

			case 2:
				return option->alias();

			default:
				return QVariant();
		}
	}
	else if (role == Qt::CheckStateRole)
	{
		if (index.column() == 1 && option->value().type() == QVariant::Bool)
			return option->value().toBool() ? Qt::Checked : Qt::Unchecked;

		return QVariant();
	}
	else if (role == Qt::EditRole)
	{
		switch (index.column())
		{
			case 0:
				return option->name();

			case 1:
				return option->toVariant();

			case 2:
				return option->alias();

			default:
				return QVariant();
		}
	}

	return QVariant();
}

QVariant EngineOptionModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
		return s_headers.at(section);

	return QVariant();
}

Qt::ItemFlags EngineOptionModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlags(Qt::NoItemFlags);

	Qt::ItemFlags defaultFlags =
		Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	if (index.column() == 0)
		return defaultFlags;
	if (index.column() == 2)
		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);

	if (index.column() == 1)
	{
		// option values are editable except button options
		EngineButtonOption* buttonOption =
			dynamic_cast<EngineButtonOption*>(m_options.at(index.row()));

		if (buttonOption)
			return defaultFlags;

		// make check options checkable
		if (m_options.at(index.row())->value().type() == QVariant::Bool)
			return Qt::ItemFlags(defaultFlags | Qt::ItemIsUserCheckable);

		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);
	}

	return defaultFlags;
}

bool EngineOptionModel::setData(const QModelIndex& index, const QVariant& data,
                                int role)
{
	Q_UNUSED(role);

	if (!index.isValid())
		return false;

	EngineOption* option = m_options.at(index.row());
	if (index.column() == 0)
	{
		const QString stringData = data.toString();
		if (stringData.isEmpty())
			return false;

		option->setName(stringData);
		return true;
	}
	else if (index.column() == 1)
	{
		if (role == Qt::CheckStateRole)
		{
			option->setValue(data.toInt() == Qt::Checked);
			return true;
		}
		else if (option->isValid(data))
		{
			option->setValue(data);
			return true;
		}
	}
	else if (index.column() == 2)
	{
		option->setAlias(data.toString());
		return true;
	}
	return false;
}

#include "engineoptionmodel.h"

#include <engineoption.h>

const QStringList EngineOptionModel::s_headers = (QStringList() <<
	tr("Name") << tr("Value"));

EngineOptionModel::EngineOptionModel(QList<EngineOption*> options, QObject* parent)
	: QAbstractItemModel(parent),
	  m_options(options)
{
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
	Q_UNUSED(index);
	Q_UNUSED(role);

	const EngineOption* option = m_options.at(index.row());

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
				return option->name();

			case 1:
				return option->value();

			default:
				return QVariant();
		}
	}
	else if (role == Qt::EditRole && index.column() == 1)
		return option->toVariant();

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

	// option values are editable
	if (index.column() == 1)
		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);

	return defaultFlags;
}

bool EngineOptionModel::setData(const QModelIndex& index, const QVariant& data,
                                int role)
{
	Q_UNUSED(role);

	if (!index.isValid())
		return false;

	// only option's value should be editable
	if (index.column() == 1)
	{
		EngineOption* option = m_options.at(index.row());
		if (option->isValid(data))
		{
			option->setValue(data);
			return true;
		}
	}
	return false;
}

#ifndef ENGINE_OPTION_MODEL_H
#define ENGINE_OPTION_MODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class EngineOption;

class EngineOptionModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		EngineOptionModel(QList<EngineOption*> options, QObject* parent = 0);

		// Inherited from QAbstractItemModel
		QModelIndex index(int row, int column,
                          const QModelIndex& parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex& index) const;
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex& index) const;
		bool setData(const QModelIndex& index, const QVariant& value,
                     int role = Qt::EditRole);

	private:
		static const QStringList s_headers;
		QList<EngineOption*> m_options;

};

#endif // ENGINE_OPTION_MODEL_H

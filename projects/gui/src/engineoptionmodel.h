#ifndef ENGINE_OPTION_MODEL_H
#define ENGINE_OPTION_MODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class EngineOption;

class EngineOptionModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		EngineOptionModel(QObject* parent = 0);
		EngineOptionModel(QList<EngineOption*> options, QObject* parent = 0);

		void setOptions(const QList<EngineOption*>& options);

		// Inherited from QAbstractItemModel
		virtual QModelIndex index(int row, int column,
					  const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex parent(const QModelIndex& index) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation,
					    int role = Qt::DisplayRole) const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual bool setData(const QModelIndex& index, const QVariant& value,
				     int role = Qt::EditRole);

	private:
		static const QStringList s_headers;
		QList<EngineOption*> m_options;

};

#endif // ENGINE_OPTION_MODEL_H

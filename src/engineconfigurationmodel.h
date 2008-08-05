#ifndef ENGINE_CONFIGURATION_MODEL_H
#define ENGINE_CONFIGURATION_MODEL_H

#include <QAbstractListModel>
#include <QList>

#include "engineconfiguration.h"

class EngineConfigurationModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		EngineConfigurationModel(QObject* parent = 0);
		
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;

		void addEngineConfiguration(const EngineConfiguration& configuration);
		QList<EngineConfiguration> engineConfigurations() const;
	
	private:
		QList<EngineConfiguration> m_configurations;

};

#endif // ENGINE_CONFIGURATION_MODEL_H


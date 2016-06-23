#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QVariantMap>

class SettingsManager
{
	public:
		SettingsManager(const QString& filename);

		void load();
		void save() const;

		QVariant value(const QString& key) const;
		void setValue(const QString& key, const QVariant& value);

	private:
		QString m_filename;
		QVariantMap m_data;
};

#endif // SETTINGSMANAGER_H

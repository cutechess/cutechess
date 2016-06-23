#include "settingsmanager.h"
#include <QFile>
#include <QTextStream>
#include <jsonparser.h>
#include <jsonserializer.h>

SettingsManager::SettingsManager(const QString& filename)
	: m_filename(filename)
{
}

void SettingsManager::load()
{
	if (!QFile::exists(m_filename))
		return;

	QFile input(m_filename);
	if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning("cannot open settings file: %s", qPrintable(m_filename));
		return;
	}

	QTextStream stream(&input);
	JsonParser parser(stream);
	m_data = parser.parse().toMap();

	if (parser.hasError())
	{
		qWarning("%s", qPrintable(QString("bad settings file line %1 in %2: %3")
			.arg(parser.errorLineNumber()).arg(m_filename)
			.arg(parser.errorString())));
		return;
	}
}

void SettingsManager::save() const
{
	QFile output(m_filename);
	if (!output.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qWarning("cannot open settings file: %s", qPrintable(m_filename));
		return;
	}

	QTextStream out(&output);
	JsonSerializer serializer(m_data);
	serializer.serialize(out);
}

QVariant SettingsManager::value(const QString& key) const
{
	if (!m_data.contains(key))
	{
		qWarning("value does not exist: %s", qPrintable(key));
		return QVariant();
	}

	return m_data.value(key);
}

void SettingsManager::setValue(const QString& key, const QVariant& value)
{
	m_data[key] = value;
}

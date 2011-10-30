#include "engineoptionfactory.h"

#include <QtDebug>

#include "engineoption.h"
#include "enginetextoption.h"
#include "enginebuttonoption.h"
#include "enginecheckoption.h"
#include "enginecombooption.h"
#include "enginespinoption.h"

EngineOption* EngineOptionFactory::create(const QVariantMap& map)
{
	// TODO: use isValid() for each option

	const QString name = map["name"].toString();
	const QString type = map["type"].toString();
	const QVariant value = map["value"];
	QVariant defaultValue = map["default"];
	const QString alias = map["alias"].toString();

	if (name.isEmpty())
	{
		qWarning() << "Empty option name";
		return 0;
	}

	// Special case for the button option type: its value is the name
	if (type == "button")
		return new EngineButtonOption(name);

	if (value.type() != QVariant::Bool &&
		value.type() != QVariant::String &&
		value.type() != QVariant::Int)
	{
		qWarning() << "Invalid value type for option:" << name;
		return 0;
	}

	if (defaultValue.isNull())
		defaultValue = value;
	else if (defaultValue.type() != QVariant::Bool &&
		defaultValue.type() != QVariant::String &&
		defaultValue.type() != QVariant::Int)
	{
		qWarning() << "Invalid default value type for option:" << name;
		return 0;
	}

	// If the option type has not been defined, use text option as
	// a default option type
	if (type.isEmpty())
	{
		return new EngineTextOption(name, value.toString(),
			defaultValue.toString(), alias);
	}
	else if (type == "text")
	{
		return new EngineTextOption(name, value.toString(),
			defaultValue.toString(), alias);
	}
	else if (type == "check")
	{
		return new EngineCheckOption(name, value.toBool(),
			defaultValue.toBool(), alias);
	}
	else if (type == "combo")
	{
		const QVariant choices = map["choices"];
		if (choices.type() != QVariant::StringList)
			return 0;

		return new EngineComboOption(name, value.toString(),
			defaultValue.toString(), choices.toStringList(), alias);
	}
	else if (type == "spin")
	{
		int intValue, defaultIntValue, minValue, maxValue;
		bool ok;

		intValue = value.toInt(&ok);
		if (!ok)
			return 0;

		defaultIntValue = defaultValue.toInt(&ok);
		if (!ok)
			return 0;

		minValue = map["min"].toInt(&ok);
		if (!ok)
			return 0;

		maxValue = map["max"].toInt(&ok);
		if (!ok)
			return 0;

		return new EngineSpinOption(name, intValue, defaultIntValue,
			minValue, maxValue, alias);
	}

	return 0;
}

#include "enginetextoption.h"

EngineTextOption::EngineTextOption(const QString& name,
				   const QVariant& value,
				   const QVariant& defaultValue)
	: EngineOption(name, value, defaultValue)
{
}

EngineOption* EngineTextOption::copy() const
{
	return new EngineTextOption(*this);
}

bool EngineTextOption::isValid(const QVariant& value) const
{
	return value.canConvert(QVariant::String);
}

QVariant EngineTextOption::toVariant() const
{
	QVariantMap map;

	map.insert("type", "text");

	map.insert("name", name());
	map.insert("value", value());
	map.insert("default", defaultValue());

	return map;
}

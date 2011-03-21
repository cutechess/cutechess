#include "enginetextoption.h"

EngineTextOption::EngineTextOption(const QString& name,
                                   const QVariant& value,
                                   const QVariant& defaultValue,
                                   const QString& alias)
	: EngineOption(name, value, defaultValue, alias)
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
	map.insert("alias", alias());

	return map;
}

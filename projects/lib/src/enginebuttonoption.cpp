#include "enginebuttonoption.h"

EngineButtonOption::EngineButtonOption(const QString& name)
	: EngineOption(name)
{
}

EngineOption* EngineButtonOption::copy() const
{
	return new EngineButtonOption(*this);
}

bool EngineButtonOption::isValid(const QVariant& value) const
{
	return value.isNull();
}

QVariant EngineButtonOption::toVariant() const
{
	QVariantMap map;

	map.insert("type", "button");
	map.insert("name", name());

	return map;
}

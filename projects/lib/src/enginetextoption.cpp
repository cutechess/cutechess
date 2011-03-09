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

QVariant EngineTextOption::toQVariant() const
{
	if (isValid(value()))
		return value();
	else if (isValid(defaultValue()))
		return defaultValue();
	else
		return QString();
}

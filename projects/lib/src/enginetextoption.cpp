#include "enginetextoption.h"

EngineTextOption::EngineTextOption(const QString& name,
				   const QVariant& value,
				   const QVariant& defaultValue)
	: EngineOption(name, value, defaultValue)
{
}

bool EngineTextOption::isValid(const QVariant& value) const
{
	return value.canConvert(QVariant::String);
}

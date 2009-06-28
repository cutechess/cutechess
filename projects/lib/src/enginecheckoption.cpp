#include "enginecheckoption.h"

EngineCheckOption::EngineCheckOption(const QString& name,
				     const QVariant& value,
				     const QVariant& defaultValue)
	: EngineOption(name, value, defaultValue)
{
}

bool EngineCheckOption::isValid(const QVariant& value) const
{
	return value.canConvert(QVariant::Bool);
}

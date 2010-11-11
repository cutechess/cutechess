#include "enginecheckoption.h"

EngineCheckOption::EngineCheckOption(const QString& name,
				     const QVariant& value,
				     const QVariant& defaultValue)
	: EngineOption(name, value, defaultValue)
{
}

bool EngineCheckOption::isValid(const QVariant& value) const
{
	if (value.canConvert(QVariant::Bool))
	{
		QString str(value.toString());
		return (str == "true" || str == "false");
	}
	return false;
}

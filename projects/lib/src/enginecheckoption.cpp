#include "enginecheckoption.h"

EngineCheckOption::EngineCheckOption(const QString& name,
				     const QVariant& value,
				     const QVariant& defaultValue)
	: EngineOption(name, value, defaultValue)
{
}

EngineOption* EngineCheckOption::copy() const
{
	return new EngineCheckOption(*this);
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

QVariant EngineCheckOption::toQVariant() const
{
	if (isValid(value()))
		return value();
	else if (isValid(defaultValue()))
		return defaultValue();
	else
		return false;
}

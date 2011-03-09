#include "enginebuttonoption.h"

EngineButtonOption::EngineButtonOption()
	: EngineOption(QString())
{
}

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

QVariant EngineButtonOption::toQVariant() const
{
	return QVariant::fromValue(*this);
}

#include "enginebuttonoption.h"

EngineButtonOption::EngineButtonOption()
	: EngineOption(QString())
{
}

EngineButtonOption::EngineButtonOption(const QString& name)
	: EngineOption(name)
{
}

bool EngineButtonOption::isValid(const QVariant& value) const
{
	return value.isNull();
}

QVariant EngineButtonOption::toQVariant() const
{
	return QVariant::fromValue(*this);
}

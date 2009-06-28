#include "enginebuttonoption.h"

EngineButtonOption::EngineButtonOption(const QString& name)
	: EngineOption(name)
{
}

bool EngineButtonOption::isValid(const QVariant& value) const
{
	return value.isNull();
}

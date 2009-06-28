#include "enginecombooption.h"

EngineComboOption::EngineComboOption(const QString& name,
				     const QVariant& value,
				     const QVariant& defaultValue,
				     const QStringList& choices)
	: EngineOption(name, value, defaultValue),
	  m_choices(choices)
{
}

bool EngineComboOption::isValid(const QVariant& value) const
{
	return m_choices.contains(value.toString());
}

QStringList EngineComboOption::choices() const
{
	return m_choices;
}

void EngineComboOption::setChoices(const QStringList& choices)
{
	m_choices = choices;
}

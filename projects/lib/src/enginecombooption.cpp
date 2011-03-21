#include "enginecombooption.h"

EngineComboOption::EngineComboOption(const QString& name,
                                     const QVariant& value,
                                     const QVariant& defaultValue,
                                     const QStringList& choices,
                                     const QString& alias)
	: EngineOption(name, value, defaultValue, alias),
	  m_choices(choices)
{
}

EngineOption* EngineComboOption::copy() const
{
	return new EngineComboOption(*this);
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

QVariant EngineComboOption::toVariant() const
{
	QVariantMap map;

	map.insert("type", "combo");

	map.insert("name", name());
	map.insert("value", value());
	map.insert("default", defaultValue());
	map.insert("alias", alias());

	map.insert("choices", choices());

	return map;
}

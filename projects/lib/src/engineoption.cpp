#include "engineoption.h"


EngineOption::EngineOption(const QString& name,
			   const QVariant& value,
			   const QVariant& defaultValue)
	: m_name(name),
	  m_value(value),
	  m_defaultValue(defaultValue)
{
}

bool EngineOption::isValid() const
{
	if (m_name.isEmpty())
		return false;
	if (!isValid(m_value))
		return false;
	if (!m_defaultValue.isNull() && !isValid(m_defaultValue))
		return false;

	return true;
}

QString EngineOption::name() const
{
	return m_name;
}

QVariant EngineOption::value() const
{
	return m_value;
}

QVariant EngineOption::defaultValue() const
{
	return m_defaultValue;
}

void EngineOption::setName(const QString& name)
{
	m_name = name;
}

void EngineOption::setValue(const QVariant& value)
{
	m_value = value;
}

void EngineOption::setDefaultValue(const QVariant& value)
{
	m_defaultValue = value;
}

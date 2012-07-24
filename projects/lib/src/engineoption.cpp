#include "engineoption.h"


EngineOption::EngineOption(const QString& name,
			   QVariant::Type valueType,
			   const QVariant& value,
			   const QVariant& defaultValue,
			   const QString& alias)
	: m_valueType(valueType),
	  m_name(name),
	  m_value(value),
	  m_defaultValue(defaultValue),
	  m_alias(alias)
{
	if (valueType != QVariant::Invalid)
	{
		m_value.convert(valueType);
		m_defaultValue.convert(valueType);
	}
}

EngineOption::~EngineOption()
{
}

QVariant::Type EngineOption::valueType() const
{
	return m_valueType;
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

QString EngineOption::alias() const
{
	return m_alias;
}

void EngineOption::setName(const QString& name)
{
	m_name = name;
}

void EngineOption::setValue(const QVariant& value)
{
	m_value = value;
	m_value.convert(m_valueType);
}

void EngineOption::setDefaultValue(const QVariant& value)
{
	m_defaultValue = value;
	m_defaultValue.convert(m_valueType);
}

void EngineOption::setAlias(const QString& alias)
{
	m_alias = alias;
}

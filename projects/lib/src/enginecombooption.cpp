/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "enginecombooption.h"

EngineComboOption::EngineComboOption(const QString& name,
                                     const QVariant& value,
                                     const QVariant& defaultValue,
                                     const QStringList& choices,
                                     const QString& alias)
	: EngineOption(name, QVariant::String, value, defaultValue, alias),
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

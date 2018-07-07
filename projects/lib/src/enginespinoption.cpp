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

#include "enginespinoption.h"

EngineSpinOption::EngineSpinOption(const QString& name,
                                   const QVariant& value,
                                   const QVariant& defaultValue,
                                   int min,
                                   int max,
                                   const QString& alias)
	: EngineOption(name, QVariant::Int, value, defaultValue, alias),
	  m_min(min),
	  m_max(max)
{
}

EngineOption* EngineSpinOption::copy() const
{
	return new EngineSpinOption(*this);
}

bool EngineSpinOption::isValid(const QVariant& value) const
{
	if (m_min > m_max)
		return false;

	bool ok = false;
	int tmp = value.toInt(&ok);
	if (!ok
	||  ((m_min != 0 || m_max != 0) && (tmp < m_min || tmp > m_max)))
		return false;

	return true;
}

int EngineSpinOption::min() const
{
	return m_min;
}

int EngineSpinOption::max() const
{
	return m_max;
}

void EngineSpinOption::setMin(int min)
{
	m_min = min;
}

void EngineSpinOption::setMax(int max)
{
	m_max = max;
}

QVariant EngineSpinOption::toVariant() const
{
	QVariantMap map;

	map.insert("type", "spin");

	map.insert("name", name());
	map.insert("value", value());
	map.insert("default", defaultValue());
	map.insert("alias", alias());

	map.insert("min", min());
	map.insert("max", max());

	return map;
}

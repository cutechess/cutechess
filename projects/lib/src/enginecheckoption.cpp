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

#include "enginecheckoption.h"

EngineCheckOption::EngineCheckOption(const QString& name,
                                     const QVariant& value,
                                     const QVariant& defaultValue,
                                     const QString& alias)
	: EngineOption(name, QVariant::Bool, value, defaultValue, alias)
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

QVariant EngineCheckOption::toVariant() const
{
	QVariantMap map;

	map.insert("type", "check");

	map.insert("name", name());
	map.insert("value", value());
	map.insert("default", defaultValue());
	map.insert("alias", alias());

	return map;
}

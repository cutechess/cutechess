/*
    This file is part of Cute Chess.

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

#include "enginetextoption.h"

EngineTextOption::EngineTextOption(const QString& name,
                                   const QVariant& value,
                                   const QVariant& defaultValue,
                                   const QString& alias)
	: EngineOption(name, QVariant::String, value, defaultValue, alias)
{
}

EngineOption* EngineTextOption::copy() const
{
	return new EngineTextOption(*this);
}

bool EngineTextOption::isValid(const QVariant& value) const
{
	return value.canConvert(QVariant::String);
}

QVariant EngineTextOption::toVariant() const
{
	QVariantMap map;

	map.insert("type", "text");

	map.insert("name", name());
	map.insert("value", value());
	map.insert("default", defaultValue());
	map.insert("alias", alias());

	return map;
}

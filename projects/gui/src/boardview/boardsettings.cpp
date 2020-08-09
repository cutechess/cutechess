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

#include "boardsettings.h"

BoardSettings::BoardSettings():
	  m_initialized(false)
{
}

bool BoardSettings::initialized() const
{
	return m_initialized;
}

void BoardSettings::setInitialized(bool value)
{
	m_initialized = value;
}

const QVariant BoardSettings::value(const QString key) const
{
	return m_map.value(key);
}

void BoardSettings::set(const QString key, const QVariant value)
{
	m_map.insert(key, value);
}


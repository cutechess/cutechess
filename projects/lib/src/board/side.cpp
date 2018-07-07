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

#include "side.h"

namespace Chess {

Side::Side(const QString& symbol)
{
	if (symbol == "w")
		m_type = White;
	else if (symbol == "b")
		m_type = Black;
	else
		m_type = NoSide;
}

QString Side::symbol() const
{
	if (m_type == White)
		return "w";
	else if (m_type == Black)
		return "b";

	return QString();
}

QString Side::toString() const
{
	if (m_type == White)
		return tr("white");
	else if (m_type == Black)
		return tr("black");

	return QString();
}

} // namespace Chess

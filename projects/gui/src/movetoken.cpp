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

#include "movetoken.h"

MoveToken::MoveToken(int ply, const QString& text)
	: PgnToken(),
	  m_ply(ply),
	  m_text(text)
{
}

QString MoveToken::toString() const
{
	return m_text;
}

void MoveToken::vInsert(QTextCursor& cursor)
{
	QString html = QString("<a class=\"move\" href=\"move://%1@\">%2</a> ")
		       .arg(m_ply).arg(m_text);

	#ifndef Q_OS_WIN32
	html.replace('-', "&#8288;-&#8288;");
	#endif
	cursor.insertHtml(html);
}

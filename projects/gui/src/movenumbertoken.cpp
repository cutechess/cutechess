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

#include "movenumbertoken.h"
#include <QTextBlockFormat>
#include <board/side.h>

MoveNumberToken::MoveNumberToken(int ply, int startingSide)
	: PgnToken(),
	  m_ply(ply),
	  m_startingSide(startingSide)
{
}

QString MoveNumberToken::toString() const
{
	int number = (m_ply + m_startingSide) / 2 + 1;
	if ((m_ply + m_startingSide) % 2 == 0)
		return QString("%1.").arg(number);
	return QString("%1...").arg(number);
}

void MoveNumberToken::vInsert(QTextCursor& cursor)
{
	QTextBlockFormat format(cursor.blockFormat());

	// Ignore black's move if it's:
	// - not the first move of the game OR
	// - the cursor's not in a comment block
	if (format.indent() == 0 && m_ply > 0
	&&  (m_ply + m_startingSide) % 2 != 0)
	{
		if (cursor.hasSelection())
			cursor.removeSelectedText();
		return;
	}

	format.setIndent(0);

	QString html = QString("<strong>%1</strong> ").arg(toString());
	if (m_ply > 0)
		cursor.insertBlock(format);
	cursor.insertHtml(html);
}

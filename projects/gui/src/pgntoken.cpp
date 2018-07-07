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

#include "pgntoken.h"


PgnToken::PgnToken()
	: m_begin(-1),
	  m_end(-1)
{
}

PgnToken::~PgnToken()
{
}

bool PgnToken::isEmpty() const
{
	return toString().isEmpty();
}

int PgnToken::length() const
{
	return m_end - m_begin;
}

void PgnToken::insert(QTextCursor& cursor)
{
	if (isEmpty())
		return;

	if (!cursor.hasSelection())
		m_begin = cursor.position();
	else
		m_begin = cursor.selectionStart();

	vInsert(cursor);
	m_end = cursor.position();
}

void PgnToken::select(QTextCursor& cursor)
{
	if (m_begin == -1 || m_end == -1)
		return;

	cursor.setPosition(m_begin);
	cursor.setPosition(m_end, QTextCursor::KeepAnchor);
}

void PgnToken::move(int diff)
{
	if (m_begin == -1 || m_end == -1)
		return;

	m_begin += diff;
	m_end += diff;
}

void PgnToken::mergeCharFormat(QTextCursor& cursor, const QTextCharFormat& format)
{
	cursor.setPosition(m_begin);
	cursor.setPosition(m_end - 1, QTextCursor::KeepAnchor);
	cursor.mergeCharFormat(format);
}

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

#include "pgnstream.h"
#include <cctype>
#include <cstring>
#include <QIODevice>
#include "board/boardfactory.h"

namespace {

void skipSection(PgnStream* in, char start)
{
	char end;
	switch (start)
	{
	case '(':
		end = ')';
		break;
	case '{':
		end = '}';
		break;
	case ';':
	case '%':
		start = 0;
		end = '\n';
		break;
	default:
		return;
	}

	int level = 1;
	char c;
	while ((c = in->readChar()) != 0)
	{
		if (c == end && --level == 0)
			break;
		if (c == start)
			level++;
	}
}

} // anonymous namespace

PgnStream::PgnStream(const QString& variant)
	: m_board(nullptr),
	  m_pos(0),
	  m_lineNumber(1),
	  m_lastChar(0),
	  m_tokenType(NoToken),
	  m_device(nullptr),
	  m_string(nullptr),
	  m_status(Ok),
	  m_phase(OutOfGame)
{
	setVariant(variant);
}

PgnStream::PgnStream(QIODevice* device, const QString& variant)
	: m_board(nullptr)
{
	setVariant(variant);
	setDevice(device);
}

PgnStream::PgnStream(const QByteArray* string, const QString& variant)
	: m_board(nullptr)
{
	setVariant(variant);
	setString(string);
}

PgnStream::~PgnStream()
{
	delete m_board;
}

void PgnStream::reset()
{
	m_pos = 0;
	m_lineNumber = 1;
	m_lastChar = 0;
	m_tokenString.clear();
	m_tagName.clear();
	m_tagValue.clear();
	m_tokenType = NoToken;
	m_device = nullptr;
	m_string = nullptr;
	m_status = Ok;
	m_phase = OutOfGame;
}

Chess::Board* PgnStream::board()
{
	return m_board;
}

QIODevice* PgnStream::device() const
{
	return m_device;
}

void PgnStream::setDevice(QIODevice* device)
{
	Q_ASSERT(device != nullptr);

	reset();
	m_device = device;
}

const QByteArray* PgnStream::string() const
{
	return m_string;
}

void PgnStream::setString(const QByteArray* string)
{
	Q_ASSERT(string != nullptr);
	reset();
	m_string = string;
}

QString PgnStream::variant() const
{
	Q_ASSERT(m_board != nullptr);
	return m_board->variant();
}

bool PgnStream::setVariant(const QString& variant)
{
	if (m_board != nullptr && m_board->variant() == variant)
		return true;
	if (!Chess::BoardFactory::variants().contains(variant))
		return false;

	delete m_board;
	m_board = Chess::BoardFactory::create(variant);
	Q_ASSERT(m_board != nullptr);

	return true;
}

bool PgnStream::isOpen() const
{
	return (m_device && m_device->isOpen()) || m_string;
}

qint64 PgnStream::pos() const
{
	if (m_device)
		return m_device->pos();
	return m_pos;
}

qint64 PgnStream::lineNumber() const
{
	return m_lineNumber;
}

char PgnStream::readChar()
{
	char c;
	if (m_device)
	{
		if (!m_device->getChar(&m_lastChar))
		{
			m_status = ReadPastEnd;
			return 0;
		}
		c = m_lastChar;
	}
	else if (m_string && m_pos < m_string->size())
	{
		c = m_string->at(m_pos++);
	}
	else
	{
		m_status = ReadPastEnd;
		return 0;
	}

	if (c == '\n')
		m_lineNumber++;

	return c;
}

void PgnStream::rewind()
{
	seek(0);
}

void PgnStream::rewindChar()
{
	Q_ASSERT(pos() > 0);

	char c;
	if (m_device)
	{
		c = m_lastChar;
		m_device->ungetChar(m_lastChar);
		m_lastChar = 0;
	}
	else if (m_string)
		c = m_string->at(m_pos--);
	else
		return;

	if (c == '\n')
		m_lineNumber--;
}

bool PgnStream::seek(qint64 pos, qint64 lineNumber)
{
	if (pos < 0)
		return false;

	bool ok = false;
	if (m_device)
	{
		ok = m_device->seek(pos);
		m_pos = 0;
	}
	else if (m_string)
	{
		ok = pos < m_string->size();
		m_pos = pos;
	}
	if (!ok)
		return false;

	m_status = Ok;
	m_lineNumber = lineNumber;
	m_lastChar = 0;
	m_phase = OutOfGame;

	return true;
}

PgnStream::Status PgnStream::status() const
{
	return m_status;
}

void PgnStream::parseUntil(const char* chars)
{
	Q_ASSERT(chars != nullptr);

	char c;
	while ((c = readChar()) != 0)
	{
		if (strchr(chars, c))
			break;
		m_tokenString.append(c);
	}
}

void PgnStream::parseTag()
{
	bool inQuotes = false;
	int phase = 0;
	char c;

	m_tagName.clear();
	m_tagValue.clear();

	while ((c = readChar()) != 0)
	{
		if (!inQuotes && c == ']')
			break;
		if (c == '\n' || c == '\r')
			break;
		m_tokenString.append(c);

		switch (phase)
		{
		case 0:
			if (!isspace(c))
			{
				phase++;
				m_tagName.append(c);
			}
			break;
		case 1:
			if (!isspace(c))
				m_tagName.append(c);
			else
				phase++;
			break;
		case 2:
			if (!isspace(c))
			{
				phase++;
				if (c == '\"')
					inQuotes = true;
				else
					m_tagValue.append(c);
			}
			break;
		case 3:
			if (inQuotes)
			{
				if (c == '\"')
				{
					inQuotes = false;
					phase++;
				}
				else
					m_tagValue.append(c);
			}
			else if (!isspace(c))
				m_tagValue.append(c);
			break;
		default:
			break;
		}
	}
}

void PgnStream::parseComment(char opBracket)
{
	int level = 1;
	char clBracket = (opBracket == '(') ? ')' : '}';

	char c;
	while ((c = readChar()) != 0)
	{
		if (c == opBracket)
			level++;
		else if (c == clBracket && --level <= 0)
			break;

		if (c != '\n' || !m_tokenString.isEmpty())
			m_tokenString.append(c);
	}
}

bool PgnStream::nextGame()
{
	char c;
	while ((c = readChar()) != 0)
	{
		if (c == '[')
		{
			rewindChar();
			m_phase = InTags;
			return true;
		}
		else
			skipSection(this, c);
	}

	return false;
}

PgnStream::TokenType PgnStream::readNext()
{
	if (m_phase == OutOfGame)
		return NoToken;

	m_tokenType = NoToken;
	m_tokenString.clear();

	char c;
	while ((c = readChar()) != 0)
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
		case '.':
			break;
		case '%':
			// Escape mechanism (skip this line)
			parseUntil("\n\r");
			m_tokenString.clear();
			break;
		case '[':
			if (m_phase != InTags)
			{
				rewindChar();
				m_phase = OutOfGame;
				return NoToken;
			}
			m_tokenType = PgnTag;
			parseTag();
			return m_tokenType;
		case '(':
		case '{':
			m_tokenType = PgnComment;
			parseComment(c);
			return m_tokenType;
		case ';':
			m_tokenType = PgnLineComment;
			parseUntil("\n\r");
			return m_tokenType;
		case '$':
			// NAG (Numeric Annotation Glyph)
			m_tokenType = PgnNag;
			parseUntil(" \t\n\r");
			return m_tokenType;
		case '*':
			// Unfinished game
			m_tokenType = PgnResult;
			m_tokenString = "*";
			m_phase = OutOfGame;
			return m_tokenType;
		case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case '0':
			// Move number or result
			m_tokenString.append(c);
			parseUntil(". \t\n\r");

			if (m_tokenString == "1-0"
			||  m_tokenString == "0-1"
			||  m_tokenString == "1/2-1/2")
			{
				m_tokenType = PgnResult;
				m_phase = OutOfGame;
			}
			else
			{
				if (m_tokenString.endsWith('.'))
					m_tokenString.chop(1);
				m_tokenType = PgnMoveNumber;
				m_phase = InGame;
			}
			return m_tokenType;
		default:
			m_tokenType = PgnMove;
			m_tokenString.append(c);
			parseUntil(" \t\n\r");
			m_phase = InGame;
			return m_tokenType;
		}
	}

	return NoToken;
}

QByteArray PgnStream::tokenString() const
{
	return m_tokenString;
}

PgnStream::TokenType PgnStream::tokenType() const
{
	return m_tokenType;
}

QByteArray PgnStream::tagName() const
{
	return m_tagName;
}

QByteArray PgnStream::tagValue() const
{
	return m_tagValue;
}

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

#include "pgngameentry.h"
#include "pgnstream.h"


PgnGameEntry::PgnGameEntry(const QString& variant)
	: m_round(0),
	  m_pos(0),
	  m_lineNumber(1),
	  m_variant(variant)
{

}

void PgnGameEntry::addTag(const QString& tagName, const QString& tagValue)
{
	if (tagName.isEmpty() || tagValue.isEmpty())
		return;

	if (tagName == "White")
		m_white = tagValue;
	else if (tagName == "Black")
		m_black = tagValue;
	else if (tagName == "Event")
		m_event = tagValue;
	else if (tagName == "Site")
		m_site = tagValue;
	else if (tagName == "Result")
		m_result = Chess::Result(tagValue);
	else if (tagName == "Variant")
		m_variant = tagValue;
	else if (tagName == "Round")
		m_round = tagValue.toInt();
}

void PgnGameEntry::clear()
{
	m_pos = 0;
	m_lineNumber = 1;
	m_round = 0;
	m_event.clear();
	m_site.clear();
	m_white.clear();
	m_black.clear();
	m_result = Chess::Result();
	m_variant = "Standard";
}

bool PgnGameEntry::read(PgnStream& in)
{
	QChar c;
	QString tagName;
	QString tagValue;
	bool haveTagName = false;
	bool foundTag = false;
	bool inTag = false;
	int bracketLevel = 0;

	while (!(c = in.readChar()).isNull())
	{
		if (!inTag)
		{
			if (c == '[')
			{
				inTag = true;
				if (!foundTag)
				{
					foundTag = true;
					m_pos = in.pos() - 1;
					m_lineNumber = in.lineNumber();
				}
			}
			else if (foundTag && !c.isSpace())
				return true;

			continue;
		}
		if (bracketLevel > 0 && c == ']')
			bracketLevel--;
		else if (c == ']' || c == '\n')
		{
			addTag(tagName, tagValue);
			tagName.clear();
			tagValue.clear();
			inTag = false;
			haveTagName = false;
			bracketLevel = 0;
			continue;
		}

		if (!haveTagName)
		{
			if (c == ' ')
				haveTagName = true;
			else
				tagName += c;
		}
		else if (c != '\"')
			tagValue += c;
		if (c == '[')
			bracketLevel++;
	}

	return foundTag;
}

qint64 PgnGameEntry::pos() const
{
	return m_pos;
}

qint64 PgnGameEntry::lineNumber() const
{
	return m_lineNumber;
}

QString PgnGameEntry::event() const
{
	return m_event;
}

QString PgnGameEntry::site() const
{
	return m_site;
}

int PgnGameEntry::round() const
{
	return m_round;
}

QString PgnGameEntry::white() const
{
	return m_white;
}

QString PgnGameEntry::black() const
{
	return m_black;
}

Chess::Result PgnGameEntry::result() const
{
	return m_result;
}

QString PgnGameEntry::variant() const
{
	return m_variant;
}

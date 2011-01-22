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
#include <cctype>
#include <QDataStream>
#include <QMap>
#include "pgnstream.h"


PgnStream& operator>>(PgnStream& in, PgnGameEntry& entry)
{
	entry.read(in);
	return in;
}

QDataStream& operator>>(QDataStream& in, PgnGameEntry& entry)
{
	entry.read(in);
	return in;
}

QDataStream& operator<<(QDataStream& out, const PgnGameEntry& entry)
{
	entry.write(out);
	return out;
}

PgnGameEntry::PgnGameEntry()
	: m_pos(0),
	  m_lineNumber(1)
{
}

void PgnGameEntry::addTag(const QByteArray& tagValue)
{
	int size = qMin(127, tagValue.size());

	m_data.append(char(size));
	m_data.append(tagValue.constData(), size);
}

void PgnGameEntry::clear()
{
	m_pos = 0;
	m_lineNumber = 1;
	m_data.clear();
}

static void skipSection(PgnStream& in, char type)
{
	char end;
	switch (type)
	{
	case '(':
		end = ')';
		break;
	case '{':
		end = '}';
		break;
	default:
		return;
	}

	int level = 1;
	char c;
	while ((c = in.readChar()) != 0)
	{
		if (c == end && --level == 0)
			break;
		if (c == type)
			level++;
	}
}

bool PgnGameEntry::read(PgnStream& in)
{
	char c;
	QByteArray tagName;
	QByteArray tagValue;
	QMap<QByteArray, QByteArray> tags;
	bool haveTagName = false;
	bool foundTag = false;
	bool inTag = false;
	bool inQuotes = false;

	clear();
	while ((c = in.readChar()) != 0)
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
			else if (foundTag && !isspace(c))
				break;
			else
				skipSection(in, c);

			continue;
		}

		if ((c == ']' && !inQuotes) || c == '\n' || c == '\r')
		{
			tags[tagName] = tagValue;
			tagName.clear();
			tagValue.clear();
			inTag = false;
			inQuotes = false;
			haveTagName = false;
			continue;
		}

		if (!haveTagName)
		{
			if (c == ' ')
				haveTagName = true;
			else
				tagName += c;
		}
		else if (c == '\"')
			inQuotes = !inQuotes;
		else if (inQuotes)
			tagValue += c;
	}

	addTag(tags["Event"]);
	addTag(tags["Site"]);
	addTag(tags["Date"]);
	addTag(tags["Round"]);
	addTag(tags["White"]);
	addTag(tags["Black"]);
	addTag(tags["Result"]);
	addTag(tags["Variant"]);

	return foundTag;
}

bool PgnGameEntry::read(QDataStream& in)
{
	// modifying this function can cause backward compatibility issues
	// in other programs.

	in >> m_pos;
	in >> m_lineNumber;
	in >> m_data;

	return in.status() == QDataStream::Ok;
}

void PgnGameEntry::write(QDataStream& out) const
{
	// modifying this function can cause backward compatibility issues
	// in other programs.

	out << m_pos;
	out << m_lineNumber;
	out << m_data;
}

qint64 PgnGameEntry::pos() const
{
	return m_pos;
}

qint64 PgnGameEntry::lineNumber() const
{
	return m_lineNumber;
}

QString PgnGameEntry::tagValue(TagType type) const
{
	int i = 0;
	for (int j = 0; j < type; j++)
		i += m_data[i] + 1;

	int size = m_data[i];
	if (size == 0)
		return QString();
	return m_data.mid(i + 1, size);
}

QString PgnGameEntry::event() const
{
	return tagValue(EventTag);
}

QString PgnGameEntry::site() const
{
	return tagValue(SiteTag);
}

QDate PgnGameEntry::date() const
{
	return QDate::fromString(tagValue(DateTag), "yyyy.MM.dd");
}

int PgnGameEntry::round() const
{
	return tagValue(RoundTag).toInt();
}

QString PgnGameEntry::white() const
{
	return tagValue(WhiteTag);
}

QString PgnGameEntry::black() const
{
	return tagValue(BlackTag);
}

Chess::Result PgnGameEntry::result() const
{
	return Chess::Result(tagValue(ResultTag));
}

QString PgnGameEntry::variant() const
{
	QString str = tagValue(VariantTag);
	if (str.isEmpty())
		return QString("standard");
	return str;
}

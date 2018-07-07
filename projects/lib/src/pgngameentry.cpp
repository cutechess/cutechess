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

#include "pgngameentry.h"
#include <cctype>
#include <QDataStream>
#include <QMap>
#include "pgnstream.h"
#include "pgngamefilter.h"

namespace {

int s_stringContains(const char* s1, const char* s2, int size)
{
	Q_ASSERT(s1 != nullptr);
	Q_ASSERT(s2 != nullptr);
	Q_ASSERT(size >= 0);

	if (!*s2)
		return 0;
	if (size == 0)
		return -1;

	const char* s1_end = s1 + size;

	while (s1 < s1_end)
	{
		if (toupper(*s1) == toupper(*s2))
		{
			const char* a = s1 + 1;
			const char* b = s2 + 1;

			while (*b && a < s1_end)
			{
				if (toupper(*a) != toupper(*b))
					break;
				a++;
				b++;
			}
			if (!*b)
				return b - s2;
			if (a == s1_end)
				return -1;
		}
		s1++;
	}

	return -1;
}

int s_stringToInt(const char *s, int size)
{
	int num = 0;
	for (int i = 0; i < size; i++)
	{
		if (!isdigit(s[i]))
			return 0;
		num = num * 10 + (s[i] - '0');
	}

	return num;
}

} // anonymous namespace

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

bool PgnGameEntry::match(const PgnGameFilter& filter) const
{
	const char* data = m_data.constData();

	if (filter.type() == PgnGameFilter::FixedString)
		return s_stringContains(data, filter.pattern(), m_data.size()) != -1;

	int whitePlayer = 0;

	int i = 0;
	for (int type = 0; type < 8; type++)
	{
		int size = data[i++];
		const char* str = data + i;

		switch (type)
		{
		case EventTag:
			if (s_stringContains(str, filter.event(), size) == -1)
				return false;
			break;
		case SiteTag:
			if (s_stringContains(str, filter.site(), size) == -1)
				return false;
			break;
		case DateTag:
			if (!filter.minDate().isNull() || !filter.maxDate().isNull())
			{
				if (size < 10)
					return false;

				int year = s_stringToInt(str, 4);
				if (year == 0)
					return false;
				int month = s_stringToInt(str + 5, 2);
				if (month == 0)
					month = 1;
				int day = s_stringToInt(str + 8, 2);
				if (day == 0)
					day = 1;

				QDate date(year, month, day);

				if ((!filter.minDate().isNull() && date < filter.minDate())
				||  (!filter.maxDate().isNull() && date > filter.maxDate()))
					return false;
			}
			break;
		case RoundTag:
			if (filter.minRound() != 0 || filter.maxRound() != 0)
			{
				int round = s_stringToInt(str, size);

				if (round == 0
				||  (filter.minRound() != 0 && round < filter.minRound())
				||  (filter.maxRound() != 0 && round > filter.maxRound()))
					return false;
			}
			break;
		case WhiteTag:
			{
				int len1 = -1;
				int len2 = -1;

				if (filter.playerSide() != Chess::Side::Black)
					len1 = s_stringContains(str, filter.player(), size);
				if (filter.playerSide() != Chess::Side::White)
					len2 = s_stringContains(str, filter.opponent(), size);

				if (len1 == -1 && len2 == -1)
					return false;
				whitePlayer = (len1 >= len2) ? 1 : 2;
			}
			break;
		case BlackTag:
			{
				int len1 = -1;
				int len2 = -1;

				if (filter.playerSide() != Chess::Side::White && whitePlayer != 1)
					len1 = s_stringContains(str, filter.player(), size);
				if (filter.playerSide() != Chess::Side::Black && whitePlayer != 2)
					len2 = s_stringContains(str, filter.opponent(), size);

				if (len1 == -1 && len2 == -1)
					return false;
			}
			break;
		case ResultTag:
		{
			if (filter.result() == PgnGameFilter::AnyResult)
				break;

			Chess::Result result(QString::fromLatin1(str, size));
			int winner = 0;

			if (!result.winner().isNull())
			{
				if (whitePlayer == 1)
					winner = result.winner() + 1;
				else
				{
					if (result.winner() == Chess::Side::White)
						winner = 2;
					else
						winner = 1;
				}
			}

			bool ok;
			switch (filter.result())
			{
			case PgnGameFilter::EitherPlayerWins:
				ok = !result.winner().isNull();
				break;
			case PgnGameFilter::WhiteWins:
				ok = result.winner() == Chess::Side::White;
				break;
			case PgnGameFilter::BlackWins:
				ok = result.winner() == Chess::Side::Black;
				break;
			case PgnGameFilter::FirstPlayerWins:
				ok = winner == 1;
				break;
			case PgnGameFilter::FirstPlayerLoses:
				ok = winner == 2;
				break;
			case PgnGameFilter::Draw:
				ok = result.isDraw();
				break;
			case PgnGameFilter::Unfinished:
				ok = result.isNone();
				break;
			default:
				ok = true;
				break;
			}

			if (ok == filter.isResultInverted())
				return false;
		}
			break;
		default:
			break;
		}
		i += size;
	}

	return true;
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

bool PgnGameEntry::read(PgnStream& in)
{
	if (!in.nextGame())
		return false;

	m_pos = in.pos();
	m_lineNumber = in.lineNumber();
	m_data.clear();

	char c;
	QByteArray tagName;
	QByteArray tagValue;
	QMap<QByteArray, QByteArray> tags;
	bool haveTagName = false;
	bool inTag = false;
	bool inQuotes = false;

	while ((c = in.readChar()) != 0)
	{
		if (!inTag)
		{
			if (c == '[')
				inTag = true;
			else if (!isspace(c))
			{
				in.rewindChar();
				break;
			}

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

	return true;
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

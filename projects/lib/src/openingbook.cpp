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

#include "openingbook.h"
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QtDebug>
#include "pgngame.h"
#include "pgnstream.h"
#include "mersenne.h"


QDataStream& operator>>(QDataStream& in, OpeningBook* book)
{
	while (in.status() == QDataStream::Ok)
	{
		quint64 key;
		OpeningBook::Entry entry = book->readEntry(in, &key);
		book->addEntry(entry, key);
	}

	return in;
}

QDataStream& operator<<(QDataStream& out, const OpeningBook* book)
{
	OpeningBook::Map::const_iterator it;
	for (it = book->m_map.constBegin(); it != book->m_map.constEnd(); ++it)
		book->writeEntry(it, out);

	return out;
}

OpeningBook::OpeningBook(AccessMode mode)
	: m_mode(mode)
{
}

OpeningBook::~OpeningBook()
{
}

bool OpeningBook::read(const QString& filename)
{
	m_filename = filename;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	if ((file.size() % entrySize()) != 0)
	{
		qWarning("Invalid size for opening book %s",
			 qUtf8Printable(filename));
		return false;
	}

	if (m_mode == Disk)
		return true;

	m_map.clear();
	QDataStream in(&file);
	in >> this;

	return !m_map.isEmpty();
}

bool OpeningBook::write(const QString& filename) const
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream out(&file);
	out << this;

	return true;
}

void OpeningBook::addEntry(const Entry& entry, quint64 key)
{
	Map::iterator it = m_map.find(key);
	while (it != m_map.end() && it.key() == key)
	{
		Entry& tmp = it.value();
		if (tmp.move == entry.move)
		{
			tmp.weight += entry.weight;
			return;
		}
		++it;
	}
	
	m_map.insert(key, entry);
}

int OpeningBook::import(const PgnGame& pgn, int maxMoves)
{
	Q_ASSERT(maxMoves > 0);

	Chess::Side winner(pgn.result().winner());
	int loserMod = -1;
	quint16 weight = 1;
	maxMoves = qMin(maxMoves, pgn.moves().size());
	int ret = maxMoves;

	if (!winner.isNull())
	{
		loserMod = int(pgn.startingSide() == winner);
		weight = 2;
		ret = (ret - loserMod) / 2 + loserMod;
	}

	const QVector<PgnGame::MoveData>& moves = pgn.moves();
	for (int i = 0; i < maxMoves; i++)
	{
		// Skip the loser's moves
		if ((i % 2) != loserMod)
		{
			Entry entry = { moves.at(i).move, weight };
			addEntry(entry, moves.at(i).key);
		}
	}

	return ret;
}

int OpeningBook::import(PgnStream& in, int maxMoves)
{
	Q_ASSERT(maxMoves > 0);

	if (!in.isOpen())
		return 0;

	int moveCount = 0;
	while (in.status() == PgnStream::Ok)
	{
		PgnGame game;
		game.read(in, maxMoves);
		if (game.moves().isEmpty())
			break;

		moveCount += import(game, maxMoves);
	}

	return moveCount;
}

QList<OpeningBook::Entry> OpeningBook::entriesFromDisk(quint64 key) const
{
	QList<Entry> entries;
	QFile file(m_filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		qWarning("Could not open book file %s",
			 qUtf8Printable(m_filename));
		return entries;
	}
	QDataStream in(&file);

	quint64 entryKey = 0;
	qint64 step = entrySize();
	qint64 n = file.size() / step;
	qint64 first = 0;
	qint64 last = n - 1;
	qint64 middle = (first + last) / 2;

	// Binary search
	while (first <= last)
	{
		qint64 pos = middle * step;
		file.seek(pos);
		Entry entry = readEntry(in, &entryKey);
		if (entryKey < key)
			first = middle + 1;
		else if (entryKey == key)
		{
			entries << entry;
			for (qint64 i = pos - step; i >= 0; i -= step)
			{
				file.seek(i);
				entry = readEntry(in, &entryKey);
				if (entryKey != key)
					break;
				entries << entry;
			}
			qint64 maxPos = (n - 1) * step;
			for (qint64 i = pos + step; i <= maxPos; i += step)
			{
				file.seek(i);
				entry = readEntry(in, &entryKey);
				if (entryKey != key)
					break;
				entries << entry;
			}
			return entries;
		}
		else
			last = middle - 1;
		middle = (first + last) / 2;
	}

	return entries;
}

QList<OpeningBook::Entry> OpeningBook::entries(quint64 key) const
{
	if (m_mode == Ram)
		return m_map.values(key);
	return entriesFromDisk(key);
}

Chess::GenericMove OpeningBook::move(quint64 key) const
{
	Chess::GenericMove move;
	
	// There can be multiple entries/moves with the same key.
	// We need to find them all to choose the best one
	const auto entries = this->entries(key);
	if (entries.isEmpty())
		return move;
	
	// Calculate the total weight of all available moves
	int totalWeight = 0;
	for (const Entry& entry : entries)
		totalWeight += entry.weight;
	if (totalWeight <= 0)
		return move;

	// Pick a move randomly, with the highest-weighted move having
	// the highest probability of getting picked.
	int pick = Mersenne::random() % totalWeight;
	int currentWeight = 0;
	for (const Entry& entry : entries)
	{
		currentWeight += entry.weight;
		if (currentWeight > pick)
			return entry.move;
	}
	
	return move;
}

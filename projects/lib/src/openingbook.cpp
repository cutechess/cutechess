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

#include "openingbook.h"
#include <QString>
#include <QFile>
#include <QDataStream>
#include "pgngame.h"
#include "pgnstream.h"


QDataStream& operator>>(QDataStream& in, OpeningBook* book)
{
	while (!in.status() != QDataStream::Ok)
		book->loadEntry(in);

	return in;
}

QDataStream& operator<<(QDataStream& out, const OpeningBook* book)
{
	OpeningBook::Map::const_iterator it;
	for (it = book->m_map.constBegin(); it != book->m_map.constEnd(); ++it)
		book->saveEntry(it, out);

	return out;
}

bool OpeningBook::read(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

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
	int weight = 1;
	maxMoves = qMin(maxMoves, pgn.moves().size());

	if (!winner.isNull())
	{
		loserMod = int(pgn.startingSide() == winner);
		weight = 2;
	}

	const QVector<PgnGame::MoveData>& moves = pgn.moves();
	for (int i = 0; i < maxMoves; i++)
	{
		// Skip the loser's moves
		if ((i % 2) == loserMod)
		{
			Entry entry = { moves[i].move, weight };
			addEntry(entry, moves[i].key);
		}
	}

	return maxMoves;
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

Chess::GenericMove OpeningBook::move(quint64 key) const
{
	Chess::GenericMove move;
	
	// There can be multiple entries/moves with the same key.
	// We need to find them all to choose the best one
	QList<Entry> entries = m_map.values(key);
	if (entries.size() == 0)
		return move;
	
	// Calculate the total weight of all available moves
	int totalWeight = 0;
	foreach (const Entry& entry, entries)
		totalWeight += entry.weight;
	if (totalWeight <= 0)
		return move;

	// Pick a move randomly, with the highest-weighted move having
	// the highest probability of getting picked.
	int pick = qrand() % totalWeight;
	int currentWeight = 0;
	foreach (const Entry& entry, entries)
	{
		currentWeight += entry.weight;
		if (currentWeight > pick)
			return entry.move;
	}
	
	return move;
}

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
#include "chessboard/chessboard.h"
#include "pgngame.h"
#include "pgnfile.h"


void OpeningBook::addEntry(const BookMove& move, quint64 key)
{
	Map::iterator it = m_map.find(key);
	while (it != m_map.end() && it.key() == key)
	{
		Entry& entry = it.value();
		if (entry.move == move)
		{
			entry.weight++;
			return;
		}
		++it;
	}
	
	Entry entry = { move, 1 };
	m_map.insert(key, entry);
}

bool OpeningBook::load(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QDataStream in(&file);
	
	m_map.clear();
	while (!in.atEnd())
	{
		loadEntry(in);
		if (in.status() != QDataStream::Ok)
		{
			m_map.clear();
			return false;
		}
	}
	
	return true;
}

bool OpeningBook::save(const QString& filename) const
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QDataStream out(&file);
	
	Map::const_iterator it;
	for (it = m_map.constBegin(); it != m_map.constEnd(); ++it)
		saveEntry(it, out);
	
	return true;
}

bool OpeningBook::pgnImport(const QString& filename, int maxMoves)
{
	using namespace Chess;
	
	Q_ASSERT(maxMoves > 0);
	
	PgnFile in(filename, variant());
	if (!in.isOpen())
		return false;
	Chess::Board* board = in.board();
	
	int gameCount = 0;
	int moveCount = 0;
	while (in.status() == QTextStream::Ok)
	{
		PgnGame game(in, maxMoves);
		if (game.isEmpty())
			break;
		
		board->setBoard(game.startingFen());
		const QVector<Chess::Move>& moves = game.moves();
		foreach (const Move& srcMove, moves)
		{
			Square src = board->chessSquare(srcMove.sourceSquare());
			Square trg = board->chessSquare(srcMove.targetSquare());
			int prom = srcMove.promotion();

			addEntry(BookMove(src, trg, prom), board->key());
			board->makeMove(srcMove);
		}
		moveCount += moves.size();
		gameCount++;
	}
	qDebug("Imported %d moves from %d games", moveCount, gameCount);
	
	return true;
}

BookMove OpeningBook::move(quint64 key) const
{
	BookMove move;
	
	// There can be multiple entries/moves with the same key.
	// We need to find the all to choose the best one
	QList<Entry> entries = m_map.values(key);
	if (entries.size() == 0)
		return move;
	
	// Calculate the total weight of all available moves
	int totalWeight = 0;
	foreach (const Entry& entry, entries)
		totalWeight += entry.weight;
	
	// Pick a move randomly, with the highest-weighted move having
	// the highest probability of getting being picked.
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

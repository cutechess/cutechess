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
		PgnGame game;
		game.load(in, maxMoves);
		if (game.isEmpty())
			break;
		
		board->setBoard(game.startingFen());
		const QVector<PgnGame::MoveData>& moves = game.moves();

		Chess::Side winner = game.result().winner();
		int weight = (winner == Chess::NoSide) ? 1 : 2;
		
		foreach (const PgnGame::MoveData& md, moves)
		{
			Chess::Move srcMove = md.move;

			// Skip the loser's moves
			if (winner == Chess::NoSide || winner == board->sideToMove())
			{
				Square src = board->chessSquare(srcMove.sourceSquare());
				Square trg = board->chessSquare(srcMove.targetSquare());
				int prom = srcMove.promotion();
				Entry entry = { BookMove(src, trg, prom), weight };
				addEntry(entry, board->key());
			}

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

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

#include "ncheckboard.h"


namespace Chess {

/*! Maximum feasible value \a maxNCheck for nth-check limit. */
static const int maxNCheck = 99;

NCheckBoard::NCheckBoard(int n)
	: StandardBoard(),
	  m_checkLimit(qBound(1, n, maxNCheck))
{
}

Board* NCheckBoard::copy() const
{
	return new NCheckBoard(*this);
}

QString NCheckBoard::variant() const
{
	return QString("%1check").arg(checkLimit());
}

QString NCheckBoard::defaultFenString() const
{
	return QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - %1+%1 0 1")
	.arg(checkLimit());
}

void NCheckBoard::vInitialize()
{
	m_checksToWin[Side::White] = m_checksToWin[Side::Black] = checkLimit();
	StandardBoard::vInitialize();
}

void NCheckBoard::updateCheckCounters(Side side, int d = -1)
{
	Q_ASSERT(m_checksToWin[Side::White] >=0);
	Q_ASSERT(m_checksToWin[Side::Black] >=0);
	Q_ASSERT(side != Side::NoSide);

	if (inCheck(side.opposite()))
		m_checksToWin[side] += d;
}

void NCheckBoard::vMakeMove(const Move& move,
			    BoardTransition* transition)
{
	StandardBoard::vMakeMove(move, transition);
	// If opponent in check then decrease counter of this side
	updateCheckCounters(sideToMove(), -1);
}

void NCheckBoard::vUndoMove(const Move& move)
{
	// If undoing checking move then increase counter of this side
	updateCheckCounters(sideToMove(), +1);
	StandardBoard::vUndoMove(move);
}

Result NCheckBoard::result()
{
	// Side wins if counter is zero
	Side opp = sideToMove().opposite();
	if (m_checksToWin[opp] == 0)
		return Result(Result::Win, opp,
			      tr("%1 checks %2 times")
			      .arg(opp.toString()).arg(checkLimit()));

	return StandardBoard::result();
}

inline int NCheckBoard::checkLimit() const
{
	return m_checkLimit;
}

void NCheckBoard::setChecksToWin(int whiteCount, int blackCount)
{
	m_checksToWin[Side::White] = qBound(0, whiteCount, checkLimit());
	m_checksToWin[Side::Black] = qBound(0, blackCount, checkLimit());
}

int NCheckBoard::checksToWin(Side side) const
{
	if (side == Side::White)
		return m_checksToWin[Side::White];
	if (side == Side::Black)
		return m_checksToWin[Side::Black];
	return -1;
}

/*!
 * A format to extend FEN for N-Check was suggested by H. G. Muller:
 * The numbers of checks to win the game are given for both sides in succession
 * of the the en passant field. They are separated by a single plus sign "+"
 * (check symbol) without any other character in between (like 3+3).
 * In contrast SCIDB and Lichess use two plus signs and *forward counters*
 * (like +0+0) appended to the normal FEN.
 */
QString NCheckBoard::vFenIncludeString(Board::FenNotation notation) const
{
	Q_UNUSED(notation);

	QString fen = QString(" %1+%2")
	.arg(m_checksToWin[Side::White])
	.arg(m_checksToWin[Side::Black]);

	return fen;
}

 /*!
  * This method extracts check counters from extended FEN (like Xboard /
  * Stockfish and SCIDB / Lichess formats) and is tolerant to standard FEN.
  * If no counters can be extracted (e.g. from a standard FEN) the number of
  * checks to win will be set to checkLimit.
  * Calls StandardBoard method for FEN handling.
  */
bool NCheckBoard::vSetFenString(const QStringList& fen)
{
	if (fen.count() < 2)
		return false;

	// Make a copy to modify and to forward to parent's method
	QStringList sfen(fen);

	// Reset counters, do not rely on getting check counters from PGN
	setChecksToWin(checkLimit(), checkLimit());
	// Try to extract and remove the check count field
	for (const QString& field: fen)
	{
		if (field.contains('+'))
		{
			int marker = field.lastIndexOf('+');
			int counterW = field.leftRef(marker).toInt();
			int counterB = field.midRef(marker+1).toInt();

			if (counterW < 0 || counterW > maxNCheck
			||  counterB < 0 || counterB > maxNCheck)
				continue;

			if (field.count('+') == 1)
				// reverse counter style (like 3+3)
				setChecksToWin(counterW, counterB);
			else
				// scidb style forward counter +0+0
				setChecksToWin(checkLimit() - counterW,
					       checkLimit() - counterB);
			sfen.removeOne(field);
			break;
		}
	}
	// correction for FENs w/o check counters if king is in check
	if (sfen == fen)
		updateCheckCounters(sideToMove().opposite(), -1);

	// Let StandardBoard do the rest
	return StandardBoard::vSetFenString(sfen);
}

ThreeCheckBoard::ThreeCheckBoard() : NCheckBoard(3) {}

Board * ThreeCheckBoard::copy() const
{
	return new ThreeCheckBoard(*this);
}

FiveCheckBoard::FiveCheckBoard() : NCheckBoard(5) {}

Board * FiveCheckBoard::copy() const
{
	return new FiveCheckBoard(*this);
}

} // namespace Chess

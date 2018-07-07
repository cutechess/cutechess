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

#include "antiboard.h"

namespace Chess {

AntiBoard::AntiBoard()
	: StandardBoard(),
	m_testKey(0),
	m_canCapture(false)
{
}

Board* AntiBoard::copy() const
{
	return new AntiBoard(*this);
}

QString AntiBoard::variant() const
{
	return "antichess";
}

QString AntiBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1";
}

bool AntiBoard::hasCastling() const
{
	return false;
}

bool AntiBoard::kingsCountAssertion( int whiteKings,
				     int blackKings) const
{
	/*
	 * Kings can be captured and pawns can promote to kings.
	 */
	return whiteKings + blackKings >= 0;
}

void AntiBoard::addPromotions(int sourceSquare,
				int targetSquare,
				QVarLengthArray<Move>& moves) const
{
	StandardBoard::addPromotions(sourceSquare, targetSquare, moves);
	moves.append(Move(sourceSquare, targetSquare, King));
}

bool AntiBoard::vSetFenString(const QStringList& fen)
{
	m_canCapture = false;
	m_testKey = 0;
	return StandardBoard::vSetFenString(fen);
}

bool AntiBoard::inCheck(Side, int) const
{
	return false;
}

bool AntiBoard::vIsLegalMove(const Move& move)
{
	if (!StandardBoard::vIsLegalMove(move))
		return false;

	if (captureType(move) != Piece::NoPiece)
		return true;

	//try to use old results to gain higher efficiency
	if (key() != m_testKey)
	{
		m_testKey = key();
		m_canCapture = false;
		QVarLengthArray<Move> moves;
		generateMoves(moves);

		// search for any legal capture move
		for (int i = 0; i < moves.size(); i++)
		{
			if (captureType(moves[i]) != Piece::NoPiece
			&&  StandardBoard::vIsLegalMove(moves[i]))
			{
				m_canCapture = true;
				break;
			}
		}
	}
	// move is illegal if a capture move exists else legal
	return !m_canCapture;
}

Result AntiBoard::vResultOfStalemate() const
{
	Side winner = sideToMove();
	QString str { tr("%1 wins").arg(winner.toString()) };
	return Result(Result::Win, winner, str);
}

Result AntiBoard::result()
{
	QString str;
	// stalemate or no pieces left
	if (!canMove())
		return vResultOfStalemate();

	// 50 move rule
	if (reversibleMoveCount() >= 100)
	{
		str = tr("Draw by fifty moves rule");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		str = tr("Draw by 3-fold repetition");
		return Result(Result::Draw, Side::NoSide, str);
	}

	return Result();
}

} // namespace Chess

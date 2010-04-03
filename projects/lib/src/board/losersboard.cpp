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

#include "losersboard.h"
#include "westernzobrist.h"

namespace Chess {

REGISTER_BOARD(LosersBoard, "losers")

LosersBoard::LosersBoard(QObject* parent)
	: WesternBoard(new WesternZobrist(), parent),
	  m_canCapture(false),
	  m_captureKey(0)
{
}

QString LosersBoard::variant() const
{
	return "losers";
}

QString LosersBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool LosersBoard::vSetFenString(const QStringList& fen)
{
	m_canCapture = false;
	m_captureKey = 0;
	return WesternBoard::vSetFenString(fen);
}

bool LosersBoard::vIsLegalMove(const Move& move)
{
	bool isCapture = (captureType(move) != Piece::NoPiece);
	if (m_captureKey != key()
	&&  !isCapture)
	{
		m_captureKey = key();
		m_canCapture = false;

		QVarLengthArray<Move> moves;
		generateMoves(moves);

		for (int i = 0; i < moves.size(); i++)
		{
			if (captureType(moves[i]) != Piece::NoPiece
			&&  WesternBoard::vIsLegalMove(moves[i]))
			{
				m_canCapture = true;
				break;
			}
		}
	}

	if (!isCapture && m_canCapture)
		return false;
	return WesternBoard::vIsLegalMove(move);
}

Result LosersBoard::result()
{
	Side winner = NoSide;
	QString str;

	// Checkmate/Stalemate
	if (!canMove())
	{
		winner = sideToMove();
		str = tr("%1 gets mated").arg(sideString(winner));
		return Result(Result::Win, winner, str);
	}

	// Lost all pieces
	int pieceCount = 0;
	for (int i = 0; i < arraySize(); i++)
	{
		if (pieceAt(i).side() == sideToMove()
		&&  ++pieceCount > 1)
			break;
	}
	if (pieceCount <= 1)
	{
		winner = sideToMove();
		str = tr("%1 lost all pieces").arg(sideString(winner));
		return Result(Result::Win, winner, str);
	}

	// 50 move rule
	if (reversibleMoveCount() >= 100)
	{
		str = tr("Draw by fifty moves rule");
		return Result(Result::Draw, NoSide, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		str = tr("Draw by 3-fold repetition");
		return Result(Result::Draw, NoSide, str);
	}

	return Result();
}

} // namespace Chess

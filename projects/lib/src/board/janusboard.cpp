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

#include "janusboard.h"

namespace Chess {

JanusBoard::JanusBoard()
	: WesternBoard(new WesternZobrist())
{
	// Janus uses notation "J" and the graphical image of Archbishop "A"
	setPieceType(Janus, tr("janus"), "J", KnightMovement | BishopMovement, "A");
}

Board* JanusBoard::copy() const
{
	return new JanusBoard(*this);
}

QString JanusBoard::variant() const
{
	return "janus";
}

int JanusBoard::width() const
{
	return 10;
}

QString JanusBoard::defaultFenString() const
{
	return "rjnbkqbnjr/pppppppppp/10/10/10/10/PPPPPPPPPP/RJNBKQBNJR w KQkq - 0 1";
}

void JanusBoard::addPromotions(int sourceSquare,
				int targetSquare,
				QVarLengthArray<Move>& moves) const
{
	WesternBoard::addPromotions(sourceSquare, targetSquare, moves);
	moves.append(Move(sourceSquare, targetSquare, Janus));
}

int JanusBoard::castlingFile(CastlingSide castlingSide) const
{
	Q_ASSERT(castlingSide != NoCastlingSide);
	return castlingSide == QueenSide ? 1 : width() - 2; // B-File and I-File
}

QString JanusBoard::sanMoveString(const Move& move)
{
	QString san = WesternBoard::sanMoveString(move);

	if (!san.startsWith("O-O"))
		return san;

	// uses Kb1/Kb8/Ki1/Ki8 for castling
	QString sym = san.right(1);
	if (sym != "+" && sym != "#")
		sym.clear();
	return pieceSymbol(King).toUpper() + lanMoveString(move).mid(2) + sym;
}


Move JanusBoard::moveFromSanString(const QString& str)
{
	/*
	 * accepts O-O and Kb1/Kb8/Ki1/Ki8 formats for castling
	 * Xboard uses O-O for B-file and Ki1/Ki8 for I-file castling
	 */
	if (str.startsWith("O-O-O"))
		return WesternBoard::moveFromSanString("O-O");
	if (str.startsWith("O-O"))
		return WesternBoard::moveFromSanString("O-O-O");

	if (!str.startsWith(pieceSymbol(King).toUpper()))
		return WesternBoard::moveFromSanString(str);  //main path

	if (hasCastlingRight(sideToMove(), KingSide))
	{
		Move castlingMove = WesternBoard::moveFromSanString("O-O");
		if (!castlingMove.isNull() && str == sanMoveString(castlingMove))
			return castlingMove;
	}
	if (hasCastlingRight(sideToMove(), QueenSide))
	{
		Move castlingMove = WesternBoard::moveFromSanString("O-O-O");
		if (!castlingMove.isNull() && str == sanMoveString(castlingMove))
			return castlingMove;
	}

	return WesternBoard::moveFromSanString(str); // normal king moves
}

} // namespace Chess

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

#include "courierboard.h"

namespace Chess {

CourierBoard::CourierBoard()
	: ShatranjBoard()
{
	// Use Bishop's and Queen's names but set to old movements:
	// War Elephant (Alfil) and Counselor (Ferz)
	setPieceType(Bishop, tr("bishop"), "E", AlfilMovement);
	setPieceType(Queen, tr("queen"), "F", FerzMovement);
	// Introduce Courier, the modern Bishop piece
	setPieceType(Courier, tr("courier"), "B", BishopMovement);
	// Mann moves like King but is no royal piece
	setPieceType(Mann, tr("mann"), "M", FerzMovement | WazirMovement);
	// Schleich moves as Wazir
	setPieceType(Schleich, tr("schleich"), "W", WazirMovement);
	// King, Rook, Knight and Pawn as in standard chess
	// but without special moves: pawn double step, en passant and castling
}

Board* CourierBoard::copy() const
{
	return new CourierBoard(*this);
}

QString CourierBoard::variant() const
{
	return "courier";
}

int CourierBoard::width() const
{
	return 12;
}

/*
 * Basic starting position:
 * rnebmkfwbenr/pppppppppppp/12/12/12/12/PPPPPPPPPPPP/RNEBMKFWBENR w - - 0 1
 * Traditional effective starting position after ceremonial moves of rook and
 * queen pawns and of the queens all by two squares:
 * rnebmk1wbenr/1ppppp1pppp1/6f5/p5p4p/P5P4P/6F5/1PPPPP1PPPP1/RNEBMK1WBENR w - - 0 1
 */
QString CourierBoard::defaultFenString() const
{
	return "rnebmk1wbenr/1ppppp1pppp1/6f5/p5p4p/"
	"P5P4P/6F5/1PPPPP1PPPP1/RNEBMK1WBENR w - - 0 1";
}

void CourierBoard::vInitialize()
{
	ShatranjBoard::vInitialize();

	int arrWidth = width() + 2;

	m_wazirOffsets.resize(4);
	m_wazirOffsets[0] = -arrWidth;
	m_wazirOffsets[1] = -1;
	m_wazirOffsets[2] = 1;
	m_wazirOffsets[3] = arrWidth;
}

void CourierBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	Chess::ShatranjBoard::generateMovesForPiece(moves, pieceType, square);
	if (pieceHasMovement(pieceType, WazirMovement))
		generateHoppingMoves(square, m_wazirOffsets, moves);
}

bool CourierBoard::inCheck(Side side, int square) const
{
	Piece piece;
	Side opSide = side.opposite();
	if (square == 0)
		square = kingSquare(side);

	// Wazir attacks by Schleich, Mann
	for (int i = 0; i < m_wazirOffsets.size(); i++)
	{
		piece = pieceAt(square + m_wazirOffsets[i]);
		if (piece.side() == opSide
		&&  pieceHasMovement(piece.type(), WazirMovement))
			return true;
	}
	return ShatranjBoard::inCheck(side, square);
}

} // namespace Chess

/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include <QString>
#include "chesspiece.h"


/**
 * The ChessMove class represents a simple chess move with minimal information
 * about the move. A Chessboard object is needed to verify the move's legality
 * or to convert it to a string.
 * @see Chessboard
 */
class ChessMove
{
public:
	ChessMove();
	
	ChessMove(int sourceSquare,
	          int targetSquare,
	          ChessPiece::PieceType promotion = ChessPiece::PT_None);
	
	/**
	 * The source square.
	 */
	int sourceSquare() const;

	/**
	 * The target square.
	 */
	int targetSquare() const;

	/**
	 * The type of the promotion piece.
	 */
	ChessPiece::PieceType promotion() const;

	bool isEmpty() const;

protected:
	int m_sourceSquare;
	int m_targetSquare;
	ChessPiece::PieceType m_promotion;
	bool m_isEmpty;
};

#endif // CHESSMOVE_H

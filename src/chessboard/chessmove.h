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

#include <QtGlobal>
#include <QString>

#include "chessboard.h"


/**
 * The ChessMove class represents a simple chess move with minimal information
 * about the move. A Chessboard object is needed to verify the move's legality
 * or to convert it to a string.
 * @see Chessboard
 */
class ChessMove
{
public:
	/**
	 * Constructs an empty chessmove.
	 */
	ChessMove();
	
	/**
	 * Creates a ChessMove object.
	 * @param sourceSquare the source square.
	 * @param targetSquare the target square.
	 * @param promotion the promotion piece.
	 */
	ChessMove(Chessboard::ChessSquare sourceSquare,
	          Chessboard::ChessSquare targetSquare,
	          Chessboard::ChessPiece promotion=Chessboard::NoPiece);
	
	/**
	 * The source square.
	 */
	Chessboard::ChessSquare sourceSquare() const;

	/**
	 * The target square.
	 */
	Chessboard::ChessSquare targetSquare() const;

	/**
	 * The type of the promotion piece.
	 */
	Chessboard::ChessPiece promotion() const;
	
private:
	Chessboard::ChessSquare m_sourceSquare;
	Chessboard::ChessSquare m_targetSquare;
	Chessboard::ChessPiece m_promotion;
};

#endif // CHESSMOVE_H


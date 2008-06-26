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

class ChessMove
{
public:
	/**
	 * Constructs an empty chessmove.
	 */
	ChessMove();
	
	/**
	 * Constructs a chessmove and verifies its legality.
	 * @param board a chessboard to verify the move against.
	 * @param sourceSquare the source square.
	 * @param targetSquare the target square.
	 * @param promotion the promotion piece.
	 */
	ChessMove(Chessboard *board,
	          Chessboard::ChessSquare sourceSquare,
	          Chessboard::ChessSquare targetSquare,
	          Chessboard::ChessPiece promotion=Chessboard::NoPiece);
	
	/**
	 * Constructs a chessmove and verifies its legality.
	 * @param board a chessboard to verity the move against.
	 * @param move a move in integer format.
	 */
	ChessMove(Chessboard *board, quint32 move);
	
	/**
	 * Constructs a chessmove and verifies its legality.
	 * @param board a chessboard to verify the move against.
	 * @param moveString a move string in SAN or coordinate notation
	 */
	ChessMove(Chessboard *board, const QString& moveString);

	/**
	 * Gets the source square.
	 * @return the source square.
	 */
	Chessboard::ChessSquare sourceSquare() const;

	/**
	 * Gets the target square.
	 * @return the target square.
	 */
	Chessboard::ChessSquare targetSquare() const;

	/**
	 * Gets the type of the moving piece.
	 * @return type of the moving piece.
	 */
	Chessboard::ChessPiece movingPiece() const;

	/**
	 * Gets the type of the promotion piece.
	 * @return type of the promotion piece.
	 */
	Chessboard::ChessPiece promotion() const;

	/**
	 * Gets the type of the captured piece.
	 * @return type of the captured piece.
	 */
	Chessboard::ChessPiece capture() const;

	/**
	 * Gets the move in integer format.
	 * @return the move in integer format.
	 */
	quint32 integerFormat() const;

	/**
	 * Gets the move string in SAN.
	 * @return the move string in SAN.
	 */
	QString sanMoveString() const;

	/**
	 * Gets the move string in coordinate notation.
	 * @return the move string in coordinate notation.
	 */
	QString coordMoveString() const;

private:
	quint32 m_move;
	QString m_sanMoveString;
	QString m_coordMoveString;
};

#endif // CHESSMOVE_H


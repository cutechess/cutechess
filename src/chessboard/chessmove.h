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
	ChessMove();
	ChessMove(Chessboard *board,
	          Chessboard::ChessSquare sourceSquare,
	          Chessboard::ChessSquare targetSquare,
	          Chessboard::ChessPiece promotion=Chessboard::NoPiece);

	Chessboard::ChessSquare sourceSquare() const;
	Chessboard::ChessSquare targetSquare() const;
	Chessboard::ChessPiece movingPiece() const;
	Chessboard::ChessPiece promotion() const;
	Chessboard::ChessPiece capture() const;

	quint32 integerFormat() const;
	QString sanMoveString() const;
	QString coordMoveString() const;

private:
	quint32 m_move;
	QString m_sanMoveString;
	QString m_coordMoveString;
};

#endif // CHESSMOVE_H


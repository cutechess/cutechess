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

#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <QtGlobal>
#include "chessboard.h"
#include "chesspiece.h"

class Zobrist
{
	public:
		static void initialize();
		static quint64 key(const Chessboard* board);
		
		static quint64 side();
		static quint64 enpassant(int square);
		static quint64 castling(Chessboard::ChessSide side, int castlingSide);
		static quint64 piece(Chessboard::ChessSide side, ChessPiece::PieceType type, int square);

		static const int MaxSquares = 120;
		static const int MaxPieceTypes = 16;
	private:
		Zobrist();
		
		static int random();
		static quint64 random64();
		static int m_randomSeed;
		
		static quint64 m_side;
		static quint64 m_enpassant[MaxSquares];
		static quint64 m_castling[2][2];
		static quint64 m_piece[2][MaxPieceTypes][MaxSquares];
};

#endif // ZOBRIST

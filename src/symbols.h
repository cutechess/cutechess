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

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <QString>

#include "chessboard/chessboard.h"

/**
 * Symbols class provides chess symbols found in the Unicode character set.
*/
class Symbols
{
	public:
		/** Symbol of black pawn. */
		static const QString BlackPawn;
		/** Symbol of black rook. */
		static const QString BlackRook;
		/** Symbol of black knight. */
		static const QString BlackKnight;
		/** Symbol of black bishop. */
		static const QString BlackBishop;
		/** Symbol of black queen. */
		static const QString BlackQueen;
		/** Symbol of black king. */
		static const QString BlackKing;

		/** Symbol of white pawn. */
		static const QString WhitePawn;
		/** Symbol of white rook. */
		static const QString WhiteRook;
		/** Symbol of white knight. */
		static const QString WhiteKnight;
		/** Symbol of white bishop. */
		static const QString WhiteBishop;
		/** Symbol of white queen. */
		static const QString WhiteQueen;
		/** Symbol of white king. */
		static const QString WhiteKing;

		/**
		 * Returns a symbol of given side and piece type.
		 * @param side The side of the wanted symbol.
		 * @param piece The piece type of the wanted symbol.
		 * @return The symbol of the given side and piece type. If the given
		 * values are invalid an empty string is returned.
		*/
		static QString symbolOf(Chessboard::ChessSide side, Chessboard::ChessPiece piece);

		/**
		 * Returns a symbol of given side and piece type.
		 * The piece type is given as a single character. The mappings to
		 * chess pieces are:
		 *
		 * K - King
		 * Q - Queen
		 * B - Bishop
		 * N - Knight
		 * R - Rook
		 * P - Pawn
		 *
		 * Uppercase and lowercase letters give same results.
		 * @param side The side of the wanted symbol.
		 * @param piece The piece type of the wanted symbol (as a single
		 * character).
		 * @return The symbol of the given side and piece type. If the given
		 * values are invalid an empty string is returned.
		*/
		static QString symbolOf(Chessboard::ChessSide side, const QChar& piece);
	
	private:
		Symbols() { }

};

#endif // SYMBOLS_H


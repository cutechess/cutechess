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

#ifndef MODERNBOARD_H
#define MODERNBOARD_H

#include "capablancaboard.h"

namespace Chess {

/*!
 * \brief A board for Modern chess
 *
 * Modern chess is a variant of chess on a 9x9 board. Each side has an
 * additional Pawn and a Minister piece with Bishop and Knight movements.
 * At start the Kings have the Queens to their right (f1, d9) and the
 * Ministers to their left.
 *
 * The King moves two squares towards the rook when castling. Pawns may also
 * promote to Minister.
 *
 * This variant was introduced in 1964 (Madrid) by Gabriel Maura, Puerto Rico.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Modern_Chess_(chess_variant)
 *
 * \sa CapablancaBoard
 * \sa ChancellorBoard
 * TODO: clarify FEN notation
 */
class LIB_EXPORT ModernBoard : public CapablancaBoard
{
	public:
		/*! Creates a new ModernBoard object. */
		ModernBoard();

		// Inherited from CapablancaBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;
		virtual int height() const;
		virtual int castlingFile(CastlingSide castlingSide) const;
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromSanString(const QString& str);
};

} // namespace Chess
#endif // MODERNBOARD_H

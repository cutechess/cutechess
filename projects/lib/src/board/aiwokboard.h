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

#ifndef AIWOKBOARD_H
#define AIWOKBOARD_H

#include "makrukboard.h"

namespace Chess {

/*!
 * \brief A board for Ai-Wok
 *
 * Ai-Wok is a variant of Makruk where the Advisor (Met) is replaced by
 * the Ai-Wok which has powers of Ferz (Met), Knight (Ma) and Rook (Rua)
 *
 * \note Rules:
 */
class LIB_EXPORT AiWokBoard : public MakrukBoard
{
	public:
		/*! Creates a new AiWokBoard object. */
		AiWokBoard();

		// Inherited from MakrukBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Ai-Wok replaces Advisor (Met, Ferz). */
		enum AiWokPieceType
		{
			AiWok = Met	//!< Ai-Wok (ferz + knight + rook)
		};
		// Inherited from MakrukBoard
		virtual bool insufficientMaterial() const;
};

} // namespace Chess
#endif // AIWOKBOARD_H

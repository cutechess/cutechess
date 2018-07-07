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

#ifndef MOVENUMBERTOKEN_H
#define MOVENUMBERTOKEN_H

#include "pgntoken.h"

/*!
 * \brief A fullmove number token in a PGN game.
 */
class MoveNumberToken : public PgnToken
{
	public:
		/*!
		 * Creates a new MoveNumberToken at \a ply for a game started
		 * by \a startingSide.
		 */
		MoveNumberToken(int ply, int startingSide);

		// Inherited from PgnToken
		virtual QString toString() const;

	protected:
		// Inherited from PgnToken
		virtual void vInsert(QTextCursor& cursor);

	private:
		int m_ply;
		int m_startingSide;
};

#endif // MOVENUMBERTOKEN_H

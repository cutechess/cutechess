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

#ifndef ATOMICBOARD_H
#define ATOMICBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Atomic chess
 *
 * Atomic chess is a variant of standard chess where captures result
 * in an explosion on the target square.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Atomic_chess
 */
class LIB_EXPORT AtomicBoard : public WesternBoard
{
	public:
		/*! Creates a new AtomicBoard object. */
		AtomicBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		// Inherited from WesternBoard
		virtual void vInitialize();
		virtual bool inCheck(Side side, int square = 0) const;
		virtual bool kingCanCapture() const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual bool vIsLegalMove(const Move& move);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);

	private:
		struct MoveData
		{
			bool isCapture;
			Piece piece;
			Piece captures[8];
		};

		QVector<MoveData> m_history;
		int m_offsets[8];
};

} // namespace Chess
#endif // ATOMICBOARD_H

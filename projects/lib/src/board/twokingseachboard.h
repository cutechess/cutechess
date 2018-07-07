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

#ifndef TWOKINGSEACHBOARD_H
#define TWOKINGSEACHBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Two Kings Each Chess (or Wild 9)
 *
 * Two Kings Each is a variant of standard chess with two kings on both
 * sides. The kings' roles depend on the position. Only the king that is
 * closer to the a-file is royal and it is exposed to check and mate.
 * If the kings are on the same file the king closer to (White's) first
 * rank is royal. The other king is a normal piece and can be captured.
 * Giving mate to the royal king wins.
 *
 * Introduced on the ICC as wild/9.
 * \note Rules: https://www.chessclub.com/user/help/wild9
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 */
class LIB_EXPORT TwoKingsEachBoard : public WesternBoard
{
	public:
		/*! Creates a new TwoKingsEachBoard object. */
		TwoKingsEachBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();
	protected:
		virtual void vInitialize();
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
		virtual Move moveFromLanString(const QString& str);
		virtual Move moveFromSanString(const QString& str);

		/*!
		 * This defines the symmetry of the game:
		 * The returned value is false per default: Black kings
		 * refer to white's base rank (square a1).
		 * If configured to return true then both sides refer to
		 * their own base rank (squares a1/a8).
		 */
		virtual bool isSymmetrical() const;
		/*!
		 * Returns the square index of the royal king of given \a side.
		 */
		virtual int royalKingSquare(Side side) const;
	private:
		bool m_symmetrical;
		const int m_castlingSourceFile;
};



/*!
 * \brief A board for Two Kings Each Chess, symmetrical version
 *
 * This version of Two Kings Each Chess is more balanced for the sides.
 * When a side's kings are on the same file the king closer to its own
 * base rank is royal.
 *
 * Symmetry suggested by Abdul-Rahman Sibahi in 2007 and Jon Åsvang in 2017.
 *
 * \sa TwoKingsEachBoard
 */
class LIB_EXPORT TwoKingsSymmetricalBoard : public TwoKingsEachBoard
{
	public:
		/*! Creates a new TwoKingsSymmetricalBoard object. */
		TwoKingsSymmetricalBoard();

		// Inherited from TwoKingsEachBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual bool isSymmetrical() const;
};

} // namespace Chess

#endif // TWOKINGSEACHBOARD_H

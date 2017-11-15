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

#ifndef GRYPHONBOARD_H
#define GRYPHONBOARD_H

#include "westernboard.h"
#include <QStack>

namespace Chess {

/*!
 * \brief A board for Gryphon Chess
 *
 * Gryphon Chess is also known as Complicacious Chess [Multiple Kings].
 * Moved pieces (except for Kings) change their type at the end of a
 * move. A Pawn becomes a Knight, a Knight transforms to a Bishop,
 * Bishop changes to Rook, Rook to Queen, Queen to King.
 *
 * The Kings are omitted from the initial setup. A side must never have
 * more than four Knights, four Bishops, four Rooks, and two Queens.
 * There can be fifteen Kings for a side, and giving mate to any King
 * wins.
 *
 * Introduced by Vernon R. Parton, UK, in 1961 and amended in 1974.
 *
 * \sa CircularGryphonBoard
 * \sa SimplifiedGryphonBoard
 *
 * \note Rules: https://en.wikipedia.org/wiki/V._R._Parton#Gryphon_Chess
 */
class LIB_EXPORT GryphonBoard : public WesternBoard
{
	public:
		/*! Creates a new GryphonBoard object. */
		GryphonBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		virtual void vInitialize();
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual bool isLegalPosition();
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;

		/*!
		 *  Returns new piece type after moving a piece of \a type.
		 *  For an undo move \a reversed must be set to true, the default
		 *  value is false.
		 */
		virtual int successorType(int type,
					  bool reversed = false) const;

	private:
		int m_start;
		int m_end;
		QStack<Piece> m_pieceStack;
};


/*!
 * \brief A board for Simplified Gryphon Chess
 *
 * Simplified Gryphon Chess initially has one King and eight Pawns per
 * side on their ususal positions, and all other pieces are absent.
 *
 * The rules are those of Gryphon Chess. However, a side must not move
 * their King sidewards or backwards until a capture of an opponent
 * piece has been made.
 *
 * Introduced by Vernon R. Parton, UK, in 1961 and modified in 1974.
 *
 * \sa GryphonBoard
 * \sa CircularGryphonBoard
 *
 * \note Rules: https://en.wikipedia.org/wiki/V._R._Parton#Gryphon_Chess
 */
class LIB_EXPORT SimplifiedGryphonBoard : public GryphonBoard
{
	public:
		/*! Creates a new SimplifiedGryphonBoard object. */
		SimplifiedGryphonBoard();

		// Inherited from GryphonBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);

	private:
		int m_captures[2];
};


/*!
 * \brief A board for Circular Gryphon Chess
 *
 * Circular Gryphon Chess is a single king variant of Gryphon Chess.
 * Moved pieces (except for Kings) change their type at the end of the
 * move in a circular path:
 * A Pawn becomes a Knight, a Knight transforms to a Bishop, Bishop
 * changes to Rook, Rook to Queen, Queen to Pawn.
 *
 * This variant uses a standard setup. A side must never have more
 * than four Knights, four Bishops, four Rooks, and two Queens.
 * There is one King per side, and giving mate to the King
 * wins. To standard chess this variant is closer than Gryphon Chess.
 *
 * Introduced by Vernon R. Parton, UK, in 1961 and amended in 1974.
 *
 * \sa GryphonBoard
 * \sa SimplifiedGryphonBoard
 * \sa ChangeOverBoard
 *
 * \note Rules: https://en.wikipedia.org/wiki/V._R._Parton#Gryphon_Chess
 */
class LIB_EXPORT CircularGryphonBoard : public GryphonBoard
{
	public:
		/*! Creates a new CircularGryphonBoard object. */
		CircularGryphonBoard();

		// Inherited from GryphonBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool inCheck(Side side,
				     int square = 0) const;
		virtual int successorType(int type,
					  bool reversed = false) const;
};



/*!
 * \brief A board for Change-Over Chess
 *
 * Change-Over Chess was introduced by L. Russel Chauvenet, USA, in 1943.
 *
 * Moved pieces (except for Kings) change their type at the end of the
 * move in a circular path in opposite way to Circular Gryphon Chess.
 * A Pawn becomes a Queen, a Queen transforms to a Rook, Rook
 * changes to Bishop, Bishop to Knight, Knight to Pawn.
 *
 * This variant uses a standard setup and giving mate to the King wins.
 * The number of Queens, Rooks, Bishops and Knights are not restricted
 * in contrast to Circular Gryphon Chess.
 *
 * \sa CircularGryphonBoard
 */
class LIB_EXPORT ChangeOverBoard : public CircularGryphonBoard
{
	public:
		/*! Creates a new ChangeOverBoard object. */
		ChangeOverBoard();

		// Inherited from GryphonBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		virtual int successorType(int type,
					  bool reversed = false) const;
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
		virtual bool isLegalPosition();
};

} // namespace Chess
#endif // GRYPHONBOARD_H

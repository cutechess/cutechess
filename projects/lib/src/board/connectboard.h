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

#ifndef CONNECTBOARD_H
#define CONNECTBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A base board for games with the goal of lining up a number of pieces
 * \brief A board for Cfour
 *
 * Cfour is an abstract strategy game
 *
 * Two players take turns to drop pieces onto the board of 7 files
 * and 6 ranks. Every piece goes to the lowest unoccupied rank of
 * the chosen file. The side that connects four pieces horizontally,
 * vertically or diagonally wins.
 *
 * Introduced in 1973/1974 by Howard Wexler and Ned Strongin, USA.
 * */
class LIB_EXPORT ConnectBoard : public WesternBoard
{
	public:
		/*! Creates a new ConnectBoard object. */
		ConnectBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual bool variantHasDrops() const;
		virtual QList<Chess::Piece> reservePieceTypes() const;
		virtual int width() const;
		virtual int height() const;
		virtual Result result();

	protected:
		/*!
		 * Returns true if all pieces gravitate towards the first rank.
		 * No empty squares can occur between pieces of the same file.
		 * Returns false if this condition is not met.
		 */
		virtual bool hasGravity() const;
		/*! Returns the number of pieces to connect in order to win */
		virtual int connectToWin() const;
		/*!
		 * The return value determines the total number of pieces.
		 * No determination is made if -1 is returned.
		 */
		virtual int requiredNumberOfPieces() const;
		/*!
		 * Returns true if the game is won in case the number of
		 * connected pieces of side \a side exceeds the required
		 * number, else false. Default: true.
		 */
		virtual bool overlinesWin(Side side) const;

		// Inherited from WesternBoard
		virtual void vInitialize();
		virtual bool vSetFenString(const QStringList & fen);
		bool kingsCountAssertion(int, int) const;
		virtual void generateMovesForPiece(QVarLengthArray<Chess::Move> & moves,
						   int pieceType,
						   int square) const;
		virtual void vMakeMove(const Chess::Move & move,
				       Chess::BoardTransition * transition);
		virtual void vUndoMove(const Chess::Move & move);
		virtual bool isLegalPosition();

	private:
		bool pieceCountOk() const;
		bool connected(int n, Side side) const;

		int m_hasGravity;
		int m_connectToWin;
		QVector<int> m_pieceCounter;
};

/*!
 * \brief A board for TicTacToe
 *
 * TicTacToe is a minimalist abstract strategy game.
 *
 * Two players take turns to drop pieces onto the 3x3 board.
 * The side that connects three pieces horizontally, vertically
 * or diagonally wins.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Tic_Tac_Toe
 * \sa ConnectBoard
 * */
class LIB_EXPORT TicTacToeBoard : public ConnectBoard
{
	public:
		/*! Creates a new TicTacToeBoard object. */
		TicTacToeBoard();

		// Inherited from ConnectBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;
		virtual int height() const;
		virtual int connectToWin() const;

	protected:
		virtual bool hasGravity() const;
};

/*!
 * \brief A board for Gomoku Free-Style
 *
 * Gomoku is an abstract strategy game from 19th century Japan.
 * It is also known as Omok, Five in a Row, and Go Bang.
 *
 * Two players take turns to drop pieces onto the 15x15 board.
 * The side that connects five pieces horizontally, vertically
 * or diagonally wins. The Gomoku tournament variant requires
 * exactly five pieces lined up to win, overlines of six or more
 * pieces are ignored.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Gomoku
 * \sa GomokuBoard
 * \sa ConnectBoard
 * */
//TODO: Black begins, One ply per move, starting phase
class LIB_EXPORT GomokuFreestyleBoard : public ConnectBoard
{
	public:
		/*! Creates a new GomokuFreestyleBoard object. */
		GomokuFreestyleBoard();

		// Inherited from ConnectBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;
		virtual int height() const;
		virtual int connectToWin() const;

	protected:
		virtual bool hasGravity() const;
};

/*!
 * \brief A board for Gomoku
 *
 * Gomoku is an abstract strategy game from 19th century Japan.
 * It is also known as Omok, Five in a Row, and Go Bang.
 *
 * Two players take turns to drop pieces onto the 15x15 board.
 * The side that connects five pieces horizontally, vertically
 * or diagonally wins. While the Gomoku tournament variant
 * requires exactly five pieces lined up to win, the free-style
 * variant also accepts overlines of six or more pieces.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Gomoku
 * \sa GomokuFreestyleBoard
 * \sa ConnectBoard
 * */
class LIB_EXPORT GomokuBoard : public GomokuFreestyleBoard
{
	public:
		/*! Creates a new GomokuBoard object. */
		GomokuBoard();

		// Inherited from GomokuBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		virtual bool overlinesWin(Side side) const;
};
}// namespace Chess
#endif // CONNECTBOARD_H

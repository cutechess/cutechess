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

#ifndef MOVEGEN_H
#define MOVEGEN_H

#define MAX_NMOVES 128			// max. num. of moves per position


/**
 * The MoveList class represents an efficient container for storing chess moves
 * in integer format. This class should only be used by Chessboard.
 */
class MoveList
{
	public:
		/**
		 * Creates a new MoveList object.
		 */
		MoveList();

		/**
		 * Gets the move at a specific index.
		 * @param index The index of the move in the list.
		 */
		quint32 operator[](int index);

		/**
		 * Adds a new move to the list.
		 * @param move The move that will be added to the list.
		 */
		void add(quint32 move);

		/**
		 * The number of moves in the list.
		 */
		int count();

		/**
		 * Tries to find a move in the list.
		 * If a match is found, additional details may be added to the move.
		 * @param move The move to find.
		 * @return True if the move is found.
		 */
		bool moveExists(quint32& move);

		/** Clears the move list.  */
		void clear();
	private:
		int m_nmoves;
		quint32 m_moves[MAX_NMOVES];
};

/**
 * Data that is needed to generate moves.
 * Only some of this data will be included in the generated moves.
 */
typedef struct _MoveData
{
	/** The source square. */
	int from;
	/** The target square. */
	int to;
	/** The en passant square. */
	int enpassantSquare;
	/** The promotion piece type. */
	int promotion;
	/** Castling (none, kingside or queenside). */
	int castle;
	/** Check mask for bishops and queens. */
	quint64 bishopCheckMask;
	/** Check mask for rooks and queens. */
	quint64 rookCheckMask;
	/** Mask of pinned pieces. */
	quint64 pins;
	/** Mask of pieces which can give a discovered check. */
	quint64 discoveredCheckers;
	/** Mask of target squares. */
	quint64 target;
} MoveData;

#endif // MOVEGEN_H


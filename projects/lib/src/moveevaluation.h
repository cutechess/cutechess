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

#ifndef MOVEEVALUATION_H
#define MOVEEVALUATION_H

#include <QString>
#include <QMetaType>

/*!
 * \brief Evaluation data for a chess move.
 *
 * Before chess engines send their move they usually print information
 * about the moves they're thinking of, how many nodes they've searched,
 * the search depth, etc. This class stores that information so that it
 * could be saved in a PGN file or displayed on the screen.
 *
 * From human players we can only get the move time.
 */
class LIB_EXPORT MoveEvaluation
{
	public:
		/*! Mate score reference */
		constexpr static int MATE_SCORE = 1000000;

		/*! A value for a null or empty score. */
		constexpr static int NULL_SCORE = 0xFFFFFFF;

		/*! Constructs an empty MoveEvaluation object. */
		MoveEvaluation();

		/*! Returns true if \a other is the same as this eval. */
		bool operator==(const MoveEvaluation& other) const;
		/*! Returns true if \a other is different from this eval. */
		bool operator!=(const MoveEvaluation& other) const;

		/*! Returns true if the evaluation is empty. */
		bool isEmpty() const;
		
		/*! Returns true if the evaluation points to a book move. */
		bool isBookEval() const;

		/*! Returns true if the eval is trusted for adjudication. */
		bool isTrusted() const;

		/*!
		 * How many plies were searched?
		 * \note For human players this is always 0.
		 */
		int depth() const;

		/*!
		 * How many plies were searched in selective search lines?
		 * \note For human players this is always 0.
		 */
		int selectiveDepth() const;

		/*!
		 * Score in centipawns from the player's point of view.
		 * \note For human players this always 0.
		 */
		int score() const;

		/*!
		 * String representation of the score from the player's
		 * point of view. Mate distances are prefixed with -M or +M.
		 *
		 * \note For human players an empty string is returned.
		 */
		QString scoreText() const;

		/*! Move time in milliseconds. */
		int time() const;

		/*!
		 * How many nodes were searched?
		 * \note For human players this is always 0.
		 */
		quint64 nodeCount() const;

		/*!
		 * How many nodes per second is the engine searching?
		 * \note For human players this is always 0.
		 */
		quint64 nps() const;

		/*!
		 * How many positions were found in endgame tablebases?
		 * \note For human players this is always 0.
		 */
		quint64 tbHits() const;

		/*!
		 * How many permille of the engine's hash table is used?
		 * \note For human players this is always 0.
		 */
		int hashUsage() const;

		/*!
		 * How many permille of ponder move choices were ponderhits?
		 * \note For human players this is always 0.
		 */
		int ponderhitRate() const;

		/*! The current ponder move in SAN notation. */
		QString ponderMove() const;

		/*!
		 * The principal variation.
		 * This is a sequence of moves that an engine
		 * expects to be played next.
		 * \note For human players this is always empty.
		 */
		QString pv() const;

		/*!
		 * Returns the principal variation number (default 0).
		 * \note For human players this is always 0.
		 */
		int pvNumber() const;


		/*! Resets everything to zero. */
		void clear();

		/*! Sets book evaluation. */
		void setBookEval(bool isBookEval);

		/*! Sets trusted property. */
		void setIsTrusted(bool isTrusted);

		/*! Sets the search depth to \a depth. */
		void setDepth(int depth);

		/*! Sets the selective search depth to \a depth. */
		void setSelectiveDepth(int depth);

		/*! Sets the score to \a score. */
		void setScore(int score);

		/*! Sets the move time to \a time. */
		void setTime(int time);

		/*! Sets the node count to \a nodeCount. */
		void setNodeCount(quint64 nodeCount);

		/*! Sets the nodes per second count to \a nps. */
		void setNps(quint64 nps);

		/*! Sets the tablebase hits count to \a tbHits. */
		void setTbHits(quint64 tbHits);

		/*! Sets the hash table usage to \a hashUsage permille. */
		void setHashUsage(int hashUsage);

		/*! Sets ponderhit rate to \a rate permille. */
		void setPonderhitRate(int rate);

		/*! Sets the current ponder move to \a san. */
		void setPonderMove(const QString& san);

		/*! Sets the principal variation to \a pv. */
		void setPv(const QString& pv);

		/*! Sets the principal variation number to \a number. */
		void setPvNumber(int number);

		/*! Merges non-empty parameters of \a other into this eval. */
		void merge(const MoveEvaluation& other);

	private:
		bool m_isBookEval;
		bool m_isTrusted;
		int m_depth;
		int m_selDepth;
		int m_score;
		int m_time;
		int m_pvNumber;
		int m_hashUsage;
		int m_ponderhitRate;
		quint64 m_nodeCount;
		quint64 m_nps;
		quint64 m_tbHits;
		QString m_pv;
		QString m_ponderMove;
};

Q_DECLARE_METATYPE(MoveEvaluation)

#endif // MOVEEVALUATION_H

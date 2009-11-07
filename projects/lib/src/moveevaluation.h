#ifndef MOVEEVALUATION_H
#define MOVEEVALUATION_H

#include <QString>

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
		/*! Constructs an empty MoveEvaluation object. */
		MoveEvaluation();

		/*! Returns true if the evaluation is empty. */
		bool isEmpty() const;
		
		/*! Returns true if the evaluation points to a book move. */
		bool isBookEval() const;

		/*!
		 * How many plies were searched?
		 * \note For human players this is always 0.
		 */
		int depth() const;

		/*!
		 * Score in centipawns from the player's point of view.
		 * \note For human player this always 0.
		 */
		int score() const;

		/*! Move time in milliseconds. */
		int time() const;

		/*!
		 * How many nodes were searched?
		 * \note For human players this is always 0.
		 */
		int nodeCount() const;

		/*!
		 * The principal variation.
		 * This is a sequence of moves that an engine
		 * expects to be played next.
		 * \note For human players this is always empty.
		 */
		QString pv() const;


		/*! Resets everything to zero. */
		void clear();

		/*! Sets book evaluation. */
		void setBookEval(bool isBookEval);

		/*! Sets the search depth to \a depth. */
		void setDepth(int depth);

		/*! Sets the score to \a score. */
		void setScore(int score);

		/*! Sets the move time to \a time. */
		void setTime(int time);

		/*! Sets the node count to \a nodeCount. */
		void setNodeCount(int nodeCount);

		/*! Sets the principal variation to \a pv. */
		void setPv(const QString& pv);

	private:
		bool m_isBookEval;
		int m_depth;
		int m_score;
		int m_time;
		int m_nodeCount;
		QString m_pv;
};

#endif // MOVEEVALUATION_H

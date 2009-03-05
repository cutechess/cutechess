#ifndef RESULT_H
#define RESULT_H

#include "chess.h"

namespace Chess {

/*!
 * \brief The result of a chess game
 *
 * The Result class is used to store the result of a chess game,
 * to compare it to other results, and to convert between the
 * result codes and result strings.
 */
class LIB_EXPORT Result
{
	public:
		/*! Result code. */
		enum Code
		{
			//! Win by any means.
			Win,
			//! Win by a mate.
			WinByMate,
			//! The loser resigns.
			WinByResignation,
			//! Timeout (loser's flag fell).
			WinByTimeout,
			//! Adjudication by the GUI due to a hopeless position for the loser.
			WinByAdjudication,
			//! Loser tries to make an illegal move.
			WinByIllegalMove,
			//! Loser disconnects, or terminates (if it's an engine).
			WinByDisconnection,
			//! Loser's connection stalls (doesn't respond to ping).
			WinByStalledConnection,
			//! Draw by any means.
			Draw,
			//! Draw by a stalemate.
			DrawByStalemate,
			//! Draw by insufficient mating material on both sides.
			DrawByMaterial,
			//! Draw by 3 repetitions of the same position.
			DrawByRepetition,
			//! Draw by 50 consecutive reversible moves.
			DrawByFiftyMoves,
			//! Both players agree to a draw.
			DrawByAgreement,
			//! One player timeouts, and the other one can't win.
			DrawByTimeout,
			//! Adjudication by the GUI due to a drawn position.
			DrawByAdjudication,
			//! No result. The game may continue.
			NoResult,
			//! Result error, caused by an invalid result string.
			ResultError
		};
		
		/*!
		 * Creates a new result.
		 *
		 * \param code The result code
		 * \param winner The winning side (or NoSide in case of a draw)
		 * \param move The move that caused the result
		 */
		explicit Result(Code code = NoResult,
				Side winner = NoSide,
				const QString& move = "");
		/*! Creates a new result from a string. */
		explicit Result(const QString& str);
		
		/*! Returns true if \a other is the same as this result. */
		bool operator==(const Result& other) const;
		/*! Returns true if \a other different from this result. */
		bool operator!=(const Result& other) const;
		
		/*! Returns the winning side, or NoSide if there's no winner. */
		Side winner() const;
		/*! Returns the losing side, or NoSide if there's no loser. */
		Side loser() const;
		
		/*! Returns true if the result is some type of draw. */
		bool isDraw() const;
		/*! Returns true if the result is NoResult. */
		bool isNone() const;
		
		/*! Returns the result code. */
		Code code() const;
		/*!
		 * Returns the simple result code.
		 * Can be Win, Draw, or NoResult.
		 */
		Code simpleCode() const;
		
		/*! Returns the result string. */
		QString toString() const;
		/*!
		 * Returns the simple result string.
		 * Can be "1-0", "0-1", "1/2-1/2", or "*".
		 */
		QString toSimpleString() const;
	
	private:
		Code m_code;
		Side m_winner;
		QString m_move;
};


} // namespace Chess
#endif // RESULT_H

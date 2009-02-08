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
			//! White wins by any means
			WhiteWins,
			//! Black wins by any means
			BlackWins,
			//! White wins by mating the black player.
			WhiteMates,
			//! Black wins by mating the white player.
			BlackMates,
			//! Black wins when white resigns
			WhiteResigns,
			//! White wins when black resigns
			BlackResigns,
			//! Black wins due to white's timeout.
			WhiteTimeout,
			//! White wins due to black's timeout.
			BlackTimeout,
			//! Draw by a stalemate.
			Stalemate,
			//! Draw by any means
			Draw,
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
			//! No result. The game may continue.
			NoResult,
			//! Result error, caused by an invalid result string.
			ResultError
		};
		
		/*! Creates a new result from a result code. */
		Result(Code code = NoResult);
		/*! Creates a new result from a string. */
		Result(const QString& str);
		
		/*! Returns true if \a other is the same as this result. */
		bool operator==(const Result& other) const;
		/*! Returns true if \a otherCode is the same as this result's code. */
		bool operator==(Code otherCode) const;
		/*! Returns true if \a other different from this result. */
		bool operator!=(const Result& other) const;
		/*! Returns true if \a otherCode is different from this result's code. */
		bool operator!=(Code otherCode) const;
		
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
		 * Can be WhiteWins, BlackWins, Draw, or NoResult.
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
};


} // namespace Chess
#endif // RESULT_H

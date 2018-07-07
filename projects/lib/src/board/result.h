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

#ifndef RESULT_H
#define RESULT_H

#include "side.h"
#include <QMetaType>
#include <QCoreApplication>

namespace Chess {

/*!
 * \brief The result of a chess game
 *
 * The Result class is used to store the result of a chess game,
 * and to compare it to other results.
 */
class LIB_EXPORT Result
{
	Q_DECLARE_TR_FUNCTIONS(Result)

	public:
		/*! Result type. */
		enum Type
		{
			//! Win by any means.
			Win,
			//! Draw by any means.
			Draw,
			//! Loser resigns.
			Resignation,
			//! A player's time flag falls.
			Timeout,
			//! Adjudication by the GUI.
			Adjudication,
			//! Loser tries to make an illegal move.
			IllegalMove,
			//! Loser disconnects, or terminates (if it's an engine).
			Disconnection,
			//! Loser's connection stalls (doesn't respond to ping).
			StalledConnection,
			//! Both players agree to a result.
			Agreement,
			//! No result. The game may continue.
			NoResult,
			//! Result error, caused by an invalid result string.
			ResultError
		};

		/*!
		 * Creates a new result.
		 *
		 * \param type The type of the result
		 * \param winner The winning side (or NoSide in case of a draw)
		 * \param description A description of the result. If the Result
		 *        class has a preset description for \a type, this
		 *        additional description is appended to it.
		 */
		explicit Result(Type type = NoResult,
				Side winner = Side(),
				const QString& description = QString());
		/*! Creates a new result from a string. */
		explicit Result(const QString& str);

		/*! Returns true if \a other is the same as this result. */
		bool operator==(const Result& other) const;
		/*! Returns true if \a other different from this result. */
		bool operator!=(const Result& other) const;
		/*! Returns true if the result is NoResult. */
		bool isNone() const;
		/*! Returns true if the result is Draw. */
		bool isDraw() const;

		/*! Returns the winning side, or NoSide if there's no winner. */
		Side winner() const;
		/*! Returns the losing side, or NoSide if there's no loser. */
		Side loser() const;

		/*! Returns the type of the result. */
		Type type() const;
		/*! Returns the result description. */
		QString description() const;
		/*!
		 * Returns the short string representation of the result.
		 * Can be "1-0", "0-1", "1/2-1/2", or "*".
		 */
		QString toShortString() const;
		/*!
		 * Returns the verbose string representation of the result.
		 *
		 * Uses the format "result {description}".
		 * Eg. "1-0 {White mates}".
		 */
		QString toVerboseString() const;

	private:
		Type m_type;
		Side m_winner;
		QString m_description;
};

} // namespace Chess

Q_DECLARE_METATYPE(Chess::Result)

#endif // RESULT_H

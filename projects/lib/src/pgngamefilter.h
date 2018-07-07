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

#ifndef PGNGAMEFILTER_H
#define PGNGAMEFILTER_H

#include <QByteArray>
#include <QDate>
#include "board/side.h"
class QString;
class PgnGameEntry;

/*!
 * \brief A filter for chess games in a PGN database.
 *
 * A PgnGameFilter object can be used to filter games in a PGN
 * database by matching the filtering terms to the games' PGN
 * tags.
 *
 * \sa PgnGameEntry
 */
class LIB_EXPORT PgnGameFilter
{
	public:
		/*! The type of the filter. */
		enum Type
		{
			/*! Filter all tags with a single fixed string. */
			FixedString,
			/*! Use invididual filtering terms for each tag. */
			Advanced
		};

		/*! The result of the game. */
		enum Result
		{
			AnyResult,		//!< Any result (no filtering)
			EitherPlayerWins,	//!< Either player wins
			WhiteWins,		//!< The white player wins
			BlackWins,		//!< The black player wins
			FirstPlayerWins,	//!< The first player wins
			FirstPlayerLoses,	//!< The first player loses
			Draw,			//!< The game is a draw
			Unfinished		//!< The game wasn't completed
		};

		/*!
		 * Creates a new empty filter.
		 *
		 * An empty filter will not filter out any games.
		 * The filter's type is \a Advanced.
		 */
		PgnGameFilter();
		/*!
		 * Creates a new filter based on a fixed string.
		 *
		 * Games that have tags matching \a pattern will be filtered in.
		 * The filter's type is \a FixedString.
		 */
		PgnGameFilter(const QString& pattern);

		/*! Returns the type of the filter. */
		Type type() const;

		/*! Returns the pattern for \a FixedString mode. */
		const char* pattern() const;

		/*! Returns the filter for the \a Event tag. */
		const char* event() const;
		/*! Returns the filter for the \a Site tag. */
		const char* site() const;
		/*!
		 * Returns the filter for the first player.
		 *
		 * The first player's side/color is determined by the
		 * playerSide() member function.
		 */
		const char* player() const;
		/*! Returns the filter for the opponent of the first player. */
		const char* opponent() const;
		/*! Returns the filter for the side/color of the first player. */
		Chess::Side playerSide() const;
		/*!
		 * Returns the filter for the game's minimum starting date.
		 *
		 * \note A null QDate won't filter out any games.
		 */
		const QDate& minDate() const;
		/*!
		 * Returns the filter for the game's maximum starting date.
		 *
		 * \note A null QDate won't filter out any games.
		 */
		const QDate& maxDate() const;
		/*! Returns the filter for the minimum round ordinal. */
		int minRound() const;
		/*! Returns the filter for the maximum round ordinal. */
		int maxRound() const;
		/*! Returns the filter for the \a Result tag. */
		Result result() const;
		/*!
		 * Returns true if the filter is looking for the inverse
		 * of \a result(); otherwise returns false.
		 */
		bool isResultInverted() const;

		/*!
		 * Sets the \a FixedString pattern to \a pattern.
		 *
		 * This function will change the filter type to \a FixedString.
		 */
		void setPattern(const QString& pattern);

		/*! Sets the \a Event tag filter to \a event. */
		void setEvent(const QString& event);
		/*! Sets the \a Site tag filter to \a site. */
		void setSite(const QString& site);
		/*! Sets the minimum starting date filter to \a date. */
		void setMinDate(const QDate& date);
		/*! Sets the maximum starting date filter to \a date. */
		void setMaxDate(const QDate& date);
		/*! Sets the minimum round ordinal filter to \a round. */
		void setMinRound(int round);
		/*! Sets the maximum round ordinal filter to \a round. */
		void setMaxRound(int round);
		/*! Sets the \a side player's filter to \a name. */
		void setPlayer(const QString& name, Chess::Side side);
		/*! Sets the first player's opponent filter to \a name. */
		void setOpponent(const QString& name);
		/*! Sets the \a Result tag filter to \a result. */
		void setResult(Result result);
		/*! Sets the \a resultInverted value to \a invert. */
		void setResultInverted(bool invert);

	private:
		Type m_type;
		QByteArray m_pattern;
		QByteArray m_event;
		QByteArray m_site;
		QByteArray m_player;
		QByteArray m_opponent;
		Chess::Side m_playerSide;
		QDate m_minDate;
		QDate m_maxDate;
		int m_minRound;
		int m_maxRound;
		Result m_result;
		bool m_resultInverted;
};

inline PgnGameFilter::Type PgnGameFilter::type() const
{
	return m_type;
}

inline const char* PgnGameFilter::pattern() const
{
	return m_pattern.constData();
}

inline const char* PgnGameFilter::event() const
{
	return m_event.constData();
}

inline const char* PgnGameFilter::site() const
{
	return m_site.constData();
}

inline const QDate& PgnGameFilter::minDate() const
{
	return m_minDate;
}

inline const QDate& PgnGameFilter::maxDate() const
{
	return m_maxDate;
}

inline int PgnGameFilter::minRound() const
{
	return m_minRound;
}

inline int PgnGameFilter::maxRound() const
{
	return m_maxRound;
}

inline PgnGameFilter::Result PgnGameFilter::result() const
{
	return m_result;
}

inline bool PgnGameFilter::isResultInverted() const
{
	return m_resultInverted;
}

inline const char* PgnGameFilter::player() const
{
	return m_player.constData();
}

inline const char* PgnGameFilter::opponent() const
{
	return m_opponent.constData();
}

inline Chess::Side PgnGameFilter::playerSide() const
{
	return m_playerSide;
}

#endif // PGNGAMEFILTER_H

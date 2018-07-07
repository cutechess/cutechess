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


#ifndef TOURNAMENTFACTORY_H
#define TOURNAMENTFACTORY_H

class QString;
class QObject;
class Tournament;
class GameManager;

/*!
 * \brief A factory for creating Tournament objects. */
class LIB_EXPORT TournamentFactory
{
	public:
		/*!
		 * Creates and returns a new tournament of type \a type
		 * that uses \a manager to manage its games.
		 *
		 * Returns 0 if \a type is not supported.
		 */
		static Tournament* create(const QString& type,
					  GameManager* manager,
					  QObject* parent = nullptr);

	private:
		TournamentFactory();
};

#endif // TOURNAMENTFACTORY_H

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


#ifndef PYRAMIDTOURNAMENT_H
#define PYRAMIDTOURNAMENT_H

#include "tournament.h"

/*!
 * \brief Pyramid type chess tournament.
 *
 * In a Pyramid tournament each player meets all
 * of their predecessors in sequence.
 */
class LIB_EXPORT PyramidTournament : public Tournament
{
	Q_OBJECT

	public:
		/*! Creates a new Pyramid tournament. */
		explicit PyramidTournament(GameManager* gameManager,
					      QObject *parent = nullptr);
		// Inherited from Tournament
		virtual QString type() const;

	protected:
		// Inherited from Tournament
		virtual void initializePairing();
		virtual int gamesPerCycle() const;
		virtual TournamentPair* nextPair(int gameNumber);

	private:
		int m_pairNumber;
		int m_currentPlayer;
};

#endif // PYRAMIDTOURNAMENT_H

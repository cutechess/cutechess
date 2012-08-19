#ifndef GAUNTLETTOURNAMENT_H
#define GAUNTLETTOURNAMENT_H

#include "tournament.h"

/*!
 * \brief Gauntlet type chess tournament.
 *
 * In a Gauntlet tournament the first participant plays
 * against all the others.
 */
class LIB_EXPORT GauntletTournament : public Tournament
{
	Q_OBJECT

	public:
		/*! Creates a new Gauntlet tournament. */
		explicit GauntletTournament(GameManager* gameManager,
					    QObject *parent = 0);
		// Inherited from Tournament
		virtual QString type() const;

	protected:
		// Inherited from Tournament
		virtual void initializePairing();
		virtual int gamesPerCycle() const;
		virtual QPair<int, int> nextPair();

	private:
		int m_opponent;
};

#endif // GAUNTLETTOURNAMENT_H

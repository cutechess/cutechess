#include "tournamentfactory.h"
#include "roundrobintournament.h"

Tournament* TournamentFactory::create(const QString& type,
				      GameManager* manager,
				      QObject* parent)
{
	if (type == "round-robin")
		return new RoundRobinTournament(manager, parent);
	return 0;
}

#include "tournamentfactory.h"
#include "roundrobintournament.h"
#include "gauntlettournament.h"

Tournament* TournamentFactory::create(const QString& type,
				      GameManager* manager,
				      QObject* parent)
{
	if (type == "round-robin")
		return new RoundRobinTournament(manager, parent);
	if (type == "gauntlet")
		return new GauntletTournament(manager, parent);

	return 0;
}

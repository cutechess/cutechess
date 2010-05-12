#include "boardfactory.h"
#include "atomicboard.h"
#include "capablancaboard.h"
#include "caparandomboard.h"
#include "crazyhouseboard.h"
#include "frcboard.h"
#include "gothicboard.h"
#include "losersboard.h"
#include "standardboard.h"

namespace Chess {

REGISTER_BOARD(AtomicBoard, "atomic")
REGISTER_BOARD(CapablancaBoard, "capablanca")
REGISTER_BOARD(CaparandomBoard, "caparandom")
REGISTER_BOARD(CrazyhouseBoard, "crazyhouse")
REGISTER_BOARD(FrcBoard, "fischerandom")
REGISTER_BOARD(GothicBoard, "gothic")
REGISTER_BOARD(LosersBoard, "losers")
REGISTER_BOARD(StandardBoard, "standard")


ClassRegistry<Board>* BoardFactory::registry()
{
	static ClassRegistry<Board>* registry = new ClassRegistry<Board>;
	return registry;
}

Board* BoardFactory::create(const QString& variant, QObject* parent)
{
	return registry()->create(variant, parent);
}

QStringList BoardFactory::variants()
{
	return registry()->items().keys();
}

} // namespace Chess

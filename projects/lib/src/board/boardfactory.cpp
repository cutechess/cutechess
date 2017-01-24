/*
    This file is part of Cute Chess.

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

#include "boardfactory.h"
#include "atomicboard.h"
#include "capablancaboard.h"
#include "caparandomboard.h"
#include "crazyhouseboard.h"
#include "extinctionboard.h"
#include "frcboard.h"
#include "gothicboard.h"
#include "hordeboard.h"
#include "losersboard.h"
#include "standardboard.h"
#include "berolinaboard.h"
#include "chessgiboard.h"
#include "kingofthehillboard.h"
#include "loopboard.h"
#include "ncheckboard.h"
#include "racingkingsboard.h"

namespace Chess {

REGISTER_BOARD(ThreeCheckBoard, "3check")
REGISTER_BOARD(FiveCheckBoard, "5check")
REGISTER_BOARD(AtomicBoard, "atomic")
REGISTER_BOARD(BerolinaBoard, "berolina")
REGISTER_BOARD(CapablancaBoard, "capablanca")
REGISTER_BOARD(CaparandomBoard, "caparandom")
REGISTER_BOARD(ChessgiBoard, "chessgi")
REGISTER_BOARD(CrazyhouseBoard, "crazyhouse")
REGISTER_BOARD(ExtinctionBoard, "extinction")
REGISTER_BOARD(KingletBoard, "kinglet")
REGISTER_BOARD(FrcBoard, "fischerandom")
REGISTER_BOARD(GothicBoard, "gothic")
REGISTER_BOARD(HordeBoard, "horde")
REGISTER_BOARD(KingOfTheHillBoard, "kingofthehill")
REGISTER_BOARD(LoopBoard, "loop")
REGISTER_BOARD(LosersBoard, "losers")
REGISTER_BOARD(RacingKingsBoard, "racingkings")
REGISTER_BOARD(StandardBoard, "standard")


ClassRegistry<Board>* BoardFactory::registry()
{
	static ClassRegistry<Board>* registry = new ClassRegistry<Board>;
	return registry;
}

Board* BoardFactory::create(const QString& variant)
{
	return registry()->create(variant);
}

QStringList BoardFactory::variants()
{
	return registry()->items().keys();
}

} // namespace Chess

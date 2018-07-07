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

#include "giveawayboard.h"

namespace Chess {

GiveawayBoard::GiveawayBoard()
	: AntiBoard()
{
}

Board* GiveawayBoard::copy() const
{
	return new GiveawayBoard(*this);
}

QString GiveawayBoard::variant() const
{
	return "giveaway";
}

QString GiveawayBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool GiveawayBoard::hasCastling() const
{
	return true;
}

void GiveawayBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	if (captureType(move) == King)
		removeCastlingRights(sideToMove().opposite());

	return WesternBoard::vMakeMove(move, transition);
}

} // namespace Chess

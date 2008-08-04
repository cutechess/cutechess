/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QString>

#include "chessplayer.h"

ChessPlayer::ChessPlayer(const TimeControl& timeControl, QObject* parent)
	: QObject(parent)
{
	m_side = Chessboard::NoSide;
	m_timeControl = timeControl;

	m_opponent = 0;
}

void ChessPlayer::newGame(Chessboard::ChessSide side)
{
	Q_CHECK_PTR(m_opponent);
	
	setSide(side);
	m_timeControl.setTimeLeft(m_timeControl.timePerTc());
	m_timeControl.setMovesLeft(m_timeControl.movesPerTc());
}

TimeControl ChessPlayer::timeControl() const
{
	return m_timeControl;
}

void ChessPlayer::setTimeControl(const TimeControl& timeControl)
{
	m_timeControl = timeControl;
}

void ChessPlayer::setSide(Chessboard::ChessSide side)
{
	m_side = side;
}

void ChessPlayer::setOpponent(ChessPlayer* opponent)
{
	Q_CHECK_PTR(opponent);
	m_opponent = opponent;
}

Chessboard::ChessSide ChessPlayer::side() const
{
	return m_side;
}

QString ChessPlayer::name() const
{
	return m_name;
}

void ChessPlayer::setName(const QString& name)
{
	m_name = name;
}


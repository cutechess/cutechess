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

#include <QString>

#include "chessplayer.h"

ChessPlayer::ChessPlayer(const TimeControl& timeControl, QObject* parent)
	: QObject(parent),
	  m_timeControl(timeControl),
	  m_opponent(0),
	  m_side(Chess::NoSide)
{

}

void ChessPlayer::newGame(Chess::Side side, ChessPlayer* opponent)
{
	Q_ASSERT(opponent != 0);

	m_opponent = opponent;
	setSide(side);
	m_timeControl.setTimeLeft(m_timeControl.timePerTc());
	m_timeControl.setMovesLeft(m_timeControl.movesPerTc());
}

void ChessPlayer::go()
{
	if (m_timeControl.timePerTc() != 0)
		emit startedThinking(m_timeControl.timeLeft());
	else if (m_timeControl.timePerMove() != 0)
		emit startedThinking(m_timeControl.timePerMove());
}

const TimeControl& ChessPlayer::timeControl() const
{
	return m_timeControl;
}

void ChessPlayer::setTimeControl(const TimeControl& timeControl)
{
	m_timeControl = timeControl;
}

void ChessPlayer::setSide(Chess::Side side)
{
	m_side = side;
}

Chess::Side ChessPlayer::side() const
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


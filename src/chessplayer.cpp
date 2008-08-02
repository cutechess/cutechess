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
#include "timecontrol.h"

ChessPlayer::ChessPlayer(TimeControl* whiteTimeControl,
                         TimeControl* blackTimeControl,
                         QObject *parent)
: QObject(parent)
{
	m_side = Chessboard::NoSide;

	m_ownTimeControl = 0;
	m_opponentsTimeControl = 0;
	setTimeControls(whiteTimeControl, blackTimeControl);
}

void ChessPlayer::newGame(Chessboard::ChessSide side)
{
	setSide(side);
	m_ownTimeControl->setTimeLeft(m_ownTimeControl->timePerTc());
	m_ownTimeControl->setMovesLeft(m_ownTimeControl->movesPerTc());
}

TimeControl* ChessPlayer::timeControl() const
{
	return m_ownTimeControl;
}

void ChessPlayer::setTimeControls(TimeControl* whiteTimeControl,
                                  TimeControl* blackTimeControl)
{
	Q_CHECK_PTR(whiteTimeControl);
	Q_CHECK_PTR(blackTimeControl);

	m_whiteTimeControl = whiteTimeControl;
	m_blackTimeControl = blackTimeControl;

	if (m_side == Chessboard::White) {
		m_ownTimeControl = m_whiteTimeControl;
		m_opponentsTimeControl = m_blackTimeControl;
	} else if (m_side == Chessboard::Black) {
		m_ownTimeControl = m_blackTimeControl;
		m_opponentsTimeControl = m_whiteTimeControl;
	}
}

void ChessPlayer::setSide(Chessboard::ChessSide side)
{
	m_side = side;
	if (m_side == Chessboard::White) {
		m_ownTimeControl = m_whiteTimeControl;
		m_opponentsTimeControl = m_blackTimeControl;
	} else if (m_side == Chessboard::Black) {
		m_ownTimeControl = m_blackTimeControl;
		m_opponentsTimeControl = m_whiteTimeControl;
	}
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


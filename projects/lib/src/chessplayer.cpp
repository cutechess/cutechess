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

#include "chessplayer.h"
#include <QString>
#include <QTimer>


ChessPlayer::ChessPlayer(QObject* parent)
	: QObject(parent),
	  m_isReady(false),
	  m_chessboard(0),
	  m_opponent(0),
	  m_gameInProgress(false),
	  m_connected(true),
	  m_forfeited(false),
	  m_side(Chess::NoSide)
{
	m_timer.setSingleShot(true);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

bool ChessPlayer::isConnected() const
{
	return m_connected;
}

bool ChessPlayer::isReady() const
{
	return m_isReady;
}

void ChessPlayer::newGame(Chess::Side side, ChessPlayer* opponent)
{
	Q_ASSERT(opponent != 0);
	Q_ASSERT(m_isReady);
	Q_ASSERT(m_chessboard != 0);

	m_forfeited = false;
	m_eval.clear();
	m_gameInProgress = true;
	m_opponent = opponent;
	setSide(side);
	m_timeControl.setTimeLeft(m_timeControl.timePerTc());
	m_timeControl.setMovesLeft(m_timeControl.movesPerTc());
}

void ChessPlayer::endGame(Chess::Result result)
{
	Q_UNUSED(result);

	m_gameInProgress = false;
	m_chessboard = 0;
	m_timer.stop();
}

const MoveEvaluation& ChessPlayer::evaluation() const
{
	return m_eval;
}

void ChessPlayer::startClock()
{
	if (!m_gameInProgress)
		return;

	m_eval.clear();

	if (m_timeControl.timePerTc() != 0)
		emit startedThinking(m_timeControl.timeLeft());
	else if (m_timeControl.timePerMove() != 0)
		emit startedThinking(m_timeControl.timePerMove());
	
	m_timeControl.startTimer();
	m_timer.start(qMax(m_timeControl.timeLeft(), 0) + 200);
}

void ChessPlayer::makeBookMove(const Chess::Move& move)
{
	m_timeControl.startTimer();
	makeMove(move);
	m_timeControl.update();
}

TimeControl* ChessPlayer::timeControl()
{
	return &m_timeControl;
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

Chess::Side ChessPlayer::otherSide() const
{
	if (m_side == Chess::White)
		return Chess::Black;
	if (m_side == Chess::Black)
		return Chess::White;
	return Chess::NoSide;
}

QString ChessPlayer::name() const
{
	return m_name;
}

void ChessPlayer::setName(const QString& name)
{
	m_name = name;
}

void ChessPlayer::setBoard(Chess::Board* board)
{
	m_chessboard = board;
}

void ChessPlayer::emitForfeit(Chess::Result::Code code, const QString& arg)
{
	if (m_forfeited)
		return;
	m_forfeited = true;
	emit forfeit(Chess::Result(code, otherSide(), arg));
}

void ChessPlayer::emitMove(const Chess::Move& move)
{
	m_timeControl.update();
	m_eval.setTime(m_timeControl.lastMoveTime());

	m_timer.stop();
	if (m_timeControl.timeLeft() <= 0)
	{
		emitForfeit(Chess::Result::WinByTimeout);
		return;
	}
	
	emit moveMade(move);
}

void ChessPlayer::closeConnection()
{
	m_connected = false;
}

void ChessPlayer::onDisconnect()
{
	m_connected = false;
	emitForfeit(Chess::Result::WinByDisconnection);
}

void ChessPlayer::onTimeout()
{
	emitForfeit(Chess::Result::WinByTimeout);
}

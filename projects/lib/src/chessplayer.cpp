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

#include "chessplayer.h"
#include <QTimer>
#include "board/board.h"


ChessPlayer::ChessPlayer(QObject* parent)
	: QObject(parent),
	  m_state(NotStarted),
	  m_timer(new QTimer(this)),
	  m_claimedResult(false),
	  m_validateClaims(true),
	  m_canPlayAfterTimeout(false),
	  m_board(nullptr),
	  m_opponent(nullptr)
{
	m_timer->setSingleShot(true);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

ChessPlayer::~ChessPlayer()
{
}

bool ChessPlayer::isReady() const
{
	switch (m_state)
	{
	case Idle:
	case Observing:
	case Thinking:
	case Disconnected:
		return true;
	default:
		return false;
	}
}

void ChessPlayer::newGame(Chess::Side side, ChessPlayer* opponent, Chess::Board* board)
{
	Q_ASSERT(opponent != nullptr);
	Q_ASSERT(board != nullptr);
	Q_ASSERT(isReady());
	Q_ASSERT(m_state != Disconnected);

	m_claimedResult = false;
	m_eval.clear();
	m_opponent = opponent;
	m_board = board;
	m_side = side;
	m_timeControl.initialize();

	setState(Observing);
	startGame();
}

void ChessPlayer::endGame(const Chess::Result& result)
{
	Q_UNUSED(result);
	if (m_state != Observing && m_state != Thinking)
		return;

	Q_ASSERT(m_state != Disconnected);
	setState(FinishingGame);
	m_board = nullptr;
	m_timer->stop();
	disconnect(this, SIGNAL(ready()), this, SLOT(go()));
}

void ChessPlayer::go()
{
	if (m_state == Disconnected)
		return;
	setState(Thinking);

	disconnect(this, SIGNAL(ready()), this, SLOT(go()));
	if (!isReady())
	{
		connect(this, SIGNAL(ready()), this, SLOT(go()));
		return;
	}

	Q_ASSERT(m_board != nullptr);
	m_side = m_board->sideToMove();
	
	startClock();
	startThinking();
}

void ChessPlayer::quit()
{
	setState(Disconnected);
	emit disconnected();
}

const MoveEvaluation& ChessPlayer::evaluation() const
{
	return m_eval;
}

void ChessPlayer::startClock()
{
	if (m_state != Thinking)
		return;

	m_eval.clear();

	if (m_timeControl.isValid())
		emit startedThinking(m_timeControl.timeLeft());

	m_timeControl.startTimer();

	if (!m_timeControl.isInfinite())
	{
		int t = m_timeControl.timeLeft() + m_timeControl.expiryMargin();
		m_timer->start(qMax(t, 0) + 200);
	}
}

void ChessPlayer::makeBookMove(const Chess::Move& move)
{
	m_timeControl.startTimer();
	makeMove(move);
	m_timeControl.update(false);
	m_eval.setBookEval(true);

	emit moveMade(move);
}

const TimeControl* ChessPlayer::timeControl() const
{
	return &m_timeControl;
}

void ChessPlayer::setTimeControl(const TimeControl& timeControl)
{
	m_timeControl = timeControl;
}

Chess::Side ChessPlayer::side() const
{
	return m_side;
}

Chess::Board* ChessPlayer::board()
{
	return m_board;
}

const ChessPlayer* ChessPlayer::opponent() const
{
	return m_opponent;
}

ChessPlayer::State ChessPlayer::state() const
{
	return m_state;
}

bool ChessPlayer::hasError() const
{
	return !m_error.isEmpty();
}

QString ChessPlayer::errorString() const
{
	return m_error;
}

void ChessPlayer::setState(State state)
{
	if (state == m_state)
		return;
	if (m_state == Thinking)
		emit stoppedThinking();

	m_state = state;
}

void ChessPlayer::setError(const QString& error)
{
	m_error = error;
}

QString ChessPlayer::name() const
{
	return m_name;
}

void ChessPlayer::setName(const QString& name)
{
	m_name = name;
	emit nameChanged(m_name);
}

bool ChessPlayer::canPlayAfterTimeout() const
{
	return m_canPlayAfterTimeout;
}

void ChessPlayer::setCanPlayAfterTimeout(bool enable)
{
        m_canPlayAfterTimeout = enable;
}

void ChessPlayer::startPondering()
{
}

void ChessPlayer::clearPonderState()
{
}

bool ChessPlayer::areClaimsValidated() const
{
	return m_validateClaims;
}

void ChessPlayer::setClaimsValidated(bool validate)
{
	m_validateClaims = validate;
}

void ChessPlayer::claimResult(const Chess::Result& result)
{
	if (m_claimedResult)
		return;

	m_timer->stop();
	m_timeControl.update();
	if (m_state == Thinking)
		setState(Observing);
	m_claimedResult = true;

	emit resultClaim(result);
}

void ChessPlayer::forfeit(Chess::Result::Type type, const QString& description)
{
	if (m_side.isNull())
	{
		claimResult(Chess::Result(type, m_side, description));
		return;
	}

	claimResult(Chess::Result(type, m_side.opposite(), description));
}

void ChessPlayer::emitMove(const Chess::Move& move)
{
	if (m_state == Thinking)
		setState(Observing);

	m_timeControl.update();
	m_eval.setTime(m_timeControl.lastMoveTime());
	m_eval.setIsTrusted(!areClaimsValidated());

	m_timer->stop();
	if (m_timeControl.expired() && !canPlayAfterTimeout())
	{
		forfeit(Chess::Result::Timeout);
		return;
	}

	emit moveMade(move);
}

void ChessPlayer::kill()
{
	setState(Disconnected);
	emit disconnected();
}

void ChessPlayer::onCrashed()
{
	kill();
	forfeit(Chess::Result::Disconnection);
}

void ChessPlayer::onTimeout()
{
	if (!canPlayAfterTimeout())
		forfeit(Chess::Result::Timeout);
}

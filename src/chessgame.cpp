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

#include "chessgame.h"
#include "chessboard/standardchessboard.h"
#include "chessboard/chessmove.h"
#include "chessplayer.h"
#include "timecontrol.h"

ChessGame::ChessGame(QObject *parent)
	: QObject(parent)
{
	m_moveCount = 0;
	m_whitePlayer = 0;
	m_blackPlayer = 0;
	m_playerToMove = 0;
	m_gameInProgress = false;
	m_chessboard = new StandardChessboard();
}

ChessGame::~ChessGame()
{
	delete m_chessboard;
}

Chessboard* ChessGame::chessboard() const
{
	return m_chessboard;
}

void ChessGame::moveMade(const ChessMove& move)
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != 0);

	if (!m_gameInProgress)
	{
		qDebug("%s sent a move when no game is in progress", qPrintable(sender->name()));
		return;
	}

	if (sender != m_playerToMove)
	{
		qDebug("%s tried to make a move on the opponent's turn", qPrintable(sender->name()));
		return;
	}

	if (move.isEmpty())
	{
		qDebug("%s sent an illegal move", qPrintable(sender->name()));
		return;
	}

	m_moveCount++;

	// Get the elapsed time and update the time control
	TimeControl playerTimeControl = m_playerToMove->timeControl();
	playerTimeControl.update(m_timer.elapsed());
	m_playerToMove->setTimeControl(playerTimeControl);

	m_playerToMove = (m_playerToMove == m_whitePlayer) ? m_blackPlayer : m_whitePlayer;

	// Start counting the duration of the next move
	m_timer.start();

	m_playerToMove->makeMove(move);
	m_chessboard->makeMove(move);
	
	if (m_chessboard->result() == Chessboard::NoResult)
		m_playerToMove->go();
	else
	{
		m_gameInProgress = false;
		qDebug("Game ended");
	}
	
	emit moveHappened(move);
}

void ChessGame::newGame(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer)
{
	m_chessboard->setFenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");

	m_moveCount = 0;
	m_whitePlayer = whitePlayer;
	m_blackPlayer = blackPlayer;

	connect(m_whitePlayer, SIGNAL(moveMade(const ChessMove&)),
	        this, SLOT(moveMade(const ChessMove&)));
	connect(m_blackPlayer, SIGNAL(moveMade(const ChessMove&)),
	        this, SLOT(moveMade(const ChessMove&)));

	m_gameInProgress = true;

	m_whitePlayer->newGame(Chessboard::White, m_blackPlayer);
	m_blackPlayer->newGame(Chessboard::Black, m_whitePlayer);
	m_playerToMove = m_whitePlayer;
	
	m_timer.start();
	m_playerToMove->go();
}

ChessPlayer* ChessGame::whitePlayer() const
{
	return m_whitePlayer;
}

ChessPlayer* ChessGame::blackPlayer() const
{
	return m_blackPlayer;
}


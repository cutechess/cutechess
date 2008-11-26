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
#include "chessboard/chessboard.h"
#include "chessplayer.h"
#include "timecontrol.h"
#include "chessboard/openingbook.h"
#include "pgngame.h"


ChessGame::ChessGame(QObject *parent, Chess::Variant variant)
	: QObject(parent),
	  m_whitePlayer(0),
	  m_blackPlayer(0),
	  m_playerToMove(0),
	  m_book(0),
	  m_gameInProgress(false),
	  m_moveCount(0),
	  m_result(Chess::NoResult)
{
	m_chessboard = new Chess::Board(variant);
}

ChessGame::~ChessGame()
{
	delete m_chessboard;
}

Chess::Board* ChessGame::chessboard() const
{
	return m_chessboard;
}

void ChessGame::endGame()
{
	if (m_gameInProgress)
	{
		m_gameInProgress = false;
		// PgnGame(this).write("games.pgn");
		qDebug("Game ended");
	}
}

void ChessGame::moveMade(const Chess::Move& move)
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

	if (!m_chessboard->isLegalMove(move))
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
	
	m_result = m_chessboard->result();
	if (m_result == Chess::NoResult)
		m_playerToMove->go();
	else
		endGame();
	
	emit moveHappened(move);
}

void ChessGame::resign()
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != 0);

	if (m_result == Chess::NoResult)
	{
		if (sender == m_whitePlayer)
			m_result = Chess::WhiteResigns;
		else if (sender == m_blackPlayer)
			m_result = Chess::BlackResigns;
	}
	endGame();
}

Chess::Move ChessGame::bookMove()
{
	if (m_book == 0)
		return Chess::Move(0, 0);
	
	BookMove bookMove = m_book->move(m_chessboard->key());
	return m_chessboard->moveFromBook(bookMove);
}

void ChessGame::newGame(ChessPlayer* whitePlayer,
                        ChessPlayer* blackPlayer,
                        OpeningBook* book)
{
	m_chessboard->setBoard(Chess::standardFen);

	m_result = Chess::NoResult;
	m_moveCount = 0;
	m_whitePlayer = whitePlayer;
	m_blackPlayer = blackPlayer;
	m_book = book;

	connect(m_whitePlayer, SIGNAL(moveMade(const Chess::Move&)),
	        this, SLOT(moveMade(const Chess::Move&)));
	connect(m_blackPlayer, SIGNAL(moveMade(const Chess::Move&)),
	        this, SLOT(moveMade(const Chess::Move&)));
	
	connect(m_whitePlayer, SIGNAL(resign()), this, SLOT(resign()));
	connect(m_blackPlayer, SIGNAL(resign()), this, SLOT(resign()));

	m_gameInProgress = true;

	m_whitePlayer->newGame(Chess::White, m_blackPlayer);
	m_blackPlayer->newGame(Chess::Black, m_whitePlayer);
	if (m_chessboard->sideToMove() == Chess::White)
		m_playerToMove = m_whitePlayer;
	else
		m_playerToMove = m_blackPlayer;
	
	// Play the opening book moves first
	Chess::Move move;
	while (m_chessboard->isLegalMove(move = bookMove())
	   &&  !m_chessboard->isRepeatMove(move)
	   &&  m_moveCount < 30)
	{
		// Update the time control
		TimeControl playerTimeControl = m_playerToMove->timeControl();
		playerTimeControl.update(0);
		m_playerToMove->setTimeControl(playerTimeControl);
		
		m_playerToMove->makeMove(move);
		m_playerToMove = (m_playerToMove == m_whitePlayer) ? m_blackPlayer : m_whitePlayer;
		
		m_playerToMove->makeMove(move);
		m_chessboard->makeMove(move);
		
		Q_ASSERT(m_chessboard->result() == Chess::NoResult);
		emit moveHappened(move);
	}
	
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

Chess::Result ChessGame::result() const
{
	return m_result;
}

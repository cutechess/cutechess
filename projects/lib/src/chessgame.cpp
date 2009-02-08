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

#include "chessgame.h"
#include <QtDebug>
#include "chessboard/chessboard.h"
#include "chessplayer.h"
#include "openingbook.h"
#include "pgngame.h"


ChessGame::ChessGame(Chess::Variant variant, QObject* parent)
	: QObject(parent),
	  m_book(0),
	  m_gameInProgress(false),
	  m_moveCount(0),
	  m_result(Chess::NoResult)
{
	m_player[Chess::White] = 0;
	m_player[Chess::Black] = 0;
	m_board = new Chess::Board(variant);
}

ChessGame::~ChessGame()
{
	delete m_board;
}

Chess::Board* ChessGame::board() const
{
	return m_board;
}

ChessPlayer* ChessGame::playerToMove()
{
	if (m_board->sideToMove() == Chess::NoSide)
		return 0;
	return m_player[m_board->sideToMove()];
}

ChessPlayer* ChessGame::playerToWait()
{
	if (m_board->sideToMove() == Chess::NoSide)
		return 0;
	return m_player[!((int)m_board->sideToMove())];
}

void ChessGame::endGame()
{
	if (m_gameInProgress)
	{
		m_gameInProgress = false;
		// PgnGame(this).write("games.pgn");
		qDebug() << "Game ended";
	}
	emit gameEnded();
}

void ChessGame::moveMade(const Chess::Move& move)
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != 0);

	if (!m_gameInProgress)
	{
		qDebug() << sender->name() << "sent a move when no game is in progress";
		return;
	}

	if (sender != playerToMove())
	{
		qDebug() << sender->name() << "tried to make a move on the opponent's turn";
		return;
	}

	if (!m_board->isLegalMove(move))
	{
		qDebug() << sender->name() << "sent an illegal move";
		return;
	}

	m_moveCount++;

	playerToWait()->makeMove(move);
	m_board->makeMove(move, true);
	
	m_result = m_board->result();
	if (m_result == Chess::NoResult)
		playerToMove()->go();
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
		if (sender == m_player[Chess::White])
			m_result = Chess::WhiteResigns;
		else if (sender == m_player[Chess::Black])
			m_result = Chess::BlackResigns;
	}
	endGame();
}

void ChessGame::onTimeout()
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != 0);
	Q_ASSERT(sender == playerToMove());
	
	if (sender == m_player[Chess::White])
	{
		m_result = Chess::WhiteResigns;
		qDebug() << "White loses on time";
	}
	else if (sender == m_player[Chess::Black])
	{
		m_result = Chess::BlackResigns;
		qDebug() << "Black loses on time";
	}
	endGame();
}

Chess::Move ChessGame::bookMove()
{
	if (m_book == 0)
		return Chess::Move(0, 0);
	
	BookMove bookMove = m_book->move(m_board->key());
	return m_board->moveFromBook(bookMove);
}

void ChessGame::setPlayer(Chess::Side side, ChessPlayer* player)
{
	Q_ASSERT(side != Chess::NoSide);
	Q_ASSERT(player != 0);
	m_player[side] = player;

	connect(player, SIGNAL(moveMade(const Chess::Move&)),
	        this, SLOT(moveMade(const Chess::Move&)));
	connect(player, SIGNAL(resign()), this, SLOT(resign()));
	connect(player, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void ChessGame::setFenString(const QString& fen)
{
	m_fen = fen;
}

void ChessGame::setOpeningBook(OpeningBook* book)
{
	Q_ASSERT(book != 0);
	m_book = book;
}

void ChessGame::start()
{
	if (m_fen.isEmpty())
		m_board->setBoard();
	else
		m_board->setBoard(m_fen);
	
	m_gameInProgress = true;

	for (int i = 0; i < 2; i++)
	{
		Q_ASSERT(m_player[i] != 0);
		m_player[i]->newGame((Chess::Side)i, m_player[!i]);
	}
	
	// Play the opening book moves first
	Chess::Move move;
	while (m_board->isLegalMove(move = bookMove())
	   &&  !m_board->isRepeatMove(move)
	   &&  m_moveCount < 30)
	{
		playerToMove()->makeBookMove(move);
		
		playerToWait()->makeMove(move);
		m_board->makeMove(move, true);
		
		Q_ASSERT(m_board->result() == Chess::NoResult);
		emit moveHappened(move);
	}
	
	playerToMove()->go();
}

ChessPlayer* ChessGame::player(Chess::Side side) const
{
	Q_ASSERT(side != Chess::NoSide);
	return m_player[side];
}

Chess::Result ChessGame::result() const
{
	return m_result;
}

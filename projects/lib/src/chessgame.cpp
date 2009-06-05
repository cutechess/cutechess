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
	  PgnGame(variant),
	  m_gameEnded(false),
	  m_gameInProgress(false),
	  m_drawMoveNum(0),
	  m_drawScore(0),
	  m_drawScoreCount(0),
	  m_resignMoveCount(0),
	  m_resignScore(0)
{
	m_resignScoreCount[Chess::White] = 0;
	m_resignScoreCount[Chess::Black] = 0;

	m_player[Chess::White] = 0;
	m_player[Chess::Black] = 0;
	m_board = new Chess::Board(variant, this);
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

void ChessGame::stop()
{
	if (m_gameEnded)
		return;

	m_gameEnded = true;
	if (!m_gameInProgress)
	{
		setResult(Chess::Result());
		emit gameEnded();
		return;
	}
	
	m_gameInProgress = false;

	m_player[Chess::White]->endGame(result());
	m_player[Chess::Black]->endGame(result());

	connect(this, SIGNAL(playersReady()), this, SIGNAL(gameEnded()), Qt::QueuedConnection);
	syncPlayers(true);
}

void ChessGame::kill()
{
	m_player[Chess::White]->closeConnection();
	m_player[Chess::Black]->closeConnection();

	stop();
}

void ChessGame::adjudication(const MoveEvaluation& eval)
{
	int side = !((int)m_board->sideToMove());
	if (eval.depth() <= 0)
	{
		m_drawScoreCount = 0;
		m_resignScoreCount[side] = 0;
		return;
	}

	// Draw adjudication
	if (m_drawMoveNum > 0)
	{
		if (qAbs(eval.score()) <= m_drawScore)
			m_drawScoreCount++;
		else
			m_drawScoreCount = 0;
		if (moves().size() / 2 >= m_drawMoveNum
		&&  m_drawScoreCount >= 2)
		{
			setResult(Chess::Result(Chess::Result::DrawByAdjudication));
			return;
		}
	}

	// Resign adjudication
	if (m_resignMoveCount > 0)
	{
		int& count = m_resignScoreCount[side];
		if (eval.score() <= m_resignScore)
			count++;
		else
			count = 0;
		if (count >= m_resignMoveCount)
		{
			Chess::Result::Code code = Chess::Result::WinByAdjudication;
			Chess::Side winner = Chess::Side(!side);
			setResult(Chess::Result(code, winner));
		}
	}
}

void ChessGame::onMoveMade(const Chess::Move& move)
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != 0);

	Q_ASSERT(m_gameInProgress);
	Q_ASSERT(m_board->isLegalMove(move));
	if (sender != playerToMove())
	{
		qDebug() << sender->name() << "tried to make a move on the opponent's turn";
		return;
	}

	QString comment;

	// Save the evaluation as a PGN comment
	const MoveEvaluation& eval = sender->evaluation();
	if (!eval.isEmpty())
	{
		if (eval.depth() > 0)
		{
			if (eval.score() > 0)
				comment += "+";
			comment += QString::number((double)eval.score() / 100.0, 'f', 2) + '/';
			comment += QString::number(eval.depth()) + ' ';
		}
		// Round the time to the nearest second
		comment += QString::number((eval.time() + 500) / 1000) + 's';
	}

	addMove(move, m_board, comment);

	// Get the result before sending the move to the opponent
	m_board->makeMove(move, false);
	setResult(m_board->result());
	if (result().isNone())
		adjudication(eval);
	m_board->undoMove();

	ChessPlayer* player = playerToWait();
	player->makeMove(move);

	if (result().isNone())
		player->go();
	else
		stop();

	m_board->makeMove(move, true);
	emit moveMade(move);
}

void ChessGame::onForfeit(Chess::Result result)
{
	if (m_gameEnded)
		return;

	setResult(result);
	stop();
}

Chess::Move ChessGame::bookMove(const OpeningBook* book)
{
	if (book == 0)
		return Chess::Move(0, 0);
	
	GenericMove bookMove = book->move(m_board->key());
	return m_board->moveFromBook(bookMove);
}

void ChessGame::setPlayer(Chess::Side side, ChessPlayer* player)
{
	Q_ASSERT(side != Chess::NoSide);
	Q_ASSERT(player != 0);
	m_player[side] = player;

	connect(player, SIGNAL(moveMade(const Chess::Move&)),
	        this, SLOT(onMoveMade(const Chess::Move&)));
	connect(player, SIGNAL(forfeit(Chess::Result)),
		this, SLOT(onForfeit(Chess::Result)));
}

bool ChessGame::setFenString(const QString& fen)
{
	if (!m_board->setBoard(fen))
		return false;
	setStartingFen(fen);
	return true;
}

void ChessGame::setOpeningBook(const OpeningBook* book, int maxMoves)
{
	Q_ASSERT(book != 0);
	Q_ASSERT(!m_gameInProgress);
	
	setBoard();
	m_moves.clear();
	for (int i = 0; i < maxMoves; i++)
	{
		Chess::Move move = bookMove(book);
		if (!m_board->isLegalMove(move)
		||  m_board->isRepeatMove(move))
			break;
		
		addMove(move, m_board);

		m_board->makeMove(move);
		if (!m_board->result().isNone())
		{
			m_moves.pop_back();
			break;
		}
	}
}

void ChessGame::setOpeningMoves(const QVector<PgnGame::MoveData>& moves)
{
	Q_ASSERT(!m_gameInProgress);
	m_moves = moves;
}

void ChessGame::setDrawThreshold(int moveNumber, int score)
{
	m_drawMoveNum = moveNumber;
	m_drawScore = score;
}

void ChessGame::setResignThreshold(int moveCount, int score)
{
	m_resignMoveCount = moveCount;
	m_resignScore = score;
}

void ChessGame::setBoard()
{
	QString fen = startingFen();
	if (fen.isEmpty())
	{
		fen = m_board->variant().startingFen();
		setStartingFen(fen);
	}

	if (!m_board->setBoard(fen))
		qFatal("Invalid FEN: %s", qPrintable(fen));
}

bool ChessGame::arePlayersReady() const
{
	return (m_player[0]->isReady() && m_player[1]->isReady());
}

void ChessGame::syncPlayers(bool ignoreSender)
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());

	if (ignoreSender && sender != 0)
	{
		// There should be no sender, but for some reason we've got one
		disconnect(sender, SIGNAL(ready()), this, SLOT(syncPlayers()));
		sender = 0;
	}

	if (!sender)
	{
		bool ready = true;
		for (int i = 0; i < 2; i++)
		{
			if (!m_player[i]->isReady())
			{
				ready = false;
				connect(m_player[i], SIGNAL(ready()),
					this, SLOT(syncPlayers()));
			}
		}
		if (!ready)
			return;
	}
	else
	{
		disconnect(sender, SIGNAL(ready()), this, SLOT(syncPlayers()));
		if (!arePlayersReady() || sender->state() == ChessPlayer::Disconnected)
			return;
	}

	emit playersReady();
}

void ChessGame::start()
{
	setResult(Chess::Result());

	disconnect(this, SIGNAL(playersReady()), this, SLOT(start()));
	if (!arePlayersReady())
	{
		connect(this, SIGNAL(playersReady()), this, SLOT(start()));
		syncPlayers(true);
		return;
	}
	if (m_gameEnded)
		return;
	
	m_gameInProgress = true;
	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player = m_player[i];
		Q_ASSERT(player != 0);
		Q_ASSERT(player->isReady());
		
		if (!player->supportsVariant(m_board->variant()))
		{
			qDebug() << player->name() << "doesn't support variant"
				 << m_board->variant().toString();
			setResult(Chess::Result(Chess::Result::ResultError));
			stop();
			return;
		}	
	}
	
	setBoard();
	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side(i);
		setPlayerName(side, m_player[i]->name());
		setTimeControl(*m_player[i]->timeControl(), side);
		m_player[i]->newGame(side, m_player[!i], m_board);
	}
	
	// Play the forced opening moves first
	QVector<PgnGame::MoveData>::iterator it;
	for (it = m_moves.begin(); it != m_moves.end(); ++it)
	{
		const Chess::Move& move = it->move;
		Q_ASSERT(m_board->isLegalMove(move));
		
		it->comment = "book";
		playerToMove()->makeBookMove(move);
		playerToWait()->makeMove(move);
		m_board->makeMove(move, true);
		
		emit moveMade(move);

		if (!m_board->result().isNone())
		{
			qDebug() << "Every move was played from the book";
			setResult(m_board->result());
			stop();
			return;
		}
	}
	
	playerToMove()->go();
}

ChessPlayer* ChessGame::player(Chess::Side side) const
{
	Q_ASSERT(side != Chess::NoSide);
	return m_player[side];
}

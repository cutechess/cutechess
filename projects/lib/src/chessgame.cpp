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
#include "board/board.h"
#include "chessplayer.h"
#include "openingbook.h"


ChessGame::ChessGame(Chess::Board* board, QObject* parent)
	: QObject(parent),
	  PgnGame(board->variant()),
	  m_origThread(0),
	  m_board(board),
	  m_gameEnded(false),
	  m_gameInProgress(false),
	  m_drawMoveNum(0),
	  m_drawScore(0),
	  m_drawScoreCount(0),
	  m_resignMoveCount(0),
	  m_resignScore(0)
{
	for (int i = 0; i < 2; i++)
	{
		m_resignScoreCount[i] = 0;
		m_player[i] = 0;
		m_book[i] = 0;
		m_bookDepth[i] = 0;
	}

	emit humanEnabled(false);
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
	return m_player[Chess::otherSide(m_board->sideToMove())];
}

void ChessGame::stop()
{
	if (m_gameEnded)
		return;

	m_gameEnded = true;
	emit humanEnabled(false);
	if (!m_gameInProgress)
	{
		setResult(Chess::Result());
		finish();
		return;
	}
	
	m_gameInProgress = false;

	m_player[Chess::White]->endGame(result());
	m_player[Chess::Black]->endGame(result());
	
	connect(this, SIGNAL(playersReady()), this, SLOT(finish()), Qt::QueuedConnection);
	syncPlayers(true);
}

void ChessGame::sendGameEnded()
{
	emit gameEnded();
}

void ChessGame::finish()
{
	disconnect(this, SIGNAL(playersReady()), this, SLOT(finish()));
	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] != 0)
			m_player[i]->disconnect(this);
	}

	// Change thread affinity back to the way it was before the game
	if (m_origThread != 0 && thread() != m_origThread)
	{
		moveToThread(m_origThread);
		for (int i = 0; i < 2; i++)
		{
			if (m_player[i] != 0)
				m_player[i]->moveToThread(m_origThread);
		}
		m_origThread = 0;
	}

	QMetaObject::invokeMethod(this, "sendGameEnded", Qt::QueuedConnection);
}

void ChessGame::kill()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] != 0)
			m_player[i]->closeConnection();
	}

	stop();
}

void ChessGame::adjudication(const MoveEvaluation& eval)
{
	Chess::Side side(Chess::otherSide(m_board->sideToMove()));

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
			setResult(Chess::Result(Chess::Result::Adjudication, Chess::NoSide));
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
			setResult(Chess::Result(Chess::Result::Adjudication,
						Chess::otherSide(side)));
	}
}

static QString evalString(const MoveEvaluation& eval)
{
	if (eval.isBookEval())
		return "book";
	if (eval.isEmpty())
		return QString();

	QString str;
	if (eval.depth() > 0)
	{
		int score = eval.score();
		int absScore = qAbs(score);
		if (score > 0)
			str += "+";

		// Detect mate-in-n scores
		if (absScore > 9900
		&&  (absScore = 1000 - (absScore % 1000)) < 100)
		{
			if (score < 0)
				str += "-";
			str += "M" + QString::number(absScore);
		}
		else
			str += QString::number(double(score) / 100.0, 'f', 2);

		str += "/" + QString::number(eval.depth()) + " ";
	}

	int precision = 0;
	int t = eval.time();
	if (t < 100)
		precision = 3;
	else if (t < 1000)
		precision = 2;
	else if (t < 10000)
		precision = 1;
	str += QString::number(double(t / 1000.0), 'f', precision) + 's';

	return str;
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

	addMove(move, m_board, evalString(sender->evaluation()));

	// Get the result before sending the move to the opponent
	m_board->makeMove(move, false);
	setResult(m_board->result());
	if (result().isNone())
		adjudication(sender->evaluation());
	m_board->undoMove();

	ChessPlayer* player = playerToWait();
	player->makeMove(move);
	m_board->makeMove(move, true);

	if (result().isNone())
		startTurn();
	else
		stop();

	emit moveMade(move);
}

void ChessGame::startTurn()
{
	Chess::Side side(m_board->sideToMove());
	Q_ASSERT(side != Chess::NoSide);

	Chess::Move move(bookMove(side));
	if (move.isNull())
		m_player[side]->go();
	else
		m_player[side]->makeBookMove(move);

	emit humanEnabled(m_player[side]->isHuman());
}

void ChessGame::onForfeit(Chess::Result result)
{
	if (m_gameEnded)
		return;

	if (!m_gameInProgress && result.winner() == Chess::NoSide)
	{
		ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
		Q_ASSERT(sender != 0);
		qWarning("%s: %s", qPrintable(sender->name()),
				   qPrintable(result.description()));
	}

	setResult(result);
	stop();
}

Chess::Move ChessGame::bookMove(Chess::Side side)
{
	Q_ASSERT(side != Chess::NoSide);

	if (m_book[side] == 0
	||  m_moves.size() >= m_bookDepth[side] * 2)
		return Chess::Move();

	Chess::GenericMove bookMove = m_book[side]->move(m_board->key());
	return m_board->moveFromGenericMove(bookMove);
}

void ChessGame::setPlayer(Chess::Side side, ChessPlayer* player)
{
	Q_ASSERT(side != Chess::NoSide);
	Q_ASSERT(player != 0);
	m_player[side] = player;
}

bool ChessGame::setFenString(const QString& fen)
{
	if (!m_board->setFenString(fen))
		return false;
	setStartingFen(fen);
	return true;
}

void ChessGame::generateOpening()
{
	if (m_book[Chess::White] == 0 || m_book[Chess::Black] == 0)
		return;
	setBoard();

	// First play moves that are already in the move
	// list (eg. moves from a PGN game)
	QVector<PgnGame::MoveData>::iterator it;
	for (it = m_moves.begin(); it != m_moves.end(); ++it)
	{
		const Chess::Move& move = it->move;
		Q_ASSERT(m_board->isLegalMove(move));

		m_board->makeMove(move);
		if (!m_board->result().isNone())
			return;
	}

	// Then play the opening book moves
	forever
	{
		Chess::Move move = bookMove(m_board->sideToMove());
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

void ChessGame::setOpeningBook(const OpeningBook* book,
                               Chess::Side side,
			       int depth)
{
	Q_ASSERT(!m_gameInProgress);
	
	if (side == Chess::NoSide)
	{
		setOpeningBook(book, Chess::White, depth);
		setOpeningBook(book, Chess::Black, depth);
	}
	else
	{
		m_book[side] = book;
		m_bookDepth[side] = depth;
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
		fen = m_board->defaultFenString();
		setStartingFen(fen);
	}

	if (!m_board->setFenString(fen))
		qFatal("Invalid FEN: %s", qPrintable(fen));
}

bool ChessGame::arePlayersReady() const
{
	return (m_player[Chess::White]->isReady() &&
	        m_player[Chess::Black]->isReady());
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

void ChessGame::start(QThread* thread)
{
	m_origThread = 0;
	if (thread != 0)
	{
		Q_ASSERT(parent() == 0);
		Q_ASSERT(m_player[Chess::White]->parent() == 0);
		Q_ASSERT(m_player[Chess::Black]->parent() == 0);

		m_origThread = this->thread();
		moveToThread(thread);
		m_player[Chess::White]->moveToThread(thread);
		m_player[Chess::Black]->moveToThread(thread);
	}

	// Start the game in the correct thread
	QMetaObject::invokeMethod(this, "startGame", Qt::QueuedConnection);
}

void ChessGame::startGame()
{
	setResult(Chess::Result());
	emit humanEnabled(false);

	for (int i = 0; i < 2; i++)
	{
		connect(m_player[i], SIGNAL(forfeit(Chess::Result)),
			this, SLOT(onForfeit(Chess::Result)));
	}

	disconnect(this, SIGNAL(playersReady()), this, SLOT(startGame()));
	if (!arePlayersReady())
	{
		connect(this, SIGNAL(playersReady()), this, SLOT(startGame()));
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
				 << m_board->variant();
			setResult(Chess::Result(Chess::Result::ResultError));
			stop();
			return;
		}	
	}
	
	setBoard();
	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side(i);

		setPlayerName(side, m_player[side]->name());
		Q_ASSERT(timeControl(side).isValid());
		m_player[side]->setTimeControl(timeControl(side));
		m_player[side]->newGame(side, m_player[Chess::otherSide(side)], m_board);
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
	
	for (int i = 0; i < 2; i++)
	{
		connect(m_player[i], SIGNAL(moveMade(const Chess::Move&)),
			this, SLOT(onMoveMade(const Chess::Move&)));
	}
	
	startTurn();
}

ChessPlayer* ChessGame::player(Chess::Side side) const
{
	Q_ASSERT(side != Chess::NoSide);
	return m_player[side];
}

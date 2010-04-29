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
	  m_board(board),
	  m_origThread(0),
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

ChessPlayer* ChessGame::player(Chess::Side side) const
{
	Q_ASSERT(!side.isNull());
	return m_player[side];
}

PgnGame& ChessGame::pgn()
{
	return m_pgn;
}

QString ChessGame::startingFen() const
{
	return m_startingFen;
}

const QVector<Chess::Move>& ChessGame::moves() const
{
	return m_moves;
}

Chess::Result ChessGame::result() const
{
	return m_result;
}

ChessPlayer* ChessGame::playerToMove()
{
	if (m_board->sideToMove().isNull())
		return 0;
	return m_player[m_board->sideToMove()];
}

ChessPlayer* ChessGame::playerToWait()
{
	if (m_board->sideToMove().isNull())
		return 0;
	return m_player[m_board->sideToMove().opposite()];
}

void ChessGame::stop()
{
	if (m_gameEnded)
		return;

	m_gameEnded = true;
	emit humanEnabled(false);
	if (!m_gameInProgress)
	{
		m_result = Chess::Result();
		finish();
		return;
	}
	
	m_gameInProgress = false;
	m_pgn.setTag("PlyCount", QString::number(m_pgn.moves().size()));
	m_pgn.setResult(m_result);
	m_pgn.setResultDescription(m_result.description());

	m_player[Chess::Side::White]->endGame(m_result);
	m_player[Chess::Side::Black]->endGame(m_result);
	
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
	Chess::Side side(m_board->sideToMove().opposite());

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
		if (m_moves.size() / 2 >= m_drawMoveNum
		&&  m_drawScoreCount >= 2)
		{
			m_result = Chess::Result(Chess::Result::Adjudication, Chess::Side::NoSide);
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
			m_result = Chess::Result(Chess::Result::Adjudication,
						 side.opposite());
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

	int t = eval.time();
	if (t == 0)
		return str + "0s";

	int precision = 0;
	if (t < 100)
		precision = 3;
	else if (t < 1000)
		precision = 2;
	else if (t < 10000)
		precision = 1;
	str += QString::number(double(t / 1000.0), 'f', precision) + 's';

	return str;
}

void ChessGame::addPgnMove(const Chess::Move& move, const QString& comment)
{
	PgnGame::MoveData md;
	md.key = m_board->key();
	md.move = m_board->genericMove(move);
	md.moveString = m_board->moveString(move, Chess::Board::StandardAlgebraic);
	md.comment = comment;

	m_pgn.addMove(md);
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

	m_moves.append(move);
	addPgnMove(move, evalString(sender->evaluation()));

	// Get the result before sending the move to the opponent
	m_board->makeMove(move, false);
	m_result = m_board->result();
	if (m_result.isNone())
		adjudication(sender->evaluation());
	m_board->undoMove();

	ChessPlayer* player = playerToWait();
	player->makeMove(move);
	m_board->makeMove(move, true);

	if (m_result.isNone())
		startTurn();
	else
		stop();

	emit moveMade(move);
}

void ChessGame::startTurn()
{
	Chess::Side side(m_board->sideToMove());
	Q_ASSERT(!side.isNull());

	Chess::Move move(bookMove(side));
	if (move.isNull())
		m_player[side]->go();
	else
		m_player[side]->makeBookMove(move);

	emit humanEnabled(m_player[side]->isHuman());
}

void ChessGame::onForfeit(const Chess::Result& result)
{
	if (m_gameEnded)
		return;

	if (!m_gameInProgress && result.winner().isNull())
	{
		ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
		Q_ASSERT(sender != 0);
		qWarning("%s: %s", qPrintable(sender->name()),
				   qPrintable(result.description()));
	}

	m_result = result;
	stop();
}

Chess::Move ChessGame::bookMove(Chess::Side side)
{
	Q_ASSERT(!side.isNull());

	if (m_book[side] == 0
	||  m_moves.size() >= m_bookDepth[side] * 2)
		return Chess::Move();

	Chess::GenericMove bookMove = m_book[side]->move(m_board->key());
	return m_board->moveFromGenericMove(bookMove);
}

void ChessGame::setPlayer(Chess::Side side, ChessPlayer* player)
{
	Q_ASSERT(!side.isNull());
	Q_ASSERT(player != 0);
	m_player[side] = player;
}

void ChessGame::setStartingFen(const QString& fen)
{
	Q_ASSERT(!m_gameInProgress);
	m_startingFen = fen;
}

void ChessGame::setTimeControl(const TimeControl& timeControl, Chess::Side side)
{
	if (side != Chess::Side::White)
		m_timeControl[Chess::Side::Black] = timeControl;
	if (side != Chess::Side::Black)
		m_timeControl[Chess::Side::White] = timeControl;
}

void ChessGame::setMoves(const QVector<Chess::Move>& moves)
{
	Q_ASSERT(!m_gameInProgress);
	m_moves = moves;
}

void ChessGame::setMoves(const PgnGame& pgn)
{
	Q_ASSERT(pgn.variant() == m_board->variant());

	setStartingFen(pgn.startingFenString());
	resetBoard();
	m_moves.clear();

	foreach (const PgnGame::MoveData& md, pgn.moves())
	{
		Chess::Move move(m_board->moveFromGenericMove(md.move));
		Q_ASSERT(m_board->isLegalMove(move));

		m_board->makeMove(move);
		if (!m_board->result().isNone())
			return;

		m_moves.append(move);
	}
}

void ChessGame::setOpeningBook(const OpeningBook* book,
			       Chess::Side side,
			       int depth)
{
	Q_ASSERT(!m_gameInProgress);

	if (side.isNull())
	{
		setOpeningBook(book, Chess::Side::White, depth);
		setOpeningBook(book, Chess::Side::Black, depth);
	}
	else
	{
		m_book[side] = book;
		m_bookDepth[side] = depth;
	}
}

void ChessGame::generateOpening()
{
	if (m_book[Chess::Side::White] == 0 || m_book[Chess::Side::Black] == 0)
		return;
	resetBoard();

	// First play moves that are already in the opening
	foreach (const Chess::Move& move, m_moves)
	{
		Q_ASSERT(m_board->isLegalMove(move));

		m_board->makeMove(move);
		if (!m_board->result().isNone())
			return;
	}

	// Then play the opening book moves
	forever
	{
		Chess::Move move = bookMove(m_board->sideToMove());
		if (!m_board->isLegalMove(move) || m_board->isRepeatMove(move))
			break;

		m_board->makeMove(move);
		if (!m_board->result().isNone())
			break;

		m_moves.append(move);
	}
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

void ChessGame::resetBoard()
{
	QString fen(m_startingFen);
	if (fen.isEmpty())
	{
		fen = m_board->defaultFenString();
		if (m_board->isRandomVariant())
			m_startingFen = fen;
	}

	if (!m_board->setFenString(fen))
		qFatal("Invalid FEN string: %s", qPrintable(fen));
}

bool ChessGame::arePlayersReady() const
{
	return (m_player[Chess::Side::White]->isReady() &&
		m_player[Chess::Side::Black]->isReady());
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
		Q_ASSERT(m_player[Chess::Side::White]->parent() == 0);
		Q_ASSERT(m_player[Chess::Side::Black]->parent() == 0);

		m_origThread = this->thread();
		moveToThread(thread);
		m_player[Chess::Side::White]->moveToThread(thread);
		m_player[Chess::Side::Black]->moveToThread(thread);
	}

	// Start the game in the correct thread
	QMetaObject::invokeMethod(this, "startGame", Qt::QueuedConnection);
}

void ChessGame::initializePgn()
{
	m_pgn.setVariant(m_board->variant());
	m_pgn.setStartingFenString(m_board->startingSide(), m_startingFen);
	m_pgn.setDate(QDate::currentDate());
	m_pgn.setPlayerName(Chess::Side::White, m_player[Chess::Side::White]->name());
	m_pgn.setPlayerName(Chess::Side::Black, m_player[Chess::Side::Black]->name());
	m_pgn.setResult(m_result);

	if (m_timeControl[Chess::Side::White] == m_timeControl[Chess::Side::Black])
		m_pgn.setTag("TimeControl", m_timeControl[0].toString());
	else
	{
		m_pgn.setTag("WhiteTimeControl", m_timeControl[Chess::Side::White].toString());
		m_pgn.setTag("BlackTimeControl", m_timeControl[Chess::Side::Black].toString());
	}
}

void ChessGame::startGame()
{
	m_result = Chess::Result();
	emit humanEnabled(false);

	for (int i = 0; i < 2; i++)
	{
		connect(m_player[i], SIGNAL(forfeit(const Chess::Result&)),
			this, SLOT(onForfeit(const Chess::Result&)));
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
			m_result = Chess::Result(Chess::Result::ResultError);
			stop();
			return;
		}	
	}
	
	resetBoard();
	initializePgn();
	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);

		Q_ASSERT(m_timeControl[side].isValid());
		m_player[side]->setTimeControl(m_timeControl[side]);
		m_player[side]->newGame(side, m_player[side.opposite()], m_board);
	}
	
	// Play the forced opening moves first
	for (int i = 0; i < m_moves.size(); i++)
	{
		Chess::Move move(m_moves.at(i));
		Q_ASSERT(m_board->isLegalMove(move));
		
		addPgnMove(move, "book");

		playerToMove()->makeBookMove(move);
		playerToWait()->makeMove(move);
		m_board->makeMove(move, true);
		
		emit moveMade(move);

		if (!m_board->result().isNone())
		{
			qDebug() << "Every move was played from the book";
			m_result = m_board->result();
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

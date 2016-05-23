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
#include <QThread>
#include <QTimer>
#include "board/board.h"
#include "chessplayer.h"
#include "openingbook.h"


ChessGame::ChessGame(Chess::Board* board, PgnGame* pgn, QObject* parent)
	: QObject(parent),
	  m_board(board),
	  m_startDelay(0),
	  m_finished(false),
	  m_gameInProgress(false),
	  m_paused(false),
	  m_pgnInitialized(false),
	  m_pgn(pgn)
{
	Q_ASSERT(pgn != nullptr);

	for (int i = 0; i < 2; i++)
	{
		m_player[i] = nullptr;
		m_book[i] = nullptr;
		m_bookDepth[i] = 0;
	}
}

ChessGame::~ChessGame()
{
	delete m_board;
}

QString ChessGame::errorString() const
{
	return m_error;
}

ChessPlayer* ChessGame::player(Chess::Side side) const
{
	Q_ASSERT(!side.isNull());
	return m_player[side];
}

bool ChessGame::isFinished() const
{
	return m_finished;
}

PgnGame* ChessGame::pgn() const
{
	return m_pgn;
}

Chess::Board* ChessGame::board() const
{
	return m_board;
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

ChessPlayer* ChessGame::playerToMove() const
{
	if (m_board->sideToMove().isNull())
		return nullptr;
	return m_player[m_board->sideToMove()];
}

ChessPlayer* ChessGame::playerToWait() const
{
	if (m_board->sideToMove().isNull())
		return nullptr;
	return m_player[m_board->sideToMove().opposite()];
}

void ChessGame::stop(bool emitMoveChanged)
{
	if (m_finished)
		return;

	m_finished = true;
	emit humanEnabled(false);
	if (!m_gameInProgress)
	{
		m_result = Chess::Result();
		finish();
		return;
	}
	
	initializePgn();
	m_gameInProgress = false;
	const QVector<PgnGame::MoveData>& moves(m_pgn->moves());
	int plies = moves.size();

	m_pgn->setTag("PlyCount", QString::number(plies));
	m_pgn->setResult(m_result);
	m_pgn->setResultDescription(m_result.description());

	if (emitMoveChanged && plies > 1)
	{
		const PgnGame::MoveData& md(moves.at(plies - 1));
		emit moveChanged(plies - 1, md.move, md.moveString, md.comment);
	}

	m_player[Chess::Side::White]->endGame(m_result);
	m_player[Chess::Side::Black]->endGame(m_result);

	connect(this, SIGNAL(playersReady()), this, SLOT(finish()), Qt::QueuedConnection);
	syncPlayers();
}

void ChessGame::finish()
{
	disconnect(this, SIGNAL(playersReady()), this, SLOT(finish()));
	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] != nullptr)
			m_player[i]->disconnect(this);
	}

	emit finished(this);
}

void ChessGame::kill()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] != nullptr)
			m_player[i]->kill();
	}

	stop();
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

	m_pgn->addMove(md);
}

void ChessGame::emitLastMove()
{
	PgnGame::MoveData md(m_pgn->moves().last());
	emit moveMade(md.move, md.moveString, md.comment);
}

void ChessGame::onMoveMade(const Chess::Move& move)
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != nullptr);

	Q_ASSERT(m_gameInProgress);
	Q_ASSERT(m_board->isLegalMove(move));
	if (sender != playerToMove())
	{
		qDebug("%s tried to make a move on the opponent's turn", qPrintable(sender->name()));
		return;
	}

	m_moves.append(move);
	addPgnMove(move, evalString(sender->evaluation()));

	// Get the result before sending the move to the opponent
	m_board->makeMove(move);
	m_result = m_board->result();
	if (m_result.isNone())
	{
		m_adjudicator.addEval(m_board, sender->evaluation());
		m_result = m_adjudicator.result();
	}
	m_board->undoMove();

	ChessPlayer* player = playerToWait();
	player->makeMove(move);
	m_board->makeMove(move);

	if (m_result.isNone())
		startTurn();
	else
		stop(false);

	emitLastMove();
}

void ChessGame::startTurn()
{
	if (m_paused)
		return;

	Chess::Side side(m_board->sideToMove());
	Q_ASSERT(!side.isNull());

	emit humanEnabled(m_player[side]->isHuman());

	Chess::Move move(bookMove(side));
	if (move.isNull())
	{
		m_player[side]->go();
		m_player[side.opposite()]->startPondering();
	}
	else
		m_player[side]->makeBookMove(move);
}

void ChessGame::onResultClaim(const Chess::Result& result)
{
	if (m_finished)
		return;

	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != nullptr);

	if (result.type() == Chess::Result::Disconnection)
	{
		// The engine may not be properly started so we have to
		// figure out the player's side this way
		Chess::Side side(Chess::Side::White);
		if (m_player[side] != sender)
			side = Chess::Side::Black;
		m_result = Chess::Result(result.type(), side.opposite());
	}
	else if (!m_gameInProgress && result.winner().isNull())
	{
		qWarning("Unexpected result claim from %s: %s",
			 qPrintable(sender->name()),
			 qPrintable(result.toVerboseString()));
	}
	else if (sender->areClaimsValidated() && result.loser() != sender->side())
	{
		qWarning("%s forfeits by invalid result claim: %s",
			 qPrintable(sender->name()),
			 qPrintable(result.toVerboseString()));
		m_result = Chess::Result(Chess::Result::Adjudication,
					 sender->side().opposite(),
					 "Invalid result claim");
	}
	else
		m_result = result;

	stop();
}

Chess::Move ChessGame::bookMove(Chess::Side side)
{
	Q_ASSERT(!side.isNull());

	if (m_book[side] == nullptr
	||  m_moves.size() >= m_bookDepth[side] * 2)
		return Chess::Move();

	Chess::GenericMove bookMove = m_book[side]->move(m_board->key());
	Chess::Move move = m_board->moveFromGenericMove(bookMove);
	if (move.isNull())
		return Chess::Move();

	if (!m_board->isLegalMove(move))
	{
		qWarning("Illegal opening book move for %s: %s",
			 qPrintable(side.toString()),
			 qPrintable(m_board->moveString(move, Chess::Board::LongAlgebraic)));
		return Chess::Move();
	}

	if (m_board->isRepetition(move))
		return Chess::Move();

	return move;
}

void ChessGame::setError(const QString& message)
{
	m_error = message;
}

void ChessGame::setPlayer(Chess::Side side, ChessPlayer* player)
{
	Q_ASSERT(!side.isNull());
	Q_ASSERT(player != nullptr);
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

bool ChessGame::setMoves(const PgnGame& pgn)
{
	if (pgn.variant() != m_board->variant())
		return false;

	setStartingFen(pgn.startingFenString());
	resetBoard();
	m_moves.clear();

	foreach (const PgnGame::MoveData& md, pgn.moves())
	{
		Chess::Move move(m_board->moveFromGenericMove(md.move));
		Q_ASSERT(m_board->isLegalMove(move));

		m_board->makeMove(move);
		if (!m_board->result().isNone())
			return true;

		m_moves.append(move);
	}

	return true;
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

void ChessGame::setAdjudicator(const GameAdjudicator& adjudicator)
{
	m_adjudicator = adjudicator;
}

void ChessGame::generateOpening()
{
	if (m_book[Chess::Side::White] == nullptr || m_book[Chess::Side::Black] == nullptr)
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
		if (move.isNull())
			break;

		m_board->makeMove(move);
		if (!m_board->result().isNone())
			break;

		m_moves.append(move);
	}
}

void ChessGame::emitStartFailed()
{
	emit startFailed(this);
}

void ChessGame::setStartDelay(int time)
{
	Q_ASSERT(time >= 0);
	m_startDelay = time;
}

void ChessGame::pauseThread()
{
	m_pauseSem.release();
	m_resumeSem.acquire();
}

void ChessGame::lockThread()
{
	if (QThread::currentThread() == thread())
		return;

	QMetaObject::invokeMethod(this, "pauseThread", Qt::QueuedConnection);
	m_pauseSem.acquire();
}

void ChessGame::unlockThread()
{
	if (QThread::currentThread() == thread())
		return;

	m_resumeSem.release();
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
	else if (!m_startingFen.isEmpty())
		m_startingFen = m_board->fenString();
}

void ChessGame::onPlayerReady()
{
	ChessPlayer* sender = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != nullptr);

	disconnect(sender, SIGNAL(ready()),
		   this, SLOT(onPlayerReady()));
	disconnect(sender, SIGNAL(disconnected()),
		   this, SLOT(onPlayerReady()));

	for (int i = 0; i < 2; i++)
	{
		if (!m_player[i]->isReady()
		&&  m_player[i]->state() != ChessPlayer::Disconnected)
			return;
	}

	emit playersReady();
}

void ChessGame::syncPlayers()
{
	bool ready = true;

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player = m_player[i];
		Q_ASSERT(player != nullptr);

		if (!player->isReady()
		&&  player->state() != ChessPlayer::Disconnected)
		{
			ready = false;
			connect(player, SIGNAL(ready()),
				this, SLOT(onPlayerReady()));
			connect(player, SIGNAL(disconnected()),
				this, SLOT(onPlayerReady()));
		}
	}
	if (ready)
		emit playersReady();
}

void ChessGame::start()
{
	if (m_startDelay > 0)
	{
		QTimer::singleShot(m_startDelay, this, SLOT(start()));
		m_startDelay = 0;
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		connect(m_player[i], SIGNAL(resultClaim(Chess::Result)),
			this, SLOT(onResultClaim(Chess::Result)));
	}

	// Start the game in the correct thread
	connect(this, SIGNAL(playersReady()), this, SLOT(startGame()));
	QMetaObject::invokeMethod(this, "syncPlayers", Qt::QueuedConnection);
}

void ChessGame::pause()
{
	m_paused = true;
}

void ChessGame::resume()
{
	if (!m_paused)
		return;
	m_paused = false;

	QMetaObject::invokeMethod(this, "startTurn", Qt::QueuedConnection);
}

void ChessGame::initializePgn()
{
	if (m_pgnInitialized)
		return;
	m_pgnInitialized = true;

	m_pgn->setVariant(m_board->variant());
	m_pgn->setStartingFenString(m_board->startingSide(), m_startingFen);
	m_pgn->setDate(QDate::currentDate());
	m_pgn->setPlayerName(Chess::Side::White, m_player[Chess::Side::White]->name());
	m_pgn->setPlayerName(Chess::Side::Black, m_player[Chess::Side::Black]->name());
	m_pgn->setResult(m_result);

	if (m_timeControl[Chess::Side::White] == m_timeControl[Chess::Side::Black])
		m_pgn->setTag("TimeControl", m_timeControl[0].toString());
	else
	{
		m_pgn->setTag("WhiteTimeControl", m_timeControl[Chess::Side::White].toString());
		m_pgn->setTag("BlackTimeControl", m_timeControl[Chess::Side::Black].toString());
	}
}

void ChessGame::startGame()
{
	m_result = Chess::Result();
	emit humanEnabled(false);

	disconnect(this, SIGNAL(playersReady()), this, SLOT(startGame()));
	if (m_finished)
		return;

	m_gameInProgress = true;
	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player = m_player[i];
		Q_ASSERT(player != nullptr);
		Q_ASSERT(player->isReady());

		if (player->state() == ChessPlayer::Disconnected)
			return;
		if (!player->supportsVariant(m_board->variant()))
		{
			qDebug("%s doesn't support variant %s",
				qPrintable(player->name()), qPrintable(m_board->variant()));
			m_result = Chess::Result(Chess::Result::ResultError);
			stop();
			return;
		}
	}

	resetBoard();
	initializePgn();
	emit started(this);
	emit fenChanged(m_board->startingFenString());

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
		m_board->makeMove(move);
		
		emitLastMove();

		if (!m_board->result().isNone())
		{
			qDebug("Every move was played from the book");
			m_result = m_board->result();
			stop();
			return;
		}
	}
	
	for (int i = 0; i < 2; i++)
	{
		connect(m_player[i], SIGNAL(moveMade(Chess::Move)),
			this, SLOT(onMoveMade(Chess::Move)));
		if (m_player[i]->isHuman())
			connect(m_player[i], SIGNAL(wokeUp()),
				this, SLOT(resume()));
	}
	
	startTurn();
}

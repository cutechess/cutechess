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

#include "xboardengine.h"

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include "timecontrol.h"
#include "enginespinoption.h"
#include "enginetextoption.h"


static QString msToXboardTime(int ms)
{
	int sec = ms / 1000;

	QString number = QString::number(sec / 60);
	if (sec % 60 != 0)
		number += QString(":%1").arg(sec % 60, 2, 10, QChar('0'));
	
	return number;
}


XboardEngine::XboardEngine(QIODevice* ioDevice, QObject* parent)
	: ChessEngine(ioDevice, parent),
	  m_forceMode(false),
	  m_drawOnNextMove(false),
	  m_ftName(false),
	  m_ftPing(false),
	  m_ftSetboard(false),
	  m_ftTime(true),
	  m_ftUsermove(false),
	  m_gotResult(false),
	  m_lastPing(0),
	  m_notation(Chess::Board::LongAlgebraic),
	  m_initTimer(new QTimer(this))
{
	m_initTimer->setSingleShot(true);
	m_initTimer->setInterval(2000);
	connect(m_initTimer, SIGNAL(timeout()), this, SLOT(initialize()));

	m_variants.append("standard");
	setName("XboardEngine");
}

void XboardEngine::startProtocol()
{
	// Tell the engine to turn on xboard mode
	write("xboard");
	// Tell the engine that we're using Xboard protocol 2
	write("protover 2");

	// Give the engine 2 seconds to reply to the protover command.
	// This is how Xboard deals with protocol 1 engines.
	m_initTimer->start();
}

void XboardEngine::initialize()
{
	if (state() == Starting)
	{
		onProtocolStart();
		emit ready();
	}
}

static QString variantFromXboard(const QString& str)
{
	if (str == "normal")
		return "standard";

	return str;
}

static QString variantToXboard(const QString& str)
{
	if (str == "standard")
		return "normal";

	return str;
}

void XboardEngine::startGame()
{
	m_drawOnNextMove = false;
	m_gotResult = false;
	m_forceMode = false;
	m_nextMove = Chess::Move();
	write("new");
	
	if (board()->variant() != "standard")
		write("variant " + variantToXboard(board()->variant()));
	
	if (board()->isRandomVariant()
	||  board()->fenString() != board()->defaultFenString())
	{
		if (m_ftSetboard)
			write("setboard " + board()->fenString());
		else
			qDebug() << name() << "doesn't support the setboard command.";
	}
	
	// Send the time controls
	const TimeControl* myTc = timeControl();
	if (myTc->timePerMove() > 0)
		write(QString("st %1").arg(myTc->timePerMove() / 1000));
	else
		write(QString("level %1 %2 %3")
		      .arg(myTc->movesPerTc())
		      .arg(msToXboardTime(myTc->timePerTc()))
		      .arg(myTc->timeIncrement() / 1000));

	if (myTc->maxDepth() > 0)
		write(QString("sd %1").arg(myTc->maxDepth()));

	// Show thinking
	write("post");
	// Disable pondering
	write("easy");
	setForceMode(true);
	
	// Tell the opponent's type and name to the engine
	if (m_ftName)
	{
		if (!opponent()->isHuman())
			write("computer");
		write("name " + opponent()->name());
	}
}

void XboardEngine::endGame(const Chess::Result& result)
{
	State s = state();
	if (s != Thinking && s != Observing)
		return;

	if (s != Thinking)
		m_gotResult = true;

	stopThinking();
	setForceMode(true);
	write("result " + result.toVerboseString());

	ChessEngine::endGame(result);

	// If the engine can't be pinged, we may have to wait for
	// for a move or a result, or an error, or whatever. We
	// would like to extend our middle fingers to every engine
	// developer who fails to support the ping command.
	if (!m_ftPing && m_gotResult)
		finishGame();
}

void XboardEngine::finishGame()
{
	if (!m_ftPing && state() == FinishingGame)
	{
		// Give the engine enough time to send all pending
		// output relating to the current game
		m_gotResult = true;
		QTimer::singleShot(200, this, SLOT(pong()));
	}
}

void XboardEngine::sendTimeLeft()
{
	if (!m_ftTime)
		return;
	
	int csLeft = timeControl()->timeLeft() / 10;
	int ocsLeft = opponent()->timeControl()->timeLeft() / 10;

	if (csLeft < 0)
		csLeft = 0;
	if (ocsLeft < 0)
		ocsLeft = 0;

	write(QString("time %1\notim %2").arg(csLeft).arg(ocsLeft));
}

void XboardEngine::setForceMode(bool enable)
{
	if (enable && !m_forceMode)
	{
		m_forceMode = true;
		write("force");

		// If there's a move pending, and we didn't get the
		// 'go' command, we'll send the move in force mode.
		if (!m_nextMove.isNull())
			makeMove(m_nextMove);
	}
	m_forceMode = enable;
}

QString XboardEngine::moveString(const Chess::Move& move)
{
	Q_ASSERT(!move.isNull());

	// Xboard always uses SAN for castling moves in random variants
	if (m_notation == Chess::Board::LongAlgebraic && board()->isRandomVariant())
	{
		QString str(board()->moveString(move, Chess::Board::StandardAlgebraic));
		if (str.startsWith("O-O"))
			return str;
	}

	return board()->moveString(move, m_notation);
}

void XboardEngine::makeMove(const Chess::Move& move)
{
	Q_ASSERT(!move.isNull());

	QString moveString;
	if (move == m_nextMove)
		moveString = m_nextMoveString;
	else
		moveString = this->moveString(move);

	// If we're not in force mode, we'll have to wait for the
	// 'go' command until the move can be sent to the engine.
	if (!m_forceMode)
	{
		if (m_nextMove.isNull())
		{
			m_nextMove = move;
			m_nextMoveString = moveString;
			return;
		}
		else if (move != m_nextMove)
			setForceMode(true);
	}

	if (m_ftUsermove)
		write("usermove " + moveString);
	else
		write(moveString);

	m_nextMove = Chess::Move();
}

void XboardEngine::startThinking()
{
	setForceMode(false);
	sendTimeLeft();

	if (m_nextMove.isNull())
		write("go");
	else
		makeMove(m_nextMove);
}

void XboardEngine::stopThinking()
{
	if (state() == Thinking)
		write("?");
}

ChessEngine::Protocol XboardEngine::protocol() const
{
	return ChessEngine::Xboard;
}

bool XboardEngine::sendPing()
{
	if (!m_ftPing)
	{
		if (state() == FinishingGame)
			return true;
		return false;
	}

	// Ping the engine with a random number. The engine should
	// later send the number back at us.
	m_lastPing = (qrand() % 32) + 1;
	write(QString("ping %1").arg(m_lastPing));
	return true;
}

void XboardEngine::sendQuit()
{
	write("quit");
}

void XboardEngine::setFeature(const QString& name, const QString& val)
{
	if (name == "ping")
		m_ftPing = (val == "1");
	else if (name == "setboard")
		m_ftSetboard = (val == "1");
	else if (name == "san")
	{
		if (val == "1")
			m_notation = Chess::Board::StandardAlgebraic;
		else
			m_notation = Chess::Board::LongAlgebraic;
	}
	else if (name == "usermove")
		m_ftUsermove = (val == "1");
	else if (name == "time")
		m_ftTime = (val == "1");
	else if (name == "myname")
	{
		if (this->name() == "XboardEngine")
			setName(val);
	}
	else if (name == "variants")
	{
		m_variants.clear();
		QStringList variants = val.split(',');
		foreach (const QString& str, variants)
		{
			QString variant = variantFromXboard(str.trimmed());
			if (!variant.isEmpty())
				m_variants.append(variant);
		}
	}
	else if (name == "name")
		m_ftName = (val == "1");
	else if (name == "memory")
	{
		if (val == "1")
			m_options.append(new EngineSpinOption("memory"));
	}
	else if (name == "smp")
	{
		if (val == "1")
			m_options.append(new EngineSpinOption("cores"));
	}
	else if (name == "egt")
	{
		QStringList list = val.split(',');
		foreach (const QString& str, list)
		{
			QString egtType = QString("egtpath %1").arg(str.trimmed());
			m_options.append(new EngineTextOption(egtType));
		}
	}
	else if (name == "done")
	{
		write("accepted done", Unbuffered);
		m_initTimer->stop();
		
		if (val == "1")
			initialize();
		return;
	}
	else
	{
		write("rejected " + name, Unbuffered);
		return;
	}
	
	write("accepted " + name, Unbuffered);
}

void XboardEngine::parseLine(const QString& line)
{
	const QString command = line.section(' ', 0, 0);
	if (command.isEmpty())
		return;

	const QString args = line.right(line.length() - command.length() - 1);

	if (command == "move")
	{
		if (state() != Thinking)
		{
			if (state() == FinishingGame)
				finishGame();
			else
				qDebug() << "Unexpected move from" << name();
			return;
		}

		Chess::Move move = board()->moveFromString(args);
		if (move.isNull())
		{
			emitForfeit(Chess::Result::IllegalMove, args);
			return;
		}

		if (m_drawOnNextMove)
		{
			m_drawOnNextMove = false;
			Chess::Result boardResult;
			board()->makeMove(move);
			boardResult = board()->result();
			board()->undoMove();

			// If the engine claimed a draw before this move, the
			// game must have ended in a draw by now
			if (!boardResult.isDraw())
			{
				qDebug("%s forfeits by invalid draw claim",
				       qPrintable(name()));
				emitForfeit(Chess::Result::Adjudication);
				return;
			}
		}

		emitMove(move);
	}
	else if (command == "pong")
	{
		if (args.toInt() == m_lastPing)
			pong();
	}
	else if (command == "1-0" || command == "0-1"
	     ||  command == "1/2-1/2" || command == "resign")
	{
		if ((state() != Thinking && state() != Observing)
		||  !board()->result().isNone())
		{
			finishGame();
			return;
		}

		if (command == "1/2-1/2")
		{
			if (state() == Thinking)
				// The engine claims that its next move will draw the game
				m_drawOnNextMove = true;
			else
			{
				qDebug("%s forfeits by invalid draw claim",
				       qPrintable(name()));
				emitForfeit(Chess::Result::Adjudication);
			}
			return;
		}
		
		if ((command == "1-0" && side() == Chess::White)
		||  (command == "0-1" && side() == Chess::Black))
		{
			qDebug("%s forfeits by invalid victory claim",
			       qPrintable(name()));
			emitForfeit(Chess::Result::Adjudication);
		}
		else
			emitForfeit(Chess::Result::Resignation);
	}
	else if (command == "feature")
	{
		QRegExp rx("\\w+\\s*=\\s*(\"[^\"]*\"|\\d+)");
		
		int pos = 0;
		QString feature;
		QStringList list;
		
		while ((pos = rx.indexIn(args, pos)) != -1)
		{
			list = rx.cap().split('=');
			if (list.count() != 2)
				continue;
			feature = list[0].trimmed();
			
			QString val = list[1].trimmed();
			val.remove('\"');
			
			setFeature(feature, val);
			pos += rx.matchedLength();
		}
	}
	else if (command == "Error")
	{
		// If the engine complains about an unknown result command,
		// we can assume that it's safe to finish the game.
		QString str = args.section(':', 1).trimmed();
		if (str.startsWith("result"))
			finishGame();
	}
	else if (command.at(0).isDigit()) // principal variation
	{
		bool ok = false;

		QString depthStr;
		foreach (const QChar& c, command)
		{
			if (c.isDigit())
				depthStr += c;
			else
				break;
		}
		m_eval.setDepth(depthStr.toInt());

		int eval = args.section(' ', 0, 0).toInt(&ok);
		if (ok)
		{
			if (m_whiteEvalPov && side() == Chess::Black)
				eval = -eval;
			m_eval.setScore(eval);
		}

		int ms = args.section(' ', 1, 1).toInt(&ok);
		if (ok)
			m_eval.setTime(ms * 10);

		int nodes = args.section(' ', 2, 2).toInt(&ok);
		if (ok)
			m_eval.setNodeCount(nodes);

		m_eval.setPv(args.section(' ', 3));
	}
}

void XboardEngine::sendOption(const QString& name, const QString& value)
{
	write(name + " " + value);
}

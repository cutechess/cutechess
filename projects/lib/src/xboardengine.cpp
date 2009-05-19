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

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "xboardengine.h"
#include "chessboard/chessboard.h"
#include "chessboard/chessmove.h"
#include "timecontrol.h"


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
	  m_forceMode(true),
	  m_drawOnNextMove(false),
	  m_ftEgbb(false),
	  m_ftEgtb(false),
	  m_ftSmp(false),
	  m_ftMemory(false),
	  m_ftName(false),
	  m_ftPing(false),
	  m_ftSetboard(false),
	  m_ftTime(true),
	  m_ftUsermove(false),
	  m_gotResult(false),
	  m_waitForMove(false),
	  m_lastPing(0)
{
	m_variants.append(Chess::Variant::Standard);
	setName("XboardEngine");
}

void XboardEngine::start()
{
	m_isReady = true;
	// Tell the engine to turn on xboard mode
	write("xboard");
	// Tell the engine that we're using Xboard protocol 2
	write("protover 2");
	m_isReady = false;

	// Give the engine 2 seconds to reply to the protover command.
	// This is how Xboard deals with protocol 1 engines.
	m_initTimer.setSingleShot(true);
	connect(&m_initTimer, SIGNAL(timeout()), this, SLOT(initialize()));
	m_initTimer.start(2000);
}

void XboardEngine::initialize()
{
	if (!m_initialized)
	{
		Q_ASSERT(!m_isReady);
		m_initialized = true;
		m_isReady = true;
		
		flushWriteBuffer();
		
		// Send engine options
		foreach(const OptionCmd& cmd, m_optionBuffer)
		{
			if (*cmd.feature)
				write(cmd.line);
		}
		m_optionBuffer.clear();
		
		emit ready();
	}
}

static Chess::Variant variantCode(const QString& str)
{
	if (str == "normal")
		return Chess::Variant::Standard;
	else if (str == "fischerandom")
		return Chess::Variant::Fischerandom;
	else if (str == "capablanca")
		return Chess::Variant::Capablanca;
	else if (str == "gothic")
		return Chess::Variant::Gothic;
	else if (str == "caparandom")
		return Chess::Variant::Caparandom;
	
	return Chess::Variant::NoVariant;
}

static QString variantString(Chess::Variant variant)
{
	switch (variant.code())
	{
	case Chess::Variant::Standard:
		return "normal";
	case Chess::Variant::Fischerandom:
		return "fischerandom";
	case Chess::Variant::Capablanca:
		return "capablanca";
	case Chess::Variant::Gothic:
		return "gothic";
	case Chess::Variant::Caparandom:
		return "caparandom";
	default:
		return "unknown";
	}
}

void XboardEngine::startGame()
{
	m_drawOnNextMove = false;
	m_gotResult = false;
	m_waitForMove = false;
	write("new");
	
	const Chess::Variant& variant = m_chessboard->variant();
	
	if (variant != Chess::Variant::Standard)
		write("variant " + variantString(variant));
	
	if (variant.isRandom() || m_chessboard->fenString() != variant.startingFen())
	{
		if (m_ftSetboard)
			write("setboard " + m_chessboard->fenString());
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
	setObserverMode(true);
	
	// Tell the opponent's type and name to the engine
	if (m_ftName)
	{
		if (!this->opponent()->isHuman())
			write("computer");
		write("name " + this->opponent()->name());
	}
}

void XboardEngine::endGame(Chess::Result result)
{
	if (m_gameInProgress)
	{
		if (!m_waitForMove)
			m_gotResult = true;

		stopThinking();
		write("result " + result.toString());
		setObserverMode(true);
	}
	ChessEngine::endGame(result);

	// If the engine can't be pinged, we may have to wait for
	// for a move or a result, or an error, or whatever. We
	// would like to extend our middle fingers to every engine
	// developer who fails to support the ping command.
	if (!m_ftPing)
	{
		m_isReady = false;
		if (m_gotResult)
			finishGame();
	}
}

void XboardEngine::finishGame()
{
	if (!m_ftPing && m_finishingGame)
	{
		m_finishingGame = false;
		m_gotResult = true;
		QTimer::singleShot(100, this, SLOT(pong()));
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

bool XboardEngine::inObserverMode() const
{
	return m_forceMode;
}

void XboardEngine::setObserverMode(bool enabled)
{
	if (enabled)
		write("force");
	m_forceMode = enabled;
}

void XboardEngine::makeMove(const Chess::Move& move)
{
	if (!m_forceMode)
	{
		m_waitForMove = true;
		if (m_ftPing && m_isReady)
			ping(PingMove);
		else
			startClock();
		sendTimeLeft();
	}
	
	QString moveString = m_chessboard->moveString(move, m_notation);
	if (m_ftUsermove)
		write("usermove " + moveString);
	else
		write(moveString);
}

void XboardEngine::go()
{
	if (!m_forceMode)
		return;
	
	m_waitForMove = true;
	setObserverMode(false);
	if (m_ftPing && m_isReady)
		ping(PingMove);
	else
		startClock();
	sendTimeLeft();
	write("go");
}

void XboardEngine::stopThinking()
{
	if (m_waitForMove)
		write("?");
}

ChessEngine::Protocol XboardEngine::protocol() const
{
	return ChessEngine::Xboard;
}

void XboardEngine::ping(ChessEngine::PingType type)
{
	if (m_ftPing && m_isReady)
	{
		// Ping the engine with a random number. The engine should
		// later send the number back at us.
		m_pingType = type;
		m_lastPing = (qrand() % 32) + 1;
		write(QString("ping %1").arg(m_lastPing));
		m_isReady = false;
		ChessEngine::ping(type);
	}
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
			m_notation = Chess::StandardAlgebraic;
		else
			m_notation = Chess::LongAlgebraic;
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
			Chess::Variant v = variantCode(str.trimmed());
			if (!v.isNone())
				m_variants.append(v);
		}
	}
	else if (name == "name")
		m_ftName = (val == "1");
	else if (name == "memory")
		m_ftMemory = (val == "1");
	else if (name == "smp")
		m_ftSmp = (val == "1");
	else if (name == "egt")
	{
		QStringList list = val.split(',');
		foreach (const QString& str, list)
		{
			QString egtType = str.trimmed();
			if (egtType == "scorpio")
				m_ftEgbb = true;
			else if (egtType == "nalimov")
				m_ftEgtb = true;
		}
	}
	else if (name == "done")
	{
		write("accepted done");
		m_initTimer.stop();
		
		if (val == "1")
			initialize();
		return;
	}
	else
	{
		write("rejected " + name);
		return;
	}
	
	write("accepted " + name);
}

void XboardEngine::parseLine(const QString& line)
{
	QString command = line.section(' ', 0, 0);
	QString args = line.right(line.length() - command.length() - 1);

	if (command == "move")
	{
		m_waitForMove = false;
		if (!m_gameInProgress || m_forceMode)
		{
			finishGame();
			return;
		}

		Chess::Move move = m_chessboard->moveFromString(args);
		if (!m_chessboard->isLegalMove(move))
		{
			m_timer.stop();
			emitForfeit(Chess::Result::WinByIllegalMove, args);
			return;
		}

		if (m_drawOnNextMove)
		{
			m_drawOnNextMove = false;
			Chess::Result boardResult;
			m_chessboard->makeMove(move);
			boardResult = m_chessboard->result();
			m_chessboard->undoMove();

			// If the engine claimed a draw before this move, the
			// game must have ended in a draw by now
			if (!boardResult.isDraw())
			{
				qDebug("%s forfeits by invalid draw claim",
				       qPrintable(name()));
				emitForfeit(Chess::Result::WinByAdjudication);
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
		if (!m_gameInProgress || !m_chessboard->result().isNone())
		{
			finishGame();
			return;
		}

		if (command == "1/2-1/2")
		{
			if (m_waitForMove)
				// The engine claims that its next move will draw the game
				m_drawOnNextMove = true;
			else
			{
				qDebug("%s forfeits by invalid draw claim",
				       qPrintable(name()));
				emitForfeit(Chess::Result::WinByAdjudication);
			}
			return;
		}
		
		Chess::Result::Code resultCode;
		if ((command == "1-0" && side() == Chess::White)
		||  (command == "0-1" && side() == Chess::Black))
		{
			qDebug("%s forfeits by invalid victory claim",
			       qPrintable(name()));
			resultCode = Chess::Result::WinByAdjudication;
		}
		else
			// resign
			resultCode = Chess::Result::WinByResignation;

		emitForfeit(resultCode);
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
		QString str = args.section(':', 1).trimmed();

		if (!m_gameInProgress && str.startsWith("result"))
			finishGame();
	}
	else if (command.toInt() > 0)
	{
		bool ok = false;
		m_eval.setDepth(command.toInt());

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

		if (!m_eval.isEmpty())
			emit sendEvaluation(m_eval);
	}
}

void XboardEngine::setOption(const QString& line, bool* feature)
{
	if (!m_initialized)
	{
		OptionCmd cmd = { line, feature };
		m_optionBuffer.append(cmd);
		return;
	}

	if (*feature)
		write(line);
}

void XboardEngine::setConcurrency(int limit)
{
	setOption(QString("cores %1").arg(limit), &m_ftSmp);
}

void XboardEngine::setEgbbPath(const QString& path)
{
	setOption(QString("egtpath scorpio %1").arg(path), &m_ftEgbb);
}

void XboardEngine::setEgtbPath(const QString& path)
{
	setOption(QString("egtpath nalimov %1").arg(path), &m_ftEgtb);
}

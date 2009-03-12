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
		number += QString(":") + QString::number(sec % 60);
	
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
	  m_lastPing(0)
{
	m_variants.append(Chess::Variant::Standard);
	
	setName("XboardEngine");
	// Tell the engine to turn on xboard mode
	write("xboard");
	// Tell the engine that we're using Xboard protocol 2
	write("protover 2");
	
	// Give the engine 2 seconds to reply to the protover command.
	// This is how Xboard deals with protocol 1 engines.
	m_isReady = false;
	m_initTimer.setSingleShot(true);
	connect(&m_initTimer, SIGNAL(timeout()), this, SLOT(initialize()));
	m_initTimer.start(2000);
}

XboardEngine::~XboardEngine()
{
	//write("quit");
}

void XboardEngine::initialize()
{
	if (!m_initialized)
	{
		Q_ASSERT(!m_isReady);
		m_initialized = true;
		m_isReady = true;
		
		flushWriteBuffer();
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

void XboardEngine::newGame(Chess::Side side, ChessPlayer* opponent)
{
	ChessPlayer::newGame(side, opponent);
	m_forceMode = true;
	m_drawOnNextMove = false;
	write("new");
	
	const Chess::Variant& variant = m_chessboard->variant();
	
	if (variant != Chess::Variant::Standard)
		write("variant " + variantString(variant));
	
	if (variant.isRandom() || m_chessboard->fenString() != variant.startingFen())
	{
		if (m_ftSetboard)
			write("setboard " + m_chessboard->fenString());
		else
			qDebug() << m_name << "doesn't support the setboard command.";
	}
	
	// Send the time controls
	if (m_timeControl.timePerMove() > 0)
		write(QString("st ") + QString::number(m_timeControl.timePerMove() / 1000));
	else
	{
		QString command = "level";
		command += QString(" ") + QString::number(m_timeControl.movesPerTc());
		command += QString(" ") + msToXboardTime(m_timeControl.timePerTc());
		command += QString(" ") + QString::number(m_timeControl.timeIncrement() / 1000);
		write(command);
	}

	// Show thinking
	write("post");
	// Disable pondering
	write("easy");
	// Put the engine in observer mode
	write("force");
	
	// Tell the opponent's type and name to the engine
	if (m_ftName)
	{
		if (!m_opponent->isHuman())
			write("computer");
		write(QString("name ") + m_opponent->name());
	}
}

void XboardEngine::endGame(Chess::Result result)
{
	ChessPlayer::endGame(result);
	write("result " + result.toString());
}

void XboardEngine::sendTimeLeft()
{
	if (!m_ftTime)
		return;
	
	int csLeft = m_timeControl.timeLeft() / 10;
	int ocsLeft = m_opponent->timeControl()->timeLeft() / 10;

	if (csLeft < 0)
		csLeft = 0;
	if (ocsLeft < 0)
		ocsLeft = 0;

	write(QString("time ") + QString::number(csLeft) +
	      QString("\notim ") + QString::number(ocsLeft));
}

void XboardEngine::makeMove(const Chess::Move& move)
{
	if (!m_forceMode)
	{
		if (m_ftPing && m_isReady)
			ping(PingMove);
		else
			startClock();
		sendTimeLeft();
	}
	
	QString moveString = m_chessboard->moveString(move, m_notation);
	if (m_ftUsermove)
		write(QString("usermove ") + moveString);
	else
		write(moveString);
}

void XboardEngine::go()
{
	if (!m_forceMode)
		return;
	
	m_forceMode = false;
	if (m_ftPing && m_isReady)
		ping(PingMove);
	else
		startClock();
	sendTimeLeft();
	write("go");
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
		write(QString("ping ") + QString::number(m_lastPing));
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
		m_name = val;
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
		write(QString("rejected ") + name);
		return;
	}
	
	write(QString("accepted ") + name);
}

void XboardEngine::parseLine(const QString& line)
{
	QString command = line.section(' ', 0, 0);
	QString args = line.right(line.length() - command.length() - 1);

	if (command == "move")
	{
		if (!m_gameInProgress)
			return;

		Chess::Move move = m_chessboard->moveFromString(args);
		if (!m_chessboard->isLegalMove(move))
		{
			m_timer.stop();
			Chess::Result result(Chess::Result::WinByIllegalMove, otherSide(), args);
			emit forfeit(result);
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
				Chess::Result result(Chess::Result::WinByAdjudication, otherSide());
				emit forfeit(result);
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
			return;
		if (command == "1/2-1/2")
		{
			// The engine claims that its next move will draw the game
			m_drawOnNextMove = true;
			return;
		}
		
		Chess::Result result;
		if ((command == "1-0" && side() == Chess::White)
		||  (command == "0-1" && side() == Chess::Black))
		{
			qDebug("%s forfeits by invalid victory claim",
			       qPrintable(name()));
			result = Chess::Result(Chess::Result::WinByAdjudication, otherSide());
		}
		else
		{
			qDebug("%s resigns", qPrintable(name()));
			result = Chess::Result(Chess::Result::WinByResignation, otherSide());
		}
		emit forfeit(result);
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
	else if (command.toInt() > 0)
	{
		bool ok = false;
		m_eval.setDepth(command.toInt());

		int eval = args.section(' ', 0, 0).toInt(&ok);
		if (ok)
			m_eval.setScore(eval);

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

void XboardEngine::setConcurrency(int limit)
{
	if (m_ftSmp)
		write(QString("cores ") + QString::number(limit));
}

void XboardEngine::setEgbbPath(const QString& path)
{
	if (m_ftEgbb)
		write(QString("egtpath scorpio ") + path);
}

void XboardEngine::setEgtbPath(const QString& path)
{
	if (m_ftEgtb)
		write(QString("egtpath nalimov ") + path);
}

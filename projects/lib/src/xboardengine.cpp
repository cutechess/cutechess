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


XboardEngine::XboardEngine(QIODevice* ioDevice,
                         Chess::Board* chessboard,
                         QObject* parent)
	: ChessEngine(ioDevice, chessboard, parent),
	  m_forceMode(true),
	  m_drawOnNextMove(false),
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
	write("force");
	
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

	if (csLeft > 0)
		write(QString("time ") + QString::number(csLeft));
	if (ocsLeft > 0)
		write(QString("otim ") + QString::number(ocsLeft));
}

void XboardEngine::makeMove(const Chess::Move& move)
{
	QString moveString = m_chessboard->moveString(move, m_notation);
	
	if (!m_forceMode)
		sendTimeLeft();
	
	if (m_ftUsermove)
		write(QString("usermove ") + moveString);
	else
		write(moveString);
	
	if (!m_forceMode)
		ChessPlayer::go();
}

void XboardEngine::go()
{
	if (!m_forceMode)
		return;
	
	m_forceMode = false;
	sendTimeLeft();
	write("go");
	ChessPlayer::go();
}

ChessEngine::Protocol XboardEngine::protocol() const
{
	return ChessEngine::Xboard;
}

void XboardEngine::ping()
{
	if (m_ftPing && m_isReady)
	{
		// Ping the engine with a random number. The engine should
		// later send the number back at us.
		m_lastPing = qrand() % 32;
		write(QString("ping ") + QString::number(m_lastPing));
		m_isReady = false;
	}
}

void XboardEngine::setFeature(const QString& name, const QString& val)
{
	if (name == "ping")
	{
		if (val == "1")
			m_ftPing = true;
	}
	else if (name == "setboard")
	{
		if (val == "1")
			m_ftSetboard = true;
	}
	else if (name == "san")
	{
		if (val == "1")
			m_notation = Chess::StandardAlgebraic;
	}
	else if (name == "usermove")
	{
		if (val == "1")
			m_ftUsermove = true;
	}
	else if (name == "time")
	{
		if (val == "0")
			m_ftTime = false;
	}
	else if (name == "myname")
	{
		m_name = val;
	}
	else if (name == "variants")
	{
		m_variants.clear();
		QStringList variants = val.split(',');
		foreach (const QString& str, variants)
		{
			Chess::Variant v = variantCode(str);
			if (!v.isNone())
				m_variants.append(v);
		}
	}
	else if (name == "name")
	{
		if (val == "1")
			m_ftName = true;
	}
	else if (name == "memory")
	{
		if (val == "1")
			m_ftMemory = true;
	}
	else if (name == "smp")
	{
		if (val == "1")
			m_ftSmp = true;
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
		Chess::Move move = m_chessboard->moveFromString(args);
		emitMove(move);
		
		if (m_drawOnNextMove)
		{
			m_drawOnNextMove = false;
			
			// If the engine claimed a draw before this move, the
			// game must have ended in a draw by now
			if (!m_chessboard->result().isDraw())
			{
				qDebug("%s forfeits by invalid draw claim",
				       qPrintable(name()));
				emit resign();
			}
		}
	}
	else if (command == "pong")
	{
		int pong = args.toInt();
		if (!m_isReady && pong == m_lastPing)
		{
			m_isReady = true;
			flushWriteBuffer();
			emit ready();
		}
	}
	else if (command == "1-0" || command == "0-1" || command == "1/2-1/2")
	{
		if (!m_chessboard->result().isNone())
			return;
		if (command == "1/2-1/2")
		{
			// The engine claims that its next move will draw the game
			m_drawOnNextMove = true;
			return;
		}
		
		if ((command == "1-0" && side() == Chess::White)
		||  (command == "0-1" && side() == Chess::Black))
			qDebug("%s forfeits by invalid victory claim",
			       qPrintable(name()));
		else
			qDebug("%s resigns", qPrintable(name()));
		emit resign();
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
}

void XboardEngine::setConcurrency(int limit)
{
	if (m_ftSmp)
		write(QString("cores ") + QString::number(limit));
}

void XboardEngine::setMemory(int limit)
{
	if (m_ftMemory)
		write(QString("memory ") + QString::number(limit));
}

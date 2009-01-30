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
	  m_lastPing(0)
{
	setName("XboardEngine");
	// Tell the engine to turn on xboard mode
	write("xboard");
	// Tell the engine that we're using Xboard protocol 2
	write("protover 2");
	
	// Wait for the engine to initialize
	m_isReady = false;
}

XboardEngine::~XboardEngine()
{
	//write("quit");
}

void XboardEngine::newGame(Chess::Side side, ChessPlayer* opponent)
{
	ChessPlayer::newGame(side, opponent);
	m_forceMode = true;
	m_drawOnNextMove = false;
	write("new");
	write("force");
	
	QString variant;
	switch (m_chessboard->variant())
	{
	case Chess::StandardChess:
		if (m_chessboard->isRandomVariant())
			variant = "fischerandom";
		else
			variant = "normal";
		break;
	case Chess::CapablancaChess:
		if (m_chessboard->isRandomVariant())
		{
			if (m_chessboard->fenString() == Chess::gothicFen)
				variant = "gothic";
			else
				variant = "caparandom";
		}
		else
			variant = "capablanca";
		break;
	default:
		break;
	}
	
	if (variant != "normal")
		write("variant " + variant);
	write("setboard " + m_chessboard->fenString());
	
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
}

void XboardEngine::sendTimeLeft()
{
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
	write(moveString);
}

void XboardEngine::go()
{
	if (m_forceMode)
	{
		m_forceMode = false;
		sendTimeLeft();
		write("go");
	}

	ChessPlayer::go();
}

ChessEngine::Protocol XboardEngine::protocol() const
{
	return ChessEngine::Xboard;
}

void XboardEngine::ping()
{
	if (m_isReady)
	{
		// Ping the engine with a random number. The engine should
		// later send the number back at us.
		m_lastPing = qrand() % 32;
		write(QString("ping ") + QString::number(m_lastPing));
		m_isReady = false;
	}
}

void XboardEngine::parseLine(const QString& line)
{
	QString command = line.section(' ', 0, 0);
	QString args = line.right(line.length() - command.length() - 1);

	if (command == "move")
	{
		m_timeControl.update();
		Chess::Move move = m_chessboard->moveFromString(args);
		emit moveMade(move);
		
		if (m_drawOnNextMove)
		{
			m_drawOnNextMove = false;
			Chess::Result result = m_chessboard->result();
			
			// If the engine claimed a draw before this move, the
			// game must have ended in a draw by now
			if (result != Chess::DrawByMaterial
			&&  result != Chess::DrawByRepetition
			&&  result != Chess::DrawByFiftyMoves)
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
		}
	}
	else if (command == "1-0" || command == "0-1" || command == "1/2-1/2")
	{
		if (m_chessboard->result() != Chess::NoResult)
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
		QString arg;
		QString feature;
		QStringList list;
		
		while ((pos = rx.indexIn(args, pos)) != -1)
		{
			arg = args.mid(pos, rx.matchedLength());
			pos += rx.matchedLength();
			
			list = arg.split('=');
			if (list.count() != 2)
				continue;
			feature = list[0].trimmed();
			
			QString val = list[1].trimmed();
			val.remove('\"');
			
			if (feature == "san")
			{
				if (val == "1")
					m_notation = Chess::StandardAlgebraic;
			}
			else if (feature == "myname")
			{
				m_name = val;
			}
			else if (feature == "variants")
			{
				QStringList variants = val.split(',');
				QString gameVariant;
				
				switch (m_chessboard->variant())
				{
				case Chess::StandardChess:
					// TODO: Chess 960
					gameVariant = "normal";
					break;
				case Chess::CapablancaChess:
					// TODO: Gothic chess
					gameVariant = "capablanca";
					break;
				default:
					return;
				}
				
				if (!variants.contains(gameVariant))
					qDebug("Engine %s doesn't support variant %s",
					       qPrintable(m_name), qPrintable(gameVariant));
			}
			else if (feature == "done")
			{
				if (!m_initialized && val == "1")
				{
					Q_ASSERT(!m_isReady);
					m_initialized = true;
					m_isReady = true;
					write("accepted done");
					flushWriteBuffer();
				}
			}
		}
	}
}


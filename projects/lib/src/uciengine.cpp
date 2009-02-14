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

#include <QString>
#include <QStringList>
#include <QDebug>

#include "uciengine.h"
#include "chessboard/chessboard.h"
#include "chessboard/chessmove.h"
#include "timecontrol.h"


UciEngine::UciEngine(QIODevice* ioDevice,
                     Chess::Board* chessboard,
                     QObject* parent)
	: ChessEngine(ioDevice, chessboard, parent)
{
	m_variants.append(Chess::Variant::Standard);
	
	setName("UciEngine");
	
	// Tell the engine to turn on Uci mode
	write("uci");

	// Don't send any commands to the engine until it's initialized.
	m_isReady = false;
}

UciEngine::~UciEngine()
{
	//write("quit");
}

void UciEngine::sendPosition()
{
	QString str("position");
	
	const Chess::Variant& variant = m_chessboard->variant();
	if (variant.isRandom() || m_startFen != variant.startingFen())
		str += QString(" fen ") + m_startFen;
	else
		str += " startpos";
	
	if (m_moves.count() > 0)
		str += QString(" moves ") + m_moves.join(" ");
	
	write(str);
}

static Chess::Variant variantCode(const QString& str)
{
	if (str == "UCI_Chess960")
		return Chess::Variant::Fischerandom;
	else if (str == "UCI_Capablanca")
		return Chess::Variant::Capablanca;
	else if (str == "UCI_Gothic")
		return Chess::Variant::Gothic;
	else if (str == "UCI_CapaRandom")
		return Chess::Variant::Caparandom;
	
	return Chess::Variant::NoVariant;
}

static QString variantString(Chess::Variant variant)
{
	switch (variant.code())
	{
	case Chess::Variant::Fischerandom:
		return "UCI_Chess960";
	case Chess::Variant::Capablanca:
		return "UCI_Capablanca";
	case Chess::Variant::Gothic:
		return "UCI_Gothic";
	case Chess::Variant::Caparandom:
		return "UCI_CapaRandom";
	default:
		return QString();
	}
}

void UciEngine::newGame(Chess::Side side, ChessPlayer* opponent)
{
	ChessPlayer::newGame(side, opponent);
	m_moves.clear();
	m_startFen = m_chessboard->fenString();
	
	const Chess::Variant& variant = m_chessboard->variant();
	if (variant != Chess::Variant::Standard)
		setOption(variantString(variant), "true");
	write("ucinewgame");
	sendPosition();
}

void UciEngine::makeMove(const Chess::Move& move)
{
	m_moves.append(m_chessboard->moveString(move, m_notation));
	sendPosition();
}

void UciEngine::go()
{
	const TimeControl* whiteTc = 0;
	const TimeControl* blackTc = 0;
	if (side() == Chess::White)
	{
		whiteTc = timeControl();
		blackTc = m_opponent->timeControl();
	}
	else if (side() == Chess::Black)
	{
		whiteTc = m_opponent->timeControl();
		blackTc = timeControl();
	}
	else
		qFatal("Player %s doesn't have a side", qPrintable(m_name));
	
	QString command = "go";
	if (m_timeControl.timePerMove() > 0)
		command += QString(" movetime ") + QString::number(m_timeControl.timePerMove());
	else
	{
		command += QString(" wtime ") + QString::number(whiteTc->timeLeft());
		command += QString(" btime ") + QString::number(blackTc->timeLeft());
		if (whiteTc->timeIncrement() > 0)
			command += QString(" winc ") + QString::number(whiteTc->timeIncrement());
		if (blackTc->timeIncrement() > 0)
			command += QString(" binc ") + QString::number(blackTc->timeIncrement());
		if (m_timeControl.movesLeft() > 0)
			command += QString(" movestogo ") + QString::number(m_timeControl.movesLeft());
	}
	write(command);
	
	ChessPlayer::go();
}

ChessEngine::Protocol UciEngine::protocol() const
{
	return ChessEngine::Uci;
}

void UciEngine::ping()
{
	if (m_isReady)
	{
		write("isready");
		m_isReady = false;
	}
}

void UciEngine::addVariants()
{
	foreach (const UciOption& option, m_options)
	{
		Chess::Variant v = variantCode(option.name());
		if (!v.isNone())
			m_variants.append(v);
	}
}

void UciEngine::parseLine(const QString& line)
{
	QString command = line.section(' ', 0, 0);
	QString args = line.right(line.length() - command.length() - 1);

	if (command == "bestmove")
	{
		QString moveString = args.section(' ', 0, 0);
		if (moveString.isEmpty())
			moveString = args;

		m_moves.append(moveString);
		Chess::Move move = m_chessboard->moveFromString(moveString);
		emitMove(move);
	}
	else if (command == "uciok")
	{
		if (!m_initialized)
		{
			Q_ASSERT(!m_isReady);
			addVariants();
			m_initialized = true;
			m_isReady = true;

			// TODO: Send the engine the "setoption" commands

			ping();
		}
	}
	else if (command == "readyok")
	{
		if (!m_isReady)
		{
			m_isReady = true;
			flushWriteBuffer();
			emit ready();
		}
	}
	else if (command == "id")
	{
		QString tag = args.section(' ', 0, 0);
		QString tagVal = args.section(' ', 1);
		
		if (tag == "name")
			m_name = tagVal;
	}
	else if (command == "option")
	{
		UciOption option(args);
		if (option.isOk())
			m_options.append(option);
		else
		{
			qDebug() << "Invalid UCI option from" << m_name << ":"
			         << args;
		}
	}
}

void UciEngine::setOption(const UciOption& option, const QString& value)
{
	if (!option.isOk())
	{
		qDebug() << "Trying to pass an invalid option to" << m_name;
		return;
	}
	
	setOption(option.name(), value);
}

void UciEngine::setOption(const QString& name, const QString& value)
{
	bool found = false;
	foreach (const UciOption& option, m_options)
	{
		if (option.name() == name)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		qDebug() << m_name << "doesn't have UCI option" << name;
		return;
	}
	
	write(QString("setoption name ") + name + QString(" value ") + value);
}

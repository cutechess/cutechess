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

#include "uciengine.h"

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

#include "board/board.h"
#include "timecontrol.h"

#include "enginebuttonoption.h"
#include "enginecheckoption.h"
#include "enginecombooption.h"
#include "enginespinoption.h"
#include "enginetextoption.h"


REGISTER_ENGINE_CLASS(UciEngine, "uci")

UciEngine::UciEngine(QObject* parent)
	: ChessEngine(parent)
{
	m_variants.append("standard");
	
	setName("UciEngine");
}

void UciEngine::startProtocol()
{
	// Tell the engine to turn on UCI mode
	write("uci");
}

void UciEngine::sendPosition()
{
	QString str("position");
	
	if (board()->isRandomVariant() || m_startFen != board()->defaultFenString())
		str += QString(" fen ") + m_startFen;
	else
		str += " startpos";
	
	if (!m_moveStrings.isEmpty())
		str += QString(" moves") + m_moveStrings;
	
	write(str);
}

static QString variantFromUci(const QString& str)
{
	if (!str.startsWith("UCI_"))
		return QString();

	if (str == "UCI_Chess960")
		return "fischerandom";
	if (str == "UCI_CapaRandom")
		return "caparandom";

	return str.mid(4).toLower();
}

static QString variantToUci(const QString& str)
{
	if (str.isEmpty())
		return QString();

	if (str == "fischerandom")
		return "UCI_Chess960";
	if (str == "caparandom")
		return "UCI_CapaRandom";

	QString tmp = QString("UCI_%1").arg(str);
	tmp[4] = tmp[4].toUpper();
	return tmp;
}

void UciEngine::startGame()
{
	QString variant(board()->variant());
	Q_ASSERT(supportsVariant(variant));

	m_moveStrings.clear();
	if (board()->isRandomVariant())
		m_startFen = board()->fenString(Chess::Board::ShredderFen);
	else
		m_startFen = board()->fenString(Chess::Board::XFen);
	
	if (variant != "standard")
		setOption(variantToUci(variant), "true");
	write("ucinewgame");

	if (getOption("UCI_Opponent") != 0)
		setOption("UCI_Opponent", opponent()->name());

	sendPosition();
}

void UciEngine::endGame(const Chess::Result& result)
{
	stopThinking();
	ChessEngine::endGame(result);
}

void UciEngine::makeMove(const Chess::Move& move)
{
	m_moveStrings += " " + board()->moveString(move, Chess::Board::LongAlgebraic);
	sendPosition();
}

void UciEngine::startThinking()
{
	const TimeControl* whiteTc = 0;
	const TimeControl* blackTc = 0;
	const TimeControl* myTc = timeControl();
	if (side() == Chess::White)
	{
		whiteTc = myTc;
		blackTc = opponent()->timeControl();
	}
	else if (side() == Chess::Black)
	{
		whiteTc = opponent()->timeControl();
		blackTc = myTc;
	}
	else
		qFatal("Player %s doesn't have a side", qPrintable(name()));
	
	QString command = "go";
	if (myTc->timePerMove() > 0)
		command += QString(" movetime %1").arg(myTc->timePerMove());
	else
	{
		command += QString(" wtime %1").arg(whiteTc->timeLeft());
		command += QString(" btime %1").arg(blackTc->timeLeft());
		if (whiteTc->timeIncrement() > 0)
			command += QString(" winc %1").arg(whiteTc->timeIncrement());
		if (blackTc->timeIncrement() > 0)
			command += QString(" binc %1").arg(blackTc->timeIncrement());
		if (myTc->movesLeft() > 0)
			command += QString(" movestogo %1").arg(myTc->movesLeft());
		if (myTc->maxDepth() > 0)
			command += QString(" depth %1").arg(myTc->maxDepth());
		if (myTc->nodeLimit() > 0)
			command += QString(" nodes %1").arg(myTc->nodeLimit());
	}
	write(command);
}

void UciEngine::stopThinking()
{
	if (state() == Thinking)
		write("stop");
}

QString UciEngine::protocol() const
{
	return "uci";
}

bool UciEngine::sendPing()
{
	write("isready");
	return true;
}

void UciEngine::sendQuit()
{
	write("quit");
}

void UciEngine::addVariants()
{
	foreach (const EngineOption* option, m_options)
	{
		QString variant(variantFromUci(option->name()));
		if (!variant.isEmpty())
			m_variants.append(variant);
	}
}

static QVector<QStringList> parseCommand(const QString& str, const QRegExp& rx)
{
	QVector<QStringList> attrs;

	// Put the attributes' names and values in a vector
	QString item;
	int pos = 0;
	int prevPos= 0;
	while ((pos = rx.indexIn(str, pos)) != -1)
	{
		if (!item.isEmpty())
		{
			QString val = str.mid(prevPos + 1, pos - prevPos - 2);
			attrs.append((QStringList() << item << val.trimmed()));
		}
		item = rx.cap();
		pos += rx.matchedLength();
		prevPos = pos;
	}
	if (prevPos >= str.length() - 1)
		return attrs; // No value for the last attribute

	// Add the last attribute to the vector
	if (!item.isEmpty())
	{
		QString val = str.right(str.length() - prevPos - 1);
		attrs.append((QStringList() << item << val.trimmed()));
	}

	return attrs;
}

void UciEngine::parseInfo(const QString& line)
{
	QRegExp rx("\\b(depth|seldepth|time|nodes|pv|multipv|score|currmove|"
		   "currmovenumber|hashfull|nps|tbhits|cpuload|string|"
		   "refutation|currline)\\b");
	QVector<QStringList> attrs = parseCommand(line, rx);

	foreach (const QStringList& attr, attrs)
	{
		if (attr[0] == "depth")
			m_eval.setDepth(attr[1].toInt());
		else if (attr[0] == "time")
			m_eval.setTime(attr[1].toInt());
		else if (attr[0] == "nodes")
			m_eval.setNodeCount(attr[1].toInt());
		else if (attr[0] == "pv")
			m_eval.setPv(attr[1]);
		else if (attr[0] == "score")
		{
			QRegExp rx2("\\b(cp|mate|lowerbound|upperbound)\\b");
			QVector<QStringList> attrs2 = parseCommand(attr[1], rx2);
			int score = 0;
			bool hasScore = false;

			foreach (const QStringList& attr2, attrs2)
			{
				if (attr2[0] == "cp")
				{
					score = attr2[1].toInt();
					if (m_whiteEvalPov && side() == Chess::Black)
						score = -score;
					hasScore = true;
				}
				else if (attr2[0] == "lowerbound"
				     ||  attr2[0] == "upperbound")
				{
					hasScore = false;
					break;
				}
				else if (attr2[0] == "mate")
				{
					score = attr2[1].toInt();
					if (score > 0)
						score = 30001 - score * 2;
					else if (score < 0)
						score = -30000 - score * 2;
					hasScore = true;
				}
			}
			if (hasScore)
				m_eval.setScore(score);
		}
	}
}

EngineOption* UciEngine::parseOption(const QString& str)
{
	QString name;
	QString type;
	QString value;
	QStringList choices;
	int min = 0;
	int max = 0;

	QRegExp rx("\\b(name|type|default|min|max|var)\\b");
	QVector<QStringList> attrs = parseCommand(str, rx);

	foreach (const QStringList& attr, attrs)
	{
		if (attr.size() < 2)
			continue;

		if (attr[0] == "name")
			name = attr[1];
		else if (attr[0] == "type")
			type = attr[1];
		else if (attr[0] == "default")
			value = attr[1];
		else if (attr[0] == "min")
			min = attr[1].toInt();
		else if (attr[0] == "max")
			max = attr[1].toInt();
		else if (attr[0] == "var")
			choices << attr[1];
	}
	if (name.isEmpty())
		return 0;

	if (type == "button")
		return new EngineButtonOption(name);
	else if (type == "check")
		return new EngineCheckOption(name, value, value);
	else if (type == "combo")
		return new EngineComboOption(name, value, value, choices);
	else if (type == "spin")
		return new EngineSpinOption(name, value, value, min, max);
	else if (type == "string")
		return new EngineTextOption(name, value, value);

	return 0;
}

void UciEngine::parseLine(const QString& line)
{
	const QString command = line.section(' ', 0, 0);
	const QString args = line.right(line.length() - command.length() - 1);

	if (command == "bestmove")
	{
		if (state() != Thinking)
		{
			if (state() == FinishingGame)
				pong();
			else
				qDebug() << "Unexpected move from" << name();
			return;
		}

		QString moveString = args.section(' ', 0, 0);
		if (moveString.isEmpty())
			moveString = args;

		m_moveStrings += " " + moveString;
		Chess::Move move = board()->moveFromString(moveString);

		if (!move.isNull())
			emitMove(move);
		else
			emitForfeit(Chess::Result::IllegalMove, moveString);
	}
	else if (command == "uciok")
	{
		if (state() == Starting)
		{
			onProtocolStart();
			addVariants();
			ping();
		}
	}
	else if (command == "readyok")
	{
		pong();
	}
	else if (command == "id")
	{
		const QString tag = args.section(' ', 0, 0);
		const QString tagVal = args.section(' ', 1);
		
		if (tag == "name" && name() == "UciEngine")
			setName(tagVal);
	}
	else if (command == "registration")
	{
		if (args == "error")
		{
			qDebug() << "Failed to register UCI engine" << name();
			write("register later");
		}
	}
	else if (command == "option")
	{
		EngineOption* option = parseOption(args);

		if (option == 0 || !option->isValid())
			qDebug() << "Invalid UCI option from" << name() << ":"
				 << args;
		else
			m_options.append(option);
	}
	else if (command == "info")
	{
		parseInfo(args);
	}
}

void UciEngine::sendOption(const QString& name, const QString& value)
{
	write(QString("setoption name %1 value %2").arg(name).arg(value));
}

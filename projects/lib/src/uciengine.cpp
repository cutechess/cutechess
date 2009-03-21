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
#include "enginesettings.h"


UciEngine::UciEngine(QIODevice* ioDevice, QObject* parent)
	: ChessEngine(ioDevice, parent)
{
	m_notation = Chess::UciLongAlgebraic;
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

void UciEngine::applySettings(const EngineSettings& settings)
{
	ChessEngine::applySettings(settings);
	foreach(const EngineSettings::UciSetting& setting, settings.uciSettings())
		setOption(setting.name, setting.value);
}

void UciEngine::newGame(Chess::Side side, ChessPlayer* opponent)
{
	ChessPlayer::newGame(side, opponent);
	m_moves.clear();
	if (m_chessboard->variant().isRandom())
		m_startFen = m_chessboard->fenString(Chess::ShredderFen);
	else
		m_startFen = m_chessboard->fenString();
	
	const Chess::Variant& variant = m_chessboard->variant();
	if (variant != Chess::Variant::Standard)
		setOption(variantString(variant), "true");
	write("ucinewgame");
	sendPosition();
}

void UciEngine::endGame(Chess::Result result)
{
	if (m_timer.isActive())
		write("stop");
	ChessPlayer::endGame(result);
}

void UciEngine::makeMove(const Chess::Move& move)
{
	m_moves.append(m_chessboard->moveString(move, m_notation));
	sendPosition();
}

void UciEngine::go()
{
	if (m_isReady)
		ping(PingMove);
	else
		startClock();
	
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
		if (m_timeControl.maxDepth() > 0)
			command += QString(" depth ") + QString::number(m_timeControl.maxDepth());
		if (m_timeControl.nodeLimit() > 0)
			command += QString(" nodes ") + QString::number(m_timeControl.nodeLimit());
	}
	write(command);
}

ChessEngine::Protocol UciEngine::protocol() const
{
	return ChessEngine::Uci;
}

void UciEngine::ping(ChessEngine::PingType type)
{
	if (m_isReady)
	{
		m_pingType = type;
		write("isready");
		m_isReady = false;
		ChessEngine::ping(type);
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

void UciEngine::parseInfo(const QString& line)
{
	QRegExp rx("\\b(depth|seldepth|time|nodes|pv|multipv|score|currmove|"
		   "currmovenumber|hashfull|nps|tbhits|cpuload|string|"
		   "refutation|currline)\\b");
	QVector<QStringList> attrs = UciOption::parse(line, rx);

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
			QVector<QStringList> attrs2 = UciOption::parse(attr[1], rx2);
			int score = 0;
			bool hasScore = false;

			foreach (const QStringList& attr2, attrs2)
			{
				if (attr2[0] == "cp")
				{
					score = attr2[1].toInt();
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

void UciEngine::parseLine(const QString& line)
{
	QString command = line.section(' ', 0, 0);
	QString args = line.right(line.length() - command.length() - 1);

	if (command == "bestmove")
	{
		if (!m_gameInProgress)
			return;

		QString moveString = args.section(' ', 0, 0);
		if (moveString.isEmpty())
			moveString = args;

		m_moves.append(moveString);
		Chess::Move move = m_chessboard->moveFromString(moveString);

		if (m_chessboard->isLegalMove(move))
			emitMove(move);
		else
		{
			m_timer.stop();
			Chess::Result result(Chess::Result::WinByIllegalMove, otherSide(), moveString);
			emit forfeit(result);
		}
	}
	else if (command == "uciok")
	{
		if (!m_initialized)
		{
			Q_ASSERT(!m_isReady);
			addVariants();
			m_initialized = true;
			m_isReady = true;

			// Send engine options
			foreach (const OptionCmd& cmd, m_optionBuffer)
			{
				if (hasOption(cmd.name))
					this->setOption(cmd.name, cmd.value);
			}
			m_optionBuffer.clear();

			ping(PingInit);
		}
	}
	else if (command == "readyok")
	{
		pong();
	}
	else if (command == "id")
	{
		QString tag = args.section(' ', 0, 0);
		QString tagVal = args.section(' ', 1);
		
		if (tag == "name")
			m_name = tagVal;
	}
	else if (command == "registration")
	{
		if (args == "error")
		{
			qDebug() << "Failed to register UCI engine" << m_name;
			write("register later");
		}
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
	else if (command == "info")
	{
		parseInfo(args);
		if (!m_eval.isEmpty())
			emit sendEvaluation(m_eval);
	}
}

const UciOption* UciEngine::getOption(const QString& name) const
{
	for (int i = 0; i < m_options.size(); i++)
	{
		if (m_options[i].name() == name)
			return &m_options[i];
	}
	
	return 0;
}

bool UciEngine::hasOption(const QString& name) const
{
	foreach (const UciOption& option, m_options)
	{
		if (option.name() == name)
			return true;
	}
	return false;
}

void UciEngine::setOption(const UciOption* option, const QVariant& value)
{
	Q_ASSERT(option != 0);
	if (!option->isOk())
	{
		qDebug() << "Trying to pass an invalid option to" << m_name;
		return;
	}
	if (!option->isValueOk(value))
	{
		qDebug() << "Can't set option" << option->name()
		         << "to" << value.toString();
		return;
	}
	
	write(QString("setoption name ") + option->name() +
	      QString(" value ") + value.toString());
}

void UciEngine::setOption(const QString& name, const QVariant& value)
{
	if (!m_initialized)
	{
		OptionCmd cmd = { name, value };
		m_optionBuffer.append(cmd);
		return;
	}

	const UciOption* option = getOption(name);
	if (!option)
	{
		qDebug() << m_name << "doesn't have UCI option" << name;
		return;
	}
	
	setOption(option, value);
}

void UciEngine::setConcurrency(int limit)
{
	setOption("Threads", limit);
}

void UciEngine::setEgbbPath(const QString& path)
{
	// Stupid computer chess community and their lack of standards...
	setOption("Bitbase Path", path);
	setOption("Bitbases Path", path);
}

void UciEngine::setEgtbPath(const QString& path)
{
	setOption("NalimovPath", path);
}

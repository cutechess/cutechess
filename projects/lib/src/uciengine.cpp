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

#include "board/board.h"
#include "board/boardfactory.h"
#include "timecontrol.h"

#include "enginebuttonoption.h"
#include "enginecheckoption.h"
#include "enginecombooption.h"
#include "enginespinoption.h"
#include "enginetextoption.h"


UciEngine::UciEngine(QObject* parent)
	: ChessEngine(parent),
	  m_sendOpponentsName(false),
	  m_canPonder(false),
	  m_ponderState(NotPondering),
	  m_ignoreThinking(false),
	  m_rePing(false)
{
	addVariant("standard");
	setName("UciEngine");
}

void UciEngine::startProtocol()
{
	// Tell the engine to turn on UCI mode
	write("uci");
}

QString UciEngine::positionString()
{
	QString str("position");

	if (board()->isRandomVariant() || m_startFen != board()->defaultFenString())
		str += QString(" fen ") + m_startFen;
	else
		str += " startpos";

	if (!m_moveStrings.isEmpty())
		str += QString(" moves") + m_moveStrings;

	return str;
}

void UciEngine::sendPosition()
{
	write(positionString());
}

static QString variantFromUci(const QString& str)
{
	if (str.size() < 5 || !str.startsWith("UCI_"))
		return QString();

	QString variant;

	if (str == "UCI_Chess960")
		variant = "fischerandom";
	else
		variant = str.mid(4).toLower();

	if (!Chess::BoardFactory::variants().contains(variant))
		return QString();
	return variant;
}

static QString variantToUci(const QString& str)
{
	if (str.isEmpty() || str == "standard")
		return QString();

	if (str == "fischerandom")
		return "UCI_Chess960";
	if (str == "caparandom")
		return "UCI_CapaRandom";

	QString tmp = QString("UCI_%1").arg(str);
	tmp[4] = tmp.at(4).toUpper();
	return tmp;
}

void UciEngine::startGame()
{
	Q_ASSERT(supportsVariant(board()->variant()));

	m_ignoreThinking = false;
	m_rePing = false;
	m_ponderState = NotPondering;
	m_ponderMove = Chess::Move();
	m_bmBuffer.clear();
	m_moveStrings.clear();

	if (board()->isRandomVariant())
		m_startFen = board()->fenString(Chess::Board::ShredderFen);
	else
		m_startFen = board()->fenString(Chess::Board::XFen);
	
	QString uciVariant(variantToUci(board()->variant()));
	if (uciVariant != m_variantOption)
	{
		if (!m_variantOption.isEmpty())
			sendOption(m_variantOption, false);
		m_variantOption = uciVariant;
	}
	if (!m_variantOption.isEmpty())
		sendOption(m_variantOption, true);

	write("ucinewgame");

	if (m_canPonder)
		sendOption("Ponder", pondering());

	if (m_sendOpponentsName)
	{
		QString opType = opponent()->isHuman() ? "human" : "computer";
		QString value = QString("none none %1 %2")
				.arg(opType)
				.arg(opponent()->name());
		sendOption("UCI_Opponent", value);
	}

	sendPosition();
}

void UciEngine::endGame(const Chess::Result& result)
{
	m_ignoreThinking = true;
	if (stopThinking())
		ping(false);
	ChessEngine::endGame(result);
}

void UciEngine::makeMove(const Chess::Move& move)
{
	if (!m_ponderMove.isNull())
	{
		if (move == m_ponderMove)
			m_ponderState = PonderHit;

		m_ponderMove = Chess::Move();
		if (m_ponderState != PonderHit)
		{
			m_moveStrings.truncate(m_moveStrings.lastIndexOf(' '));
			if (isReady())
			{
				m_ignoreThinking = true;
				if (stopThinking())
					ping(false);
			}
			else
			{
				// Cancel sending the "go ponder" message
				clearWriteBuffer();
				m_rePing = true;
			}
		}
	}

	if (m_ponderState != PonderHit)
	{
		m_ponderState = NotPondering;
		m_moveStrings += " " + board()->moveString(move, Chess::Board::LongAlgebraic);
		if (m_ignoreThinking)
			m_bmBuffer << positionString() << "isready";
		else
			sendPosition();
	}
}

void UciEngine::startThinking()
{
	if (m_ponderState == PonderHit)
	{
		m_ponderState = NotPondering;
		write("ponderhit");
		return;
	}

	const TimeControl* whiteTc = nullptr;
	const TimeControl* blackTc = nullptr;
	const TimeControl* myTc = timeControl();
	if (side() == Chess::Side::White)
	{
		whiteTc = myTc;
		blackTc = opponent()->timeControl();
	}
	else if (side() == Chess::Side::Black)
	{
		whiteTc = opponent()->timeControl();
		blackTc = myTc;
	}
	else
		qFatal("Player %s doesn't have a side", qPrintable(name()));
	
	QString command = "go";
	if (!m_ponderMove.isNull())
	{
		command += " ponder";
		m_ponderState = Pondering;
	}
	else
		m_ponderState = NotPondering;
	if (myTc->isInfinite())
	{
		if (myTc->plyLimit() == 0 && myTc->nodeLimit() == 0)
			command += " infinite";
	}
	else if (myTc->timePerMove() > 0)
		command += QString(" movetime %1").arg(myTc->timeLeft());
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
	}
	if (myTc->plyLimit() > 0)
		command += QString(" depth %1").arg(myTc->plyLimit());
	if (myTc->nodeLimit() > 0)
		command += QString(" nodes %1").arg(myTc->nodeLimit());

	write(command);
}

void UciEngine::startPondering()
{
	if (m_ponderMove.isNull())
		return;

	m_moveStrings += " " + board()->moveString(m_ponderMove, Chess::Board::LongAlgebraic);
	sendPosition();
	ping();
	startThinking();
}

bool UciEngine::isPondering() const
{
	return (m_ponderState != NotPondering);
}

void UciEngine::sendStop()
{
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

QStringRef UciEngine::parseUciTokens(const QStringRef& first,
				     const QString* types,
				     int typeCount,
				     QVarLengthArray<QStringRef>& tokens,
				     int& type)
{
	QStringRef token(first);
	type = -1;
	tokens.clear();

	do
	{
		bool newType = false;
		for (int i = 0; i < typeCount; i++)
		{
			if (token == types[i])
			{
				if (type != -1)
					return token;
				type = i;
				newType = true;
				break;
			}
		}
		if (!newType && type != -1)
			tokens.append(token);
	}
	while (!(token = nextToken(token)).isNull());

	return token;
}

static QStringRef joinTokens(const QVarLengthArray<QStringRef>& tokens)
{
	Q_ASSERT(!tokens.isEmpty());

	const QStringRef& last = tokens[tokens.size() - 1];
	int start = tokens[0].position();
	int end = last.position() + last.size();

	return QStringRef(last.string(), start, end - start);
}

void UciEngine::parseInfo(const QVarLengthArray<QStringRef>& tokens,
			  int type)
{
	enum Keyword
	{
		InfoDepth,
		InfoSelDepth,
		InfoTime,
		InfoNodes,
		InfoPv,
		InfoMultiPv,
		InfoScore,
		InfoCurrMove,
		InfoCurrMoveNumber,
		InfoHashFull,
		InfoNps,
		InfoTbHits,
		InfoCpuLoad,
		InfoString,
		InfoRefutation,
		InfoCurrLine
	};

	if (tokens.isEmpty())
		return;
	
	switch (type)
	{
	case InfoDepth:
		m_eval.setDepth(tokens[0].toString().toInt());
		break;
	case InfoTime:
		m_eval.setTime(tokens[0].toString().toInt());
		break;
	case InfoNodes:
		m_eval.setNodeCount(tokens[0].toString().toULongLong());
		break;
	case InfoPv:
		m_eval.setPv(joinTokens(tokens).toString());
		break;
	case InfoScore:
		{
			int score = 0;
			for (int i = 1; i < tokens.size(); i++)
			{
				if (tokens[i - 1] == "cp")
				{
					score = tokens[i].toString().toInt();
					if (whiteEvalPov()
					&&  side() == Chess::Side::Black)
						score = -score;
				}
				else if (tokens[i - 1] == "mate")
				{
					score = tokens[i].toString().toInt();
					if (score > 0)
						score = 30001 - score * 2;
					else if (score < 0)
						score = -30000 - score * 2;
				}
				else if (tokens[i - 1] == "lowerbound"
				     ||  tokens[i - 1] == "upperbound")
					return;
				i++;
			}
			m_eval.setScore(score);
		}
		break;
	default:
		break;
	}
}

void UciEngine::parseInfo(const QStringRef& line)
{
	static const QString types[] =
	{
		"depth",
		"seldepth",
		"time",
		"nodes",
		"pv",
		"multipv",
		"score",
		"currmove",
		"currmovenumber",
		"hashfull",
		"nps",
		"tbhits",
		"cpuload",
		"string",
		"refutation",
		"currline"
	};

	int type = -1;
	QStringRef token(nextToken(line));
	QVarLengthArray<QStringRef> tokens;

	while (!token.isNull())
	{
		token = parseUciTokens(token, types, 16, tokens, type);
		parseInfo(tokens, type);
	}
}

EngineOption* UciEngine::parseOption(const QStringRef& line)
{
	enum Keyword
	{
		OptionName,
		OptionType,
		OptionDefault,
		OptionMin,
		OptionMax,
		OptionVar
	};
	static const QString types[] =
	{
		"name",
		"type",
		"default",
		"min",
		"max",
		"var"
	};

	QString name;
	QString type;
	QString value;
	QStringList choices;
	int min = 0;
	int max = 0;
	
	int keyword = -1;
	QStringRef token(nextToken(line));
	QVarLengthArray<QStringRef> tokens;

	while (!token.isNull())
	{
		token = parseUciTokens(token, types, 6, tokens, keyword);
		if (tokens.isEmpty() || keyword == -1)
			continue;

		QString str(joinTokens(tokens).toString());

		switch (keyword)
		{
		case OptionName:
			name = str;
			break;
		case OptionType:
			type = str;
			break;
		case OptionDefault:
			value = str;
			break;
		case OptionMin:
			min = str.toInt();
			break;
		case OptionMax:
			max = str.toInt();
			break;
		case OptionVar:
			choices << str;
			break;
		}
	}
	if (name.isEmpty())
		return nullptr;

	if (type == "button")
		return new EngineButtonOption(name);
	else if (type == "check")
	{
		if (value == "true")
			return new EngineCheckOption(name, true, true);
		else
			return new EngineCheckOption(name, false, false);
	}
	else if (type == "combo")
		return new EngineComboOption(name, value, value, choices);
	else if (type == "spin")
		return new EngineSpinOption(name, value.toInt(), value.toInt(), min, max);
	else if (type == "string")
		return new EngineTextOption(name, value, value);
	else if (type == "filename")
		return new EngineTextOption(name, value, value, QString(),
					    EngineTextOption::FileDialog);

	return nullptr;
}

void UciEngine::parseLine(const QString& line)
{
	const QStringRef command(firstToken(line));

	if (command == "info")
	{
		if (m_ignoreThinking)
			return;
		parseInfo(command);
	}
	else if (command == "bestmove")
	{
		m_ponderState = NotPondering;
		if (m_ignoreThinking)
		{
			m_ignoreThinking = false;
			if (!m_bmBuffer.isEmpty())
			{
				foreach (const QString& line, m_bmBuffer)
					write(line, Unbuffered);
				m_bmBuffer.clear();
			}
			else
				pong();
			return;
		}

		if (state() != Thinking)
		{
			if (state() == FinishingGame)
				pong();
			else
				qDebug() << "Unexpected move from" << name();
			return;
		}

		QStringRef token(nextToken(command));
		QString moveString(token.toString());
		m_moveStrings += " " + moveString;
		Chess::Move move = board()->moveFromString(moveString);
		if (move.isNull())
		{
			forfeit(Chess::Result::IllegalMove, moveString);
			return;
		}

		if (m_canPonder && pondering()
		&&  (token = nextToken(token)) == "ponder")
		{
			board()->makeMove(move);
			setPonderMove(nextToken(token).toString());
			board()->undoMove();
		}

		emitMove(move);
	}
	else if (command == "readyok")
	{
		if (m_rePing)
		{
			m_rePing = false;
			pong(false);
			ping();
		}
		else
			pong();
	}
	else if (command == "uciok")
	{
		if (state() == Starting)
		{
			onProtocolStart();
			ping();
		}
	}
	else if (command == "id")
	{
		QStringRef tag(nextToken(command));
		if (tag == "name" && name() == "UciEngine")
			setName(nextToken(tag, true).toString());
	}
	else if (command == "registration")
	{
		if (nextToken(command) == "error")
		{
			qDebug("Failed to register UCI engine %s", qPrintable(name()));
			write("register later");
		}
	}
	else if (command == "option")
	{
		EngineOption* option = parseOption(command);
		QString variant;

		if (option == nullptr || !option->isValid())
			qDebug("Invalid UCI option from %s: %s",
				qPrintable(name()), qPrintable(line));
		else if (!(variant = variantFromUci(option->name())).isEmpty())
			addVariant(variant);
		else if (option->name() == "UCI_Opponent")
			m_sendOpponentsName = true;
		else if (option->name() == "Ponder")
			m_canPonder = true;
		else if (option->name().startsWith("UCI_") &&
			 option->name() != "UCI_LimitStrength" &&
			 option->name() != "UCI_Elo")
		{
			// TODO: Deal with UCI features
		}
		else
		{
			addOption(option);
			return;
		}

		delete option;
	}
}

void UciEngine::setPonderMove(const QString& moveString)
{
	Chess::Board* board = this->board();

	m_ponderMove = board->moveFromString(moveString);
	if (m_ponderMove.isNull())
	{
		qDebug("Illegal ponder move from %s: %s",
		       qPrintable(name()),
		       qPrintable(moveString));
	}
	else
	{
		// Ignore game ending ponder moves
		board->makeMove(m_ponderMove);
		if (!board->result().isNone())
			m_ponderMove = Chess::Move();
		board->undoMove();
	}
}

void UciEngine::sendOption(const QString& name, const QVariant& value)
{
	if (!value.isNull())
		write(QString("setoption name %1 value %2").arg(name).arg(value.toString()));
	else
		write(QString("setoption name %1").arg(name));
}

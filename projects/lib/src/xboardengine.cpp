/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#include <climits>

#include "timecontrol.h"
#include "enginebuttonoption.h"
#include "enginecheckoption.h"
#include "enginecombooption.h"
#include "enginespinoption.h"
#include "enginetextoption.h"

namespace {

QString msToXboardTime(int ms)
{
	int sec = ms / 1000;

	QString number = QString::number(sec / 60);
	if (sec % 60 != 0)
		number += QString(":%1").arg(sec % 60, 2, 10, QChar('0'));
	
	return number;
}

QString variantFromXboard(const QString& str)
{
	if (str == "normal")
		return "standard";

	return str;
}

QString variantToXboard(const QString& str)
{
	if (str == "standard")
		return "normal";

	return str;
}

const int s_infiniteSec = 86400;

} // anonymous namespace

XboardEngine::XboardEngine(QObject* parent)
	: ChessEngine(parent),
	  m_forceMode(false),
	  m_drawOnNextMove(false),
	  m_ftName(false),
	  m_ftPing(false),
	  m_ftSetboard(false),
	  m_ftTime(true),
	  m_ftUsermove(false),
	  m_ftReuse(true),
	  m_ftNps(false),
	  m_gotResult(false),
	  m_lastPing(0),
	  m_notation(Chess::Board::LongAlgebraic),
	  m_initTimer(new QTimer(this))
{
	m_initTimer->setSingleShot(true);
	m_initTimer->setInterval(8000);
	connect(m_initTimer, SIGNAL(timeout()), this, SLOT(initialize()));

	addVariant("standard");
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

void XboardEngine::startGame()
{
	m_drawOnNextMove = false;
	m_gotResult = false;
	m_forceMode = false;
	m_nextMove = Chess::Move();
	write("new");
	
	if (board()->variant() != "standard")
		write("variant " + variantToXboard(board()->variant()));
	
	setForceMode(true);

	if (board()->isRandomVariant()
	||  board()->fenString() != board()->defaultFenString())
	{
		if (board()->sideToMove() == Chess::Side::Black)
		{
			// Use a dummy move to force the engine to play black
			write("b2b3");
		}
		if (m_ftSetboard)
			write("setboard " + board()->fenString());
		else
		{
			qWarning("%s does not support the setboard command, using the edit command now",
				 qUtf8Printable(name()));
			write("edit");
			write("#"); // clear board on engine
			const QStringList& whitePieces = board()->pieceList(Chess::Side::White);
			for (const auto& s: whitePieces)
				write(s); // set a piece
			write("c");
			const QStringList& blackPieces = board()->pieceList(Chess::Side::Black);
			for (const auto& s: blackPieces)
				write(s); // set a piece
			write("."); // finished
		}
	}
	
	// Send the time controls
	const TimeControl* myTc = timeControl();
	if (myTc->isInfinite())
	{
		if (myTc->plyLimit() == 0 && myTc->nodeLimit() == 0)
			write(QString("st %1").arg(s_infiniteSec));
	}
	else if (myTc->timePerMove() > 0)
		write(QString("st %1").arg(myTc->timePerMove() / 1000));
	else
		write(QString("level %1 %2 %3")
		      .arg(myTc->movesPerTc())
		      .arg(msToXboardTime(myTc->timePerTc()))
		      .arg(double(myTc->timeIncrement()) / 1000));

	if (myTc->plyLimit() > 0)
		write(QString("sd %1").arg(myTc->plyLimit()));
	if (myTc->nodeLimit() > 0)
	{
		if (m_ftNps)
			write(QString("st 1\nnps %1").arg(myTc->nodeLimit()));
		else
			qWarning("%s doesn't support the nps command",
				 qUtf8Printable(name()));
	}

	// Show thinking
	write("post");
	// Pondering
	if (pondering())
		write("hard");
	else
		write("easy");
	
	// Tell the opponent's type and name to the engine
	if (m_ftName)
	{
		if (!opponent()->isHuman())
			write("computer");
		write("name " + opponent()->name());
	}
}

bool XboardEngine::restartsBetweenGames() const
{
	if (restartMode() == EngineConfiguration::RestartAuto)
		return !m_ftReuse;
	return ChessEngine::restartsBetweenGames();
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
	
	if (timeControl()->isInfinite())
	{
		write(QString("time %1").arg(s_infiniteSec));
		return;
	}

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

// compensation: xboard protocol shifts ranks one down if board height equals ten
const QString XboardEngine::transformMove(const QString& str, int height, int shift) const
{
	if (height != 10)
	      return str;

	QString ret, num;
	for (const auto c: str)
	{
		if (c.isDigit())
			num.append(c);
		else
		{
			if (!num.isEmpty())
				ret.append(QString::number(shift + num.toInt()));
			num.clear();
			ret.append(c);
		}
	}
	if (!num.isEmpty())
		ret.append(QString::number(shift + num.toInt()));

	return ret;
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

	moveString = transformMove(moveString, board()->height(), -1);

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

void XboardEngine::onTimeout()
{
	if (m_drawOnNextMove)
	{
		Q_ASSERT(state() == Thinking);

		m_drawOnNextMove = false;
		claimResult(Chess::Result(Chess::Result::Draw));
	}
	else
		ChessEngine::onTimeout();
}

void XboardEngine::sendStop()
{
	write("?");
}

QString XboardEngine::protocol() const
{
	return "xboard";
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

EngineOption* XboardEngine::parseOption(const QString& line)
{
	int start = line.indexOf(" -") + 1;
	if (start < 2)
		return nullptr;

	QString name(line.left(start - 1));

	start++;
	int end = line.indexOf(' ', start);
	if (end == -1)
		end = line.length();
	QString type(line.mid(start, end - start));

	if (type == "button" || type == "save")
		return new EngineButtonOption(name);
	if (type == "check")
	{
		bool value = line.mid(end + 1) == "1";
		return new EngineCheckOption(name, value, value);
	}
	if (type == "string" || type == "file" || type == "path")
	{
		QString value(line.mid(end + 1));
		EngineTextOption::EditorType editorType;

		if (type == "file")
			editorType = EngineTextOption::FileDialog;
		else if (type == "path")
			editorType = EngineTextOption::FolderDialog;
		else
			editorType = EngineTextOption::LineEdit;

		return new EngineTextOption(name, value, value, QString(), editorType);
	}
	if (type == "spin" || type == "slider")
	{
		QStringList params(line.mid(end + 1).split(' ', QString::SkipEmptyParts));
		if (params.size() != 3)
			return nullptr;

		bool ok = false;
		int value = params.at(0).toInt(&ok);
		if (!ok)
			return nullptr;

		int min = params.at(1).toInt(&ok);
		if (!ok || min > value)
			return nullptr;

		int max = params.at(2).toInt(&ok);
		if (!ok || max < value)
			return nullptr;

		return new EngineSpinOption(name, value, value, min, max);
	}
	if (type == "combo")
	{
		QStringList choices = line.mid(end + 1).split(" /// ", QString::SkipEmptyParts);
		if (choices.isEmpty())
			return nullptr;

		QString value;
		QStringList::iterator it;
		for (it = choices.begin(); it != choices.end(); ++it)
		{
			if (it->startsWith('*'))
			{
				it->remove(0, 1);
				value = *it;
			}
		}
		if (value.isEmpty())
			value = choices.first();

		return new EngineComboOption(name, value, value, choices);
	}

	return nullptr;
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
	else if (name == "nps")
		m_ftNps = (val == "1");
	else if (name == "time")
		m_ftTime = (val == "1");
	else if (name == "reuse")
		m_ftReuse = (val == "1");
	else if (name == "myname")
	{
		if (this->name() == "XboardEngine")
			setName(val);
	}
	else if (name == "variants")
	{
		clearVariants();
		const auto variants = val.split(',');
		for (const QString& str : variants)
		{
			QString variant = variantFromXboard(str.trimmed());
			if (!variant.isEmpty())
				addVariant(variant);
		}
	}
	else if (name == "name")
		m_ftName = (val == "1");
	else if (name == "memory")
	{
		if (val == "1")
			addOption(new EngineSpinOption("memory", 32, 32, 0, INT_MAX - 1));
	}
	else if (name == "smp")
	{
		if (val == "1")
			addOption(new EngineSpinOption("cores", 1, 1, 0, INT_MAX - 1));
	}
	else if (name == "egt")
	{
		const auto list = val.split(',');
		for (const QString& str : list)
		{
			QString egtType = QString("egtpath %1").arg(str.trimmed());
			addOption(new EngineTextOption(egtType, QString(), QString()));
		}
	}
	else if (name == "option")
	{
		EngineOption* option = parseOption(val);
		if (option == nullptr || !option->isValid())
			qWarning() << "Invalid Xboard option from" << this->name()
				   << ":" << val;
		else
			addOption(option);
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

// shift assumed mate scores further out
int XboardEngine::adaptScore(int score) const
{
	constexpr static int mateScore = 99000;
	constexpr static int newCECPMateScore = 100000;
	int absScore = qAbs<int>(score);

	// convert new CECP mate scores to old ones
	if (absScore > newCECPMateScore
	&&  absScore < newCECPMateScore + 100)
	{
		absScore = 2 * newCECPMateScore - 2 * absScore + mateScore;
		if (score >= absScore)
			absScore++;
	}

	// map assumed mate scores onto equivalents w/ higher absolute values
	int distance = 1000 - (absScore % 1000);
	if (absScore > 9900 &&  distance < 100)
		score = (score > 0) ? mateScore - distance
				    : -mateScore + distance;

	return score;
}

void XboardEngine::parseLine(const QString& line)
{
	const QStringRef command(firstToken(line));
	if (command.isEmpty())
		return;

	if (command == "1-0" || command == "0-1"
	||  command == "*" || command == "1/2-1/2" || command == "resign")
	{
		if ((state() != Thinking && state() != Observing)
		||  !board()->result().isNone())
		{
			finishGame();
			return;
		}

		QString description(nextToken(command, true).toString());
		if (description.startsWith('{'))
			description.remove(0, 1);
		if (description.endsWith('}'))
			description.chop(1);

		if (command == "*")
			claimResult(Chess::Result(Chess::Result::NoResult,
						  Chess::Side::NoSide,
						  description));
		else if (command == "1/2-1/2")
		{
			if (state() == Thinking && areClaimsValidated())
				// The engine claims that its next move will draw the game
				m_drawOnNextMove = true;
			else
				claimResult(Chess::Result(Chess::Result::Draw,
							  Chess::Side::NoSide,
							  description));
		}
		else if ((command == "1-0" && side() == Chess::Side::White)
		     ||  (command == "0-1" && side() == Chess::Side::Black))
			claimResult(Chess::Result(Chess::Result::Win,
						  side(),
						  description));
		else
			forfeit(Chess::Result::Resignation);

		return;
	}
	else if (command.at(0).isDigit()
	     && !command.contains("."))	// principal variation
	{
		bool ok = false;
		int val = 0;
		QStringRef ref(command);
		
		// Search depth
		QString depth(ref.toString());
		if (!(depth.cend() - 1)->isDigit())
			depth.chop(1);
		m_eval.setDepth(depth.toInt());

		// Evaluation
		if ((ref = nextToken(ref)).isNull())
			return;
		val = ref.toString().toInt(&ok);
		if (ok)
		{
			if (whiteEvalPov() && side() == Chess::Side::Black)
				val = -val;
			m_eval.setScore(adaptScore(val));
		}

		// Search time
		if ((ref = nextToken(ref)).isNull())
			return;
		val = ref.toString().toInt(&ok);
		if (ok)
			m_eval.setTime(val * 10);

		// Node count
		if ((ref = nextToken(ref)).isNull())
			return;
		val = ref.toString().toULongLong(&ok);
		if (ok)
			m_eval.setNodeCount(val);

		// Principal variation
		if ((ref = nextToken(ref, true)).isNull())
			return;
		m_eval.setPv(ref.toString());

		emit thinking(m_eval);

		return;
	}

	// move format of old CECP engines: 1. ... e2e4
	bool testDigitAndDot = command.at(0).isDigit() && command.contains(".");

	const QString args(nextToken(command, true).toString());
	if (command == "move" || (testDigitAndDot && args.startsWith("...")))
	{
		if (state() != Thinking)
		{
			if (state() == FinishingGame)
				finishGame();
			else
				qWarning("Unexpected move from %s",
					 qUtf8Printable(name()));
			return;
		}

		// remove "..." of old format if necessary
		int mark = (args.indexOf("..."));
		const QString& movestr = mark < 0 ? args : args.mid(4);
		const QString& newMovestr = transformMove(movestr, board()->height(), +1);

		Chess::Move move = board()->moveFromString(newMovestr);
		if (move.isNull())
		{
			forfeit(Chess::Result::IllegalMove, newMovestr);
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
				claimResult(Chess::Result(Chess::Result::Draw));
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
	else if (command == "feature")
	{
		QRegExp rx("\\w+\\s*=\\s*(\"[^\"]*\"|\\d+)");
		
		int pos = 0;
		
		while ((pos = rx.indexIn(args, pos)) != -1)
		{
			QString cap = rx.cap();
			int index = cap.indexOf('=');
			if (index != -1)
			{
				QString feature = cap.left(index).trimmed();
				QString val = cap.mid(index + 1).trimmed();
				val.remove('\"');

				setFeature(feature, val);
			}

			pos += rx.matchedLength();
		}
	}
	else if (command.startsWith("Illegal"))
	{
		forfeit(Chess::Result::Adjudication,
			tr("%1 claims illegal %2")
			.arg(this->side().toString())
			.arg(args));
	}
	else if (command == "Error")
	{
		// If the engine complains about an unknown result command,
		// we can assume that it's safe to finish the game.
		QString str = args.section(':', 1).trimmed();
		if (str.startsWith("result"))
			finishGame();
	}
}

void XboardEngine::sendOption(const QString& name, const QVariant& value)
{
	if (name == "memory" || name == "cores" || name.startsWith("egtpath "))
		write(name + " " + value.toString());
	else
	{
		if (value.isNull())
			write("option " + name);
		else
		{
			QString tmp;
			if (value.type() == QVariant::Bool)
				tmp = value.toBool() ? "1" : "0";
			else
				tmp = value.toString();
			write("option " + name + "=" + tmp);
		}
	}
}

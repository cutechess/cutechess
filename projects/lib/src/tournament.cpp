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


#include "tournament.h"
#include <QFile>
#include <QMultiMap>
#include <QSet>
#include "gamemanager.h"
#include "playerbuilder.h"
#include "board/boardfactory.h"
#include "chessplayer.h"
#include "chessgame.h"
#include "pgnstream.h"
#include "openingsuite.h"
#include "openingbook.h"
#include "sprt.h"
#include "elo.h"


Tournament::Tournament(GameManager* gameManager, QObject *parent)
	: QObject(parent),
	  m_gameManager(gameManager),
	  m_lastGame(nullptr),
	  m_variant("standard"),
	  m_round(0),
	  m_oldRound(-1),
	  m_nextGameNumber(0),
	  m_finishedGameCount(0),
	  m_savedGameCount(0),
	  m_finalGameCount(0),
	  m_gamesPerEncounter(1),
	  m_roundMultiplier(1),
	  m_startDelay(0),
	  m_openingDepth(1024),
	  m_seedCount(0),
	  m_stopping(false),
	  m_openingRepetitions(1),
	  m_openingPolicy(DefaultPolicy),
	  m_recover(false),
	  m_pgnCleanup(true),
	  m_pgnWriteUnfinishedGames(true),
	  m_finished(false),
	  m_bookOwnership(false),
	  m_openingSuite(nullptr),
	  m_sprt(new Sprt),
	  m_repetitionCounter(0),
	  m_swapSides(true),
	  m_reverseSides(false),
	  m_resultFormat(c_defaultFormat),
	  m_pgnOutMode(PgnGame::Verbose),
	  m_pair(nullptr)
{
	Q_ASSERT(gameManager != nullptr);

	// Assign header map
	m_headerMap.insert(Rank,            QString("%1 ").arg("Rank", 4));
	m_headerMap.insert(Name,            QString("%1 ").arg("Name", -25));
	m_headerMap.insert(EloDiff,         QString("%1 ").arg("Elo", 7));
	m_headerMap.insert(ErrorMargin,     QString("%1 ").arg("+/-", 7));
	m_headerMap.insert(Games,           QString("%1 ").arg("Games", 7));
	m_headerMap.insert(Wins,            QString("%1 ").arg("Wins", 7));
	m_headerMap.insert(Losses,          QString("%1 ").arg("Losses", 7));
	m_headerMap.insert(Draws,           QString("%1 ").arg("Draws", 7));
	m_headerMap.insert(Points,          QString("%1 ").arg("Points", 8));
	m_headerMap.insert(Score,           QString("%1 ").arg("Score", 7));
	m_headerMap.insert(DrawScore,       QString("%1 ").arg("Draw", 7));
	m_headerMap.insert(WhiteScore,      QString("%1 ").arg("White", 7));
	m_headerMap.insert(BlackScore,      QString("%1 ").arg("Black", 7));
	m_headerMap.insert(WhiteDrawScore,  QString("%1 ").arg("WDraw", 7));
	m_headerMap.insert(BlackDrawScore,  QString("%1 ").arg("BDraw", 7));
	m_headerMap.insert(WhiteWins,       QString("%1 ").arg("WWins", 7));
	m_headerMap.insert(WhiteLosses,     QString("%1 ").arg("WLoss.", 7));
	m_headerMap.insert(WhiteDraws,      QString("%1 ").arg("WDraws", 7));
	m_headerMap.insert(BlackWins,       QString("%1 ").arg("BWins", 7));
	m_headerMap.insert(BlackLosses,     QString("%1 ").arg("BLoss.", 7));
	m_headerMap.insert(BlackDraws,      QString("%1 ").arg("BDraws", 7));
	m_headerMap.insert(TimeForfeits,    QString("%1 ").arg("Time", 7));
	m_headerMap.insert(IllegalMoves,    QString("%1 ").arg("Illegal", 7));
	m_headerMap.insert(Disconnections,  QString("%1 ").arg("Discon", 7));
	m_headerMap.insert(StalledConnections,  QString("%1 ").arg("Stall", 7));
	m_headerMap.insert(InvalidResultClaims, QString("%1 ").arg("WrClaim", 7));
	m_headerMap.insert(RegularLosses,       QString("%1 ").arg("RegulL", 7));
	m_headerMap.insert(RegularWins,         QString("%1 ").arg("RegulW", 7));
	m_headerMap.insert(OtherWins,           QString("%1 ").arg("OtherW", 7));
	m_headerMap.insert(DrawsByStalemate,    QString("%1 ").arg("DStale", 7));
	m_headerMap.insert(DrawsByMaterial,     QString("%1 ").arg("DMater", 7));
	m_headerMap.insert(DrawsByRepetiton,    QString("%1 ").arg("DRepet", 7));
	m_headerMap.insert(DrawsByMovesRule,    QString("%1 ").arg("DMoves", 7));
	m_headerMap.insert(DrawsByCountingRules,QString("%1 ").arg("DCount", 7));
	m_headerMap.insert(DrawsByAdjudication, QString("%1 ").arg("DAdj", 7));
	m_headerMap.insert(DrawsByAgreement,    QString("%1 ").arg("DAgree", 7));
	m_headerMap.insert(OtherDraws,          QString("%1 ").arg("OtherD", 7));
	m_headerMap.insert(TimeControlString,   QString("%1 ").arg("TC", 14));
}

Tournament::~Tournament()
{
	if (!m_gameData.isEmpty())
		qWarning("Tournament: Destroyed while games are still running.");

	qDeleteAll(m_gameData);
	qDeleteAll(m_pairs);

	QSet<const OpeningBook*> books;
	for (const TournamentPlayer& player : qAsConst(m_players))
	{
		books.insert(player.book());
		delete player.builder();
	}

	if (m_bookOwnership)
		qDeleteAll(books);

	delete m_openingSuite;
	delete m_sprt;

	if (m_pgnFile.isOpen())
		m_pgnFile.close();

	if (m_epdFile.isOpen())
		m_epdFile.close();
}

GameManager* Tournament::gameManager() const
{
	return m_gameManager;
}

bool Tournament::isFinished() const
{
	return m_finished;
}

QString Tournament::errorString() const
{
	return m_error;
}

QString Tournament::name() const
{
	return m_name;
}

QString Tournament::site() const
{
	return m_site;
}

QString Tournament::variant() const
{
	return m_variant;
}

int Tournament::currentRound() const
{
	return m_round;
}

int Tournament::gamesPerEncounter() const
{
	return m_gamesPerEncounter;
}

int Tournament::roundMultiplier() const
{
	return m_roundMultiplier;
}

int Tournament::finishedGameCount() const
{
	return m_finishedGameCount;
}

int Tournament::finalGameCount() const
{
	return m_finalGameCount;
}

const TournamentPlayer& Tournament::playerAt(int index) const
{
	return m_players.at(index);
}

int Tournament::playerCount() const
{
	return m_players.size();
}

int Tournament::seedCount() const
{
	return m_seedCount;
}

Sprt* Tournament::sprt() const
{
	return m_sprt;
}

bool Tournament::canSetRoundMultiplier() const
{
	return true;
}

void Tournament::setName(const QString& name)
{
	m_name = name;
}

void Tournament::setSite(const QString& site)
{
	m_site = site;
}

void Tournament::setVariant(const QString& variant)
{
	Q_ASSERT(Chess::BoardFactory::variants().contains(variant));
	m_variant = variant;
}

void Tournament::setCurrentRound(int round)
{
	Q_ASSERT(round >= 1);
	m_round = round;
}

int Tournament::gamesInProgress() const
{
	return m_nextGameNumber - m_finishedGameCount;
}

void Tournament::setGamesPerEncounter(int count)
{
	Q_ASSERT(count > 0);
	m_gamesPerEncounter = count;
}

void Tournament::setRoundMultiplier(int factor)
{
	Q_ASSERT(canSetRoundMultiplier());
	Q_ASSERT(factor > 0);
	m_roundMultiplier = factor;
}

void Tournament::setStartDelay(int delay)
{
	Q_ASSERT(delay >= 0);
	m_startDelay = delay;
}

void Tournament::setRecoveryMode(bool recover)
{
	m_recover = recover;
}

void Tournament::setAdjudicator(const GameAdjudicator& adjudicator)
{
	m_adjudicator = adjudicator;
}

void Tournament::setOpeningSuite(OpeningSuite *suite)
{
	delete m_openingSuite;
	m_openingSuite = suite;
}

void Tournament::setOpeningDepth(int plies)
{
	m_openingDepth = plies;
}

void Tournament::setSeedCount(int seedCount)
{
	m_seedCount = seedCount;
}

void Tournament::setSwapSides(bool enabled)
{
	m_swapSides = enabled;
}

void Tournament::setResultFormat(const QString& format)
{
	m_resultFormat = format;
}

QString Tournament::resultFormat() const
{
	return m_resultFormat;
}

const QMap<QString, QString> & Tournament::namedResultFormats() const
{
	return m_namedFormats;
}

const QList< QString > Tournament::resultFieldTokens() const
{
	return m_tokenMap.keys();
}

const QMap<QString,QString>& Tournament::resultFieldGroups() const
{
	return m_namedGroups;
}

void Tournament::setPgnOutput(const QString& fileName, PgnGame::PgnMode mode)
{
	if (fileName != m_pgnFile.fileName())
	{
		m_pgnFile.close();
		m_pgnFile.setFileName(fileName);
	}
	m_pgnOutMode = mode;
}

void Tournament::setPgnWriteUnfinishedGames(bool enabled)
{
	m_pgnWriteUnfinishedGames = enabled;
}

void Tournament::setPgnCleanupEnabled(bool enabled)
{
	m_pgnCleanup = enabled;
}

void Tournament::setEpdOutput(const QString& fileName)
{
	if (fileName != m_epdFile.fileName())
	{
		m_epdFile.close();
		m_epdFile.setFileName(fileName);
	}
}

void Tournament::setOpeningRepetitions(int count)
{
	m_openingRepetitions = count;
}

void Tournament::setOpeningPolicy(Tournament::OpeningPolicy policy)
{
	m_openingPolicy = policy;
}

void Tournament::setReverseSides(bool enabled)
{
	m_reverseSides = enabled;
}

void Tournament::setOpeningBookOwnership(bool enabled)
{
	m_bookOwnership = enabled;
}

void Tournament::addPlayer(PlayerBuilder* builder,
			   const TimeControl& timeControl,
			   const OpeningBook* book,
			   int bookDepth)
{
	Q_ASSERT(builder != nullptr);

	TournamentPlayer player(builder, timeControl, book, bookDepth);
	m_players.append(player);
}

TournamentPair* Tournament::currentPair() const
{
	return m_pair;
}

TournamentPair* Tournament::pair(int player1, int player2)
{
	Q_ASSERT(player1 || player2);

	const QPair<int,int> pairs[] = {
		qMakePair(player1, player2),
		qMakePair(player2, player1)
	};

	for (const auto& p: pairs)
	{
		if (m_pairs.contains(p))
			return m_pairs[p];
	}

	// Existing pair not found -> create a new one
	auto ret = new TournamentPair(player1, player2);
	m_pairs[pairs[0]] = ret;

	return ret;
}

bool Tournament::areAllGamesFinished() const
{
	return m_finishedGameCount >= m_finalGameCount;
}

bool Tournament::hasGauntletRatingsOrder() const
{
	return false;
}

void Tournament::startGame(TournamentPair* pair)
{
	Q_ASSERT(pair->isValid());
	m_pair = pair;
	m_pair->addStartedGame();

	const TournamentPlayer& white = m_players[m_pair->firstPlayer()];
	const TournamentPlayer& black = m_players[m_pair->secondPlayer()];

	Chess::Board* board = Chess::BoardFactory::create(m_variant);
	Q_ASSERT(board != nullptr);
	ChessGame* game = new ChessGame(board, new PgnGame());

	connect(game, SIGNAL(started(ChessGame*)),
		this, SLOT(onGameStarted(ChessGame*)));
	connect(game, SIGNAL(finished(ChessGame*)),
		this, SLOT(onGameFinished(ChessGame*)));

	game->setTimeControl(white.timeControl(), Chess::Side::White);
	game->setTimeControl(black.timeControl(), Chess::Side::Black);

	game->setOpeningBook(white.book(), Chess::Side::White, white.bookDepth());
	game->setOpeningBook(black.book(), Chess::Side::Black, black.bookDepth());

	if (!m_startFen.isEmpty() || !m_openingMoves.isEmpty())
	{
		game->setStartingFen(m_startFen);
		game->setMoves(m_openingMoves);
		m_startFen.clear();
		m_openingMoves.clear();
		m_repetitionCounter++;
	}
	else
	{
		m_repetitionCounter = 1;
		if (m_openingSuite != nullptr)
		{
			if (!game->setMoves(m_openingSuite->nextGame(m_openingDepth)))
				qWarning("The opening suite is incompatible with the "
				"current chess variant");
		}
	}

	game->generateOpening();
	if (m_repetitionCounter < m_openingRepetitions)
	{
		m_startFen = game->startingFen();
		if (m_startFen.isEmpty() && board->isRandomVariant())
		{
			m_startFen = board->defaultFenString();
			game->setStartingFen(m_startFen);
		}
		m_openingMoves = game->moves();
	}

	game->pgn()->setEvent(m_name);
	game->pgn()->setSite(m_site);
	game->pgn()->setRound(m_round);

	if (m_finishedGameCount > 0)
		game->setStartDelay(m_startDelay);
	game->setAdjudicator(m_adjudicator);

	GameData* data = new GameData;
	data->number = ++m_nextGameNumber;
	data->whiteIndex = m_pair->firstPlayer();
	data->blackIndex = m_pair->secondPlayer();
	m_gameData[game] = data;

	// Some tournament types may require more games than expected
	if (m_nextGameNumber > m_finalGameCount)
		m_finalGameCount = m_nextGameNumber;

	// Make sure the next game (if any) between the pair will
	// start with reversed colors.
	if (m_swapSides)
		m_pair->swapPlayers();

	auto whiteBuilder = white.builder();
	auto blackBuilder = black.builder();
	onGameAboutToStart(game, whiteBuilder, blackBuilder);
	connect(game, SIGNAL(startFailed(ChessGame*)),
		this, SLOT(onGameStartFailed(ChessGame*)));
	m_gameManager->newGame(game,
			       whiteBuilder,
			       blackBuilder,
			       GameManager::Enqueue,
			       GameManager::ReusePlayers);
}

void Tournament::onGameAboutToStart(ChessGame *game,
				    const PlayerBuilder* white,
				    const PlayerBuilder* black)
{
	Q_UNUSED(game);
	Q_UNUSED(white);
	Q_UNUSED(black);
}

int Tournament::playerIndex(ChessGame* game, Chess::Side side) const
{
	auto gd = m_gameData[game];
	return side == Chess::Side::White ? gd->whiteIndex : gd->blackIndex;
}

void Tournament::startNextGame()
{
	if (m_stopping)
		return;

	TournamentPair* pair(nextPair(m_nextGameNumber));
	if (!pair || !pair->isValid())
		return;

	bool samePlayers = pair->hasSamePlayers(m_pair);
	if (!samePlayers && m_reverseSides)
		pair->swapPlayers();

	if ((!samePlayers && m_players.size() > 2
	     && m_openingPolicy != OpeningPolicy::RoundPolicy)
	|| (m_round > m_oldRound
	     && m_openingPolicy == OpeningPolicy::RoundPolicy))
	{
		m_startFen.clear();
		m_openingMoves.clear();
		m_repetitionCounter = 1;
		m_oldRound = m_round;
	}

	startGame(pair);
}

inline bool faulty(const Chess::Result::Type& type)
{
	return type == Chess::Result::NoResult
	    || type == Chess::Result::ResultError
	    || type == Chess::Result::Disconnection
	    || type == Chess::Result::StalledConnection;
}

bool Tournament::writePgn(PgnGame* pgn, int gameNumber)
{
	Q_ASSERT(pgn != nullptr);
	Q_ASSERT(gameNumber > 0);

	if (m_pgnFile.fileName().isEmpty())
		return true;

	bool isOpen = m_pgnFile.isOpen();
	if (!isOpen || !m_pgnFile.exists())
	{
		if (isOpen)
		{
			qWarning("PGN file %s does not exist. Reopening...",
				 qUtf8Printable(m_pgnFile.fileName()));
			m_pgnFile.close();
		}

		if (!m_pgnFile.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			qWarning("Could not open PGN file %s",
				 qUtf8Printable(m_pgnFile.fileName()));
			return false;
		}
		m_pgnOut.setDevice(&m_pgnFile);
	}

	bool ok = true;
	m_pgnGames[gameNumber] = *pgn;
	while (m_pgnGames.contains(m_savedGameCount + 1))
	{
		PgnGame tmp = m_pgnGames.take(++m_savedGameCount);
		Chess::Result::Type type = tmp.result().type();
		if (!m_pgnWriteUnfinishedGames
		&&  (tmp.result().isNone() || (m_stopping && faulty(type))))
		{
			qWarning("Omitted incomplete game %d", m_savedGameCount);
			continue;
		}
		if (!tmp.write(m_pgnOut, m_pgnOutMode)
		||  m_pgnFile.error() != QFile::NoError)
		{
			ok = false;
			qWarning("Could not write PGN game %d", m_savedGameCount);
		}
	}

	return ok;
}

bool Tournament::writeEpd(ChessGame *game)
{
	Q_ASSERT(game != nullptr);

	if (m_epdFile.fileName().isEmpty())
		return true;

	bool isOpen = m_epdFile.isOpen();
	if (!isOpen || !m_epdFile.exists())
	{
		if (isOpen)
		{
			qWarning("EPD file %s does not exist. Reopening...",
				 qUtf8Printable(m_epdFile.fileName()));
			m_epdFile.close();
		}

		if (!m_epdFile.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			qWarning("Could not open EPD file %s",
				 qUtf8Printable(m_epdFile.fileName()));
			return false;
		}
		m_epdOut.setDevice(&m_epdFile);
	}

	const QString& epdPos = game->board()->fenString();
	m_epdOut << epdPos << "\n";
	m_epdOut.flush();
	bool ok = true;
	if (m_epdFile.error() != QFile::NoError)
	{
		ok = false;
		qWarning("Could not write EPD position");
	}

	return ok;
}

void Tournament::addScore(int player, Chess::Side side, int score)
{
	m_players[player].addScore(side, score);
}

void Tournament::addOutcome(int iWhite, int iBlack, Chess::Result result)
{
	Chess::Side winner {result.winner()};
	int type {result.type()};
	QString str {result.description()};
	int iWinner {winner == Chess::Side::White ? iWhite : iBlack};
	int iLoser {winner == Chess::Side::White ? iBlack : iWhite};
	using Type = Chess::Result::Type;

	// Draw, NoResult, ResultError, Agreement (to draw), Adjudication (draw)
	if (winner.isNull())
	{
		// Examine draw types
		if (type == Type::Draw)
		{
			if (str.contains(QObject::tr("stalemate")))
				type = AuxResultType::Stalemate;
			else if (str.contains(QObject::tr("material")))
				type = AuxResultType::InsufficientMaterial;
			else if (str.contains(QObject::tr("repetition")))
				type = AuxResultType::MoveRepetiton;
			else if (str.contains(QObject::tr("moves rule")))
				type = AuxResultType::MovesRule;
			else if (str.contains(QObject::tr("counting rules")))
				type = AuxResultType::CountingRules;
			else
				type = AuxResultType::OtherDraw;
		}

		m_players[iWhite].addOutcome(type, str);
		m_players[iBlack].addOutcome(type, str);
	}
	else if (type == Type::Adjudication
		&& str.contains(QObject::tr("result claim")))
	{
		// Invalid result claim is "other win" for winner
		m_players[iWinner].addOutcome(AuxResultType::OtherWin,
					      QString(tr("Win: ")) + str);
		m_players[iLoser].addOutcome(AuxResultType::InvalidResultClaim,
					     QString(tr("Loss: ")) + str);
	}
	else if (type == Type::Win
	     ||  type == Type::Agreement
	     ||  type == Type::Resignation
	     ||  type == Type::Adjudication)
	{
		// Regular win for winner and regular loss for loser
		// Count agreement, resignation, and adjudication as regular win/loss
		m_players[iWinner].addOutcome(AuxResultType::RegularWin,
					      QString(tr("Win: ")) + str);
		m_players[iLoser].addOutcome(AuxResultType::RegularLoss,
					     QString(tr("Loss: ")) + str);
	}
	else
	{
		// Game was terminated by other means
		m_players[iWinner].addOutcome(AuxResultType::OtherWin,
					      QString(tr("Win: ")) + str);
		m_players[iLoser].addOutcome(type, QString(tr("Loss: ")) + str);
		emit gameTerminated(iLoser, result);
	}
}

void Tournament::onGameStarted(ChessGame* game)
{
	Q_ASSERT(game != nullptr);
	Q_ASSERT(m_gameData.contains(game));

	GameData* data = m_gameData[game];
	int iWhite = data->whiteIndex;
	int iBlack = data->blackIndex;
	m_players[iWhite].setName(game->player(Chess::Side::White)->name());
	m_players[iBlack].setName(game->player(Chess::Side::Black)->name());

	emit gameStarted(game, data->number, iWhite, iBlack);
}

void Tournament::onGameFinished(ChessGame* game)
{
	Q_ASSERT(game != nullptr);

	PgnGame* pgn(game->pgn());

	m_finishedGameCount++;

	Q_ASSERT(m_gameData.contains(game));
	GameData* data = m_gameData.take(game);
	int gameNumber = data->number;
	Sprt::GameResult sprtResult = Sprt::NoResult;

	int iWhite = data->whiteIndex;
	int iBlack = data->blackIndex;
	const auto whiteName = pgn->playerName(Chess::Side::White);
	if (!whiteName.isEmpty())
		m_players[iWhite].setName(whiteName);
	const auto blackName = pgn->playerName(Chess::Side::Black);
	if (!blackName.isEmpty())
		m_players[iBlack].setName(blackName);

	switch (game->result().winner())
	{
	case Chess::Side::White:
		addScore(iWhite, Chess::Side::White, 2);
		addScore(iBlack, Chess::Side::Black, 0);
		sprtResult = (iWhite == 0) ? Sprt::Win : Sprt::Loss;
		break;
	case Chess::Side::Black:
		addScore(iBlack, Chess::Side::Black, 2);
		addScore(iWhite, Chess::Side::White, 0);
		sprtResult = (iBlack == 0) ? Sprt::Win : Sprt::Loss;
		break;
	default:
		if (game->result().isDraw())
		{
			addScore(iWhite,  Chess::Side::White, 1);
			addScore(iBlack,  Chess::Side::Black, 1);
			sprtResult = Sprt::Draw;
		}
		break;
	}

	writeEpd(game);
	writePgn(pgn, gameNumber);

	addOutcome(iWhite, iBlack, game->result());
	Chess::Result::Type resultType(game->result().type());

	bool crashed = (resultType == Chess::Result::Disconnection ||
			resultType == Chess::Result::StalledConnection);
	if (!m_recover && crashed)
		stop();

	if (!m_sprt->isNull() && sprtResult != Sprt::NoResult)
	{
		m_sprt->addGameResult(sprtResult);
		if (m_sprt->status().result != Sprt::Continue)
			QMetaObject::invokeMethod(this, "stop", Qt::QueuedConnection);
	}

	emit gameFinished(game, gameNumber, iWhite, iBlack);

	if (m_pgnCleanup)
		delete pgn;

	if (areAllGamesFinished() || (m_stopping && m_gameData.isEmpty()))
	{
		m_stopping = false;
		m_lastGame = game;
		connect(m_gameManager, SIGNAL(gameDestroyed(ChessGame*)),
			this, SLOT(onGameDestroyed(ChessGame*)));
	}

	delete data;
	game->deleteLater();
}

void Tournament::onGameDestroyed(ChessGame* game)
{
	if (game != m_lastGame)
		return;

	m_lastGame = nullptr;
	onFinished();
}

void Tournament::onGameStartFailed(ChessGame* game)
{
	m_error = game->errorString();

	delete game->pgn();
	game->deleteLater();
	m_gameData.remove(game);

	stop();
}

void Tournament::onFinished()
{
	m_gameManager->cleanupIdleThreads();
	m_finished = true;
	emit finished();
}

void Tournament::start()
{
	Q_ASSERT(m_players.size() > 1);

	m_round = 1;
	m_nextGameNumber = 0;
	m_finishedGameCount = 0;
	m_savedGameCount = 0;
	m_finalGameCount = 0;
	m_stopping = false;

	if (m_openingPolicy == EncounterPolicy || m_openingPolicy == RoundPolicy)
		setOpeningRepetitions(INT_MAX);

	m_gameData.clear();
	m_pgnGames.clear();
	m_startFen.clear();
	m_openingMoves.clear();

	connect(m_gameManager, SIGNAL(ready()),
		this, SLOT(startNextGame()));

	initializePairing();
	m_finalGameCount = gamesPerCycle() * gamesPerEncounter() * roundMultiplier();

	startNextGame();
}

void Tournament::stop()
{
	if (m_stopping)
		return;

	disconnect(m_gameManager, SIGNAL(ready()),
		   this, SLOT(startNextGame()));

	if (m_gameData.isEmpty())
	{
		onFinished();
		return;
	}

	m_stopping = true;
	const auto games = m_gameData.keys();
	for (ChessGame* game : games)
		QMetaObject::invokeMethod(game, "stop", Qt::QueuedConnection);
}

bool Tournament::isStopping() const
{
	return m_stopping;
}


QString Tournament::resultsForSides(int index) const
{
	QString ret;
	TournamentPlayer player = playerAt(index);
	int gamesWithWhite  = player.whiteWins() + player.whiteDraws()
			    + player.whiteLosses();
	int whiteScore  = 2 * player.whiteWins() + player.whiteDraws();
	int games = player.gamesFinished();

	if (gamesWithWhite > 0)
	{
		ret += tr("...      %1 playing White: %2 - %3 - %4  [%5] %6\n")
		.arg(qUtf8Printable(player.name()))
		.arg(player.whiteWins())
		.arg(player.whiteLosses())
		.arg(player.whiteDraws())
		.arg(double(whiteScore) / (gamesWithWhite * 2), 0, 'f', 3)
		.arg(gamesWithWhite);
	}

	int gamesWithBlack  = player.blackWins() + player.blackDraws()
			    + player.blackLosses();
	int blackScore  = 2 * player.blackWins() + player.blackDraws();

	if (gamesWithBlack > 0)
	{
		ret += tr("...      %1 playing Black: %2 - %3 - %4  [%5] %6\n")
		.arg(qUtf8Printable(player.name()))
		.arg(player.blackWins())
		.arg(player.blackLosses())
		.arg(player.blackDraws())
		.arg(double(blackScore) / (gamesWithBlack * 2), 0, 'f', 3)
		.arg(gamesWithBlack);
	}

	if (games > 0)
	{
		ret += tr("...      White vs Black: %1 - %2 - %3  [%4] %5\n")
		.arg(player.whiteWins() + player.blackLosses())
		.arg(player.whiteLosses() + player.blackWins())
		.arg(player.draws())
		.arg(double(whiteScore + 2 * gamesWithBlack - blackScore)
			    / (games * 2), 0, 'f', 3)
		.arg(games);
	}
	return ret;
}

QString Tournament::results() const
{
	QMultiMap<qreal, RankingData> ranking;
	QString ret;

	for (int i = 0; i < playerCount(); i++)
	{
		const TournamentPlayer& player(playerAt(i));
		Elo elo(player.wins(), player.losses(), player.draws());
		Elo whiteElo(player.whiteWins(),
			     player.whiteLosses(),
			     player.whiteDraws());
		Elo blackElo(player.blackWins(),
			     player.blackLosses(),
			     player.blackDraws());

		if (playerCount() == 2)
		{
			ret += resultsForSides(0);
			ret += QString("Elo difference: %1 +/- %2, LOS: %3 %, DrawRatio: %4 %")
				.arg(elo.diff(), 0, 'f', 1)
				.arg(elo.errorMargin(), 0, 'f', 1)
				.arg(elo.LOS(), 0, 'f', 1)
				.arg(elo.drawRatio() * 100, 0, 'f', 1);
			break;
		}

		RankingData data = { player.name(),
				     player.gamesFinished(),
				     player.wins(),
				     player.losses(),
				     player.draws(),
				     player.whiteWins(),
				     player.whiteLosses(),
				     player.whiteDraws(),
				     player.blackWins(),
				     player.blackLosses(),
				     player.blackDraws(),
				     player.score() / 2.,
				     elo.pointRatio(),
				     elo.drawRatio(),
				     elo.diff(),
				     elo.errorMargin(),
				     elo.LOS(),
				     whiteElo.pointRatio(),
				     whiteElo.drawRatio(),
				     whiteElo.diff(),
				     whiteElo.errorMargin(),
				     whiteElo.LOS(),
				     blackElo.pointRatio(),
				     blackElo.drawRatio(),
				     blackElo.diff(),
				     blackElo.errorMargin(),
				     blackElo.LOS(),
				     player.outcomes(Chess::Result::Timeout),
				     player.outcomes(Chess::Result::IllegalMove),
				     player.outcomes(Chess::Result::Disconnection),
				     player.outcomes(Chess::Result::StalledConnection),
				     player.outcomes(AuxResultType::InvalidResultClaim),
				     player.outcomes(AuxResultType::RegularLoss),
				     player.outcomes(AuxResultType::RegularWin),
				     player.outcomes(AuxResultType::OtherWin),
				     player.outcomes(AuxResultType::Stalemate),
				     player.outcomes(AuxResultType::InsufficientMaterial),
				     player.outcomes(AuxResultType::MoveRepetiton),
				     player.outcomes(AuxResultType::MovesRule),
				     player.outcomes(AuxResultType::CountingRules),
				     player.outcomes(Chess::Result::Adjudication),
				     player.outcomes(Chess::Result::Agreement),
				     player.outcomes(AuxResultType::OtherDraw),
				     player.timeControl().toString() };

		// Order players like this:
		// 1. Gauntlet player (if any)
		// 2. Players with finished games, sorted by point ratio
		// 3. Players without finished games
		qreal key = -1.0;
		if ((i > 0 && i >= seedCount()) || !hasGauntletRatingsOrder())
		{
			if (data.games)
				key = 1.0 - data.score;
			else
				key = 2.0;
		}
		ranking.insert(key, data);
	}

	//First assign raw format string, then try to find named format
	QString format = m_resultFormat;
	if (m_namedFormats.contains(m_resultFormat))
		format = m_namedFormats[m_resultFormat];

	for (auto it = m_namedGroups.cbegin(); it != m_namedGroups.cend(); it++)
		format.replace(it.key(), it.value());

	ResultFormatter formatter(m_tokenMap, format);

	if (!ranking.isEmpty())
		ret += formatter.entry(m_headerMap);

	int rank = hasGauntletRatingsOrder() ? -1 : 0;
	for (auto it = ranking.constBegin(); it != ranking.constEnd(); ++it)
	{
		const RankingData& data = it.value();
		QMap<int, QString> dataMap;
		dataMap.insert(Rank,       QString("%1 ").arg(++rank, 4));
		dataMap.insert(Name,       QString("%1 ").arg(data.name, -25));
		dataMap.insert(EloDiff,    QString("%1 ").arg(data.eloDiff, 7, 'f', 0));
		dataMap.insert(ErrorMargin,QString("%1 ").arg(data.errorMargin, 7, 'f', 0));
		dataMap.insert(Games,      QString("%1 ").arg(data.games, 7));
		dataMap.insert(Wins,       QString("%1 ").arg(data.wins, 7));
		dataMap.insert(Losses,     QString("%1 ").arg(data.losses, 7));
		dataMap.insert(Draws,      QString("%1 ").arg(data.draws, 7));
		dataMap.insert(Points,     QString("%1 ").arg(data.points, 8,'f',1));
		dataMap.insert(Score,      QString("%1% ").arg(data.score * 100.0, 6, 'f', 1));
		dataMap.insert(DrawScore,  QString("%1% ").arg(data.drawScore * 100.0, 6, 'f', 1));
		dataMap.insert(WhiteScore, QString("%1% ").arg(data.whiteScore * 100.0, 6, 'f', 1));
		dataMap.insert(BlackScore, QString("%1% ").arg(data.blackScore * 100.0, 6, 'f', 1));
		dataMap.insert(WhiteDrawScore,
			       QString("%1% ").arg(data.whiteDrawScore * 100.0, 6, 'f', 1));
		dataMap.insert(BlackDrawScore,
			       QString("%1% ").arg(data.blackDrawScore * 100.0, 6, 'f', 1));
		dataMap.insert(WhiteWins,  QString("%1 ").arg(data.whiteWins, 7));
		dataMap.insert(WhiteLosses,QString("%1 ").arg(data.whiteLosses, 7));
		dataMap.insert(WhiteDraws, QString("%1 ").arg(data.whiteDraws, 7));
		dataMap.insert(BlackWins,  QString("%1 ").arg(data.blackWins, 7));
		dataMap.insert(BlackLosses,QString("%1 ").arg(data.blackLosses, 7));
		dataMap.insert(BlackDraws, QString("%1 ").arg(data.blackDraws, 7));
		dataMap.insert(TimeForfeits, QString("%1 ").arg(data.timeForfeits, 7));
		dataMap.insert(IllegalMoves, QString("%1 ").arg(data.illegalMoves, 7));
		dataMap.insert(Disconnections, QString("%1 ").arg(data.disconnections, 7));
		dataMap.insert(StalledConnections, QString("%1 ").arg(data.stalledConnections, 7));
		dataMap.insert(InvalidResultClaims, QString("%1 ").arg(data.invalidResultClaims, 7));
		dataMap.insert(RegularLosses, QString("%1 ").arg(data.regularLosses, 7));
		dataMap.insert(RegularWins, QString("%1 ").arg(data.regularWins, 7));
		dataMap.insert(OtherWins,  QString("%1 ").arg(data.otherWins, 7));
		dataMap.insert(DrawsByStalemate, QString("%1 ").arg(data.drawsByStalemate, 7));
		dataMap.insert(DrawsByMaterial, QString("%1 ").arg(data.drawsByMaterial, 7));
		dataMap.insert(DrawsByRepetiton, QString("%1 ").arg(data.drawsByRepetiton, 7));
		dataMap.insert(DrawsByMovesRule, QString("%1 ").arg(data.drawsByMovesRule, 7));
		dataMap.insert(DrawsByCountingRules,
			       QString("%1 ").arg(data.drawsByCountingRules, 7));
		dataMap.insert(DrawsByAdjudication,
			       QString("%1 ").arg(data.drawsByAdjudication, 7));
		dataMap.insert(DrawsByAgreement,
			       QString("%1 ").arg(data.drawsByAgreement, 7));
		dataMap.insert(OtherDraws, QString("%1 ").arg(data.otherDraws, 7));
		dataMap.insert(TimeControlString, QString("%1 ").arg(data.timeControl, 14));


		ret += formatter.entry(dataMap);
	}

	Sprt::Status sprtStatus = sprt()->status();
	if (sprtStatus.llr != 0.0
	||  sprtStatus.lBound != 0.0
	||  sprtStatus.uBound != 0.0)
	{
		QString sprtStr = QString("SPRT: llr %1 (%2%), lbound %3, ubound %4")
			.arg(sprtStatus.llr, 0, 'g', 3)
			.arg(sprtStatus.llr / sprtStatus.uBound * 100, 0, 'f', 1)
			.arg(sprtStatus.lBound, 0, 'g', 3)
			.arg(sprtStatus.uBound, 0, 'g', 3);
		if (sprtStatus.result == Sprt::AcceptH0)
			sprtStr.append(" - H0 was accepted");
		else if (sprtStatus.result == Sprt::AcceptH1)
			sprtStr.append(" - H1 was accepted");

		ret += "\n" + sprtStr;
	}

	return ret;
}

QString Tournament::outcomes() const
{
	QString ret;
	for (auto player: m_players)
	{
		ret += QString("\nPlayer: %0").arg(player.name());
		const auto& map(player.outcomeMap());
		// sorted listing
		for (const QString& str: map.keys())
			ret += QString("\n   \"%0\": %1")
				.arg(str)
				.arg(map[str]);
	}
	return ret;
}

ResultFormatter::ResultFormatter::ResultFormatter(const QMap<QString, int>& tokenMap,
						  const QString& format,
						  QObject* parent)
	: QObject(parent),
	  m_tokenMap(tokenMap)
{
	setEntryFormat(format);
}

void ResultFormatter::setEntryFormat(const QString& format)
{
	m_entryFormat = format;
	m_tokenList = m_entryFormat.split(',', Qt::SkipEmptyParts);
	m_tokenList.replaceInStrings(" ","");
}

QString ResultFormatter::entryFormat() const
{
	return m_entryFormat;
}

QString ResultFormatter::entry(const QMap<int, QString>& data) const
{
	QString ret;
	for (const QString& token: qAsConst(m_tokenList))
	{
		int key = m_tokenMap.value(token, -1);
		if (key >= 0)
			ret.append(data.value(key));
	}
	ret.append("\n");
	return ret;
}


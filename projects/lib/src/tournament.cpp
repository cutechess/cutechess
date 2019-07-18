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
	  m_recover(false),
	  m_pgnCleanup(true),
	  m_pgnWriteUnfinishedGames(true),
	  m_finished(false),
	  m_bookOwnership(false),
	  m_openingSuite(nullptr),
	  m_sprt(new Sprt),
	  m_repetitionCounter(0),
	  m_swapSides(true),
	  m_pgnOutMode(PgnGame::Verbose),
	  m_pair(nullptr)
{
	Q_ASSERT(gameManager != nullptr);
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

void Tournament::setSwapSides(bool enabled)
{
	m_swapSides = enabled;
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

	if (!pair->hasSamePlayers(m_pair) && m_players.size() > 2)
	{
		m_startFen.clear();
		m_openingMoves.clear();
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

void Tournament::addScore(int player, int score)
{
	m_players[player].addScore(score);
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
		addScore(iWhite, 2);
		addScore(iBlack, 0);
		sprtResult = (iWhite == 0) ? Sprt::Win : Sprt::Loss;
		break;
	case Chess::Side::Black:
		addScore(iBlack, 2);
		addScore(iWhite, 0);
		sprtResult = (iBlack == 0) ? Sprt::Win : Sprt::Loss;
		break;
	default:
		if (game->result().isDraw())
		{
			addScore(iWhite, 1);
			addScore(iBlack, 1);
			sprtResult = Sprt::Draw;
		}
		break;
	}

	writeEpd(game);
	writePgn(pgn, gameNumber);

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

QString Tournament::results() const
{
	QMultiMap<qreal, RankingData> ranking;
	QString ret;

	for (int i = 0; i < playerCount(); i++)
	{
		const TournamentPlayer& player(playerAt(i));
		Elo elo(player.wins(), player.losses(), player.draws());

		if (playerCount() == 2)
		{
			ret += QString("Elo difference: %1 +/- %2, LOS: %3 %, DrawRatio: %4 %")
				.arg(elo.diff(), 0, 'f', 1)
				.arg(elo.errorMargin(), 0, 'f', 1)
				.arg(elo.LOS(), 0, 'f', 1)
				.arg(elo.drawRatio() * 100, 0, 'f', 1);
			break;
		}

		RankingData data = { player.name(),
				     player.gamesFinished(),
				     elo.pointRatio(),
				     elo.drawRatio(),
				     elo.errorMargin(),
				     elo.diff() };
		// Order players like this:
		// 1. Gauntlet player (if any)
		// 2. Players with finished games, sorted by point ratio
		// 3. Players without finished games
		qreal key = -1.0;
		if (i > 0 || !hasGauntletRatingsOrder())
		{
			if (data.games)
				key = 1.0 - data.score;
			else
				key = 2.0;
		}
		ranking.insert(key, data);
	}

	if (!ranking.isEmpty())
		ret += QString("%1 %2 %3 %4 %5 %6 %7")
			.arg("Rank", 4)
			.arg("Name", -25)
			.arg("Elo", 7)
			.arg("+/-", 7)
			.arg("Games", 7)
			.arg("Score", 7)
			.arg("Draws", 7);

	int rank = hasGauntletRatingsOrder() ? -1 : 0;
	for (auto it = ranking.constBegin(); it != ranking.constEnd(); ++it)
	{
		const RankingData& data = it.value();
		ret += QString("\n%1 %2 %3 %4 %5 %6% %7%")
			.arg(++rank, 4)
			.arg(data.name, -25)
			.arg(data.eloDiff, 7, 'f', 0)
			.arg(data.errorMargin, 7, 'f', 0)
			.arg(data.games, 7)
			.arg(data.score * 100.0, 6, 'f', 1)
			.arg(data.draws * 100.0, 6, 'f', 1);
	}

	Sprt::Status sprtStatus = sprt()->status();
	if (sprtStatus.llr != 0.0
	||  sprtStatus.lBound != 0.0
	||  sprtStatus.uBound != 0.0)
	{
		QString sprtStr = QString("SPRT: llr %1 (%2\%), lbound %3, ubound %4")
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

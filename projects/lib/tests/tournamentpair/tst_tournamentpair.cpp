#include <QtTest/QtTest>
#include <tournamentpair.h>

class tst_TournamentPair: public QObject
{
	Q_OBJECT

	private slots:
		void initialValues();
		void hasOriginalOrder();
		void leader();
		void isValid();
		void hasSamePlayers();
		void gameStats();
		void swapPlayers();
};

void tst_TournamentPair::initialValues()
{
	TournamentPair pair;
	QCOMPARE(pair.firstPlayer(), -1);
	QCOMPARE(pair.secondPlayer(), -1);
	QCOMPARE(pair.firstScore(), 0);
	QCOMPARE(pair.secondScore(), 0);
	QCOMPARE(pair.gamesFinished(), 0);
	QCOMPARE(pair.gamesInProgress(), 0);
	QCOMPARE(pair.gamesStarted(), 0);
	QVERIFY(!pair.isValid());
	QCOMPARE(pair.leader(), -1);
	QCOMPARE(pair.scoreDiff(), 0);
	QCOMPARE(pair.scoreSum(), 0);
}

void tst_TournamentPair::hasOriginalOrder()
{
	TournamentPair pair(1, 2);
	QVERIFY(pair.hasOriginalOrder());
	pair.swapPlayers();
	QVERIFY(!pair.hasOriginalOrder());
	pair.swapPlayers();
	QVERIFY(pair.hasOriginalOrder());
}

void tst_TournamentPair::leader()
{
	TournamentPair pair;
	QCOMPARE(pair.leader(), -1);

	pair = TournamentPair(1, 2);
	QCOMPARE(pair.leader(), -1);
	pair.addFirstScore(2);
	QCOMPARE(pair.leader(), 1);
	pair.addSecondScore(2);
	QCOMPARE(pair.leader(), -1);
	pair.addSecondScore(1);
	QCOMPARE(pair.leader(), 2);
}

void tst_TournamentPair::isValid()
{
	TournamentPair pair;
	QVERIFY(!pair.isValid());

	pair = TournamentPair(1, -1);
	QVERIFY(!pair.isValid());

	pair = TournamentPair(1, 2);
	QVERIFY(pair.isValid());
}

void tst_TournamentPair::hasSamePlayers()
{
	TournamentPair pair;
	TournamentPair other;
	QVERIFY(pair.hasSamePlayers(&other));
	QVERIFY(!pair.hasSamePlayers(0));

	pair = TournamentPair(1, 2);
	other = TournamentPair(2, 3);
	QVERIFY(!pair.hasSamePlayers(&other));

	other = TournamentPair(2, 1);
	QVERIFY(pair.hasSamePlayers(&other));
}

void tst_TournamentPair::gameStats()
{
	TournamentPair pair(1, 2);
	QCOMPARE(pair.gamesStarted(), 0);
	QCOMPARE(pair.gamesInProgress(), 0);
	QCOMPARE(pair.gamesFinished(), 0);
	QCOMPARE(pair.firstScore(), 0);
	QCOMPARE(pair.secondScore(), 0);
	QCOMPARE(pair.scoreSum(), 0);
	QCOMPARE(pair.scoreDiff(), 0);

	pair.addStartedGame();
	pair.addStartedGame();
	pair.addStartedGame();
	pair.addFirstScore(2);
	pair.addFirstScore(1);
	pair.addSecondScore(1);
	QCOMPARE(pair.gamesStarted(), 3);
	QCOMPARE(pair.gamesInProgress(), 1);
	QCOMPARE(pair.gamesFinished(), 2);
	QCOMPARE(pair.firstScore(), 3);
	QCOMPARE(pair.secondScore(), 1);
	QCOMPARE(pair.scoreSum(), 4);
	QCOMPARE(pair.scoreDiff(), 2);
}

void tst_TournamentPair::swapPlayers()
{
	TournamentPair pair(1, 2);
	QCOMPARE(pair.firstPlayer(), 1);
	QCOMPARE(pair.secondPlayer(), 2);
	pair.swapPlayers();
	QCOMPARE(pair.firstPlayer(), 2);
	QCOMPARE(pair.secondPlayer(), 1);
}

QTEST_MAIN(tst_TournamentPair)
#include "tst_tournamentpair.moc"

#include <QtTest/QtTest>
#include <tournamentplayer.h>
#include <openingbook.h>

class MockOpeningBook: public OpeningBook
{
	protected:
		virtual Entry readEntry(QDataStream& in, quint64* key) const
		{
			Q_UNUSED(in);
			Q_UNUSED(key);

			Entry entry;
			return entry;
		}
		virtual void writeEntry(const Map::const_iterator& it,
					QDataStream& out) const
		{
			Q_UNUSED(it);
			Q_UNUSED(out);
		}
		virtual int entrySize() const
		{
			return 16;
		}
};

class MockPlayerBuilder: public PlayerBuilder
{
	public:
		MockPlayerBuilder(const QString& name)
			: PlayerBuilder(name)
		{
		}

		virtual bool isHuman() const
		{
			return false;
		}

		virtual ChessPlayer* create(QObject* receiver,
					    const char* method,
					    QObject* parent,
					    QString* error) const
		{
			Q_UNUSED(receiver);
			Q_UNUSED(method);
			Q_UNUSED(parent);
			Q_UNUSED(error);

			return 0;
		}
};

class tst_TournamentPlayer: public QObject
{
	Q_OBJECT

	public:
		tst_TournamentPlayer();

	private slots:
		void initialValues();
		void setName();
		void addScore();

		void cleanupTestCase();

	private:
		TimeControl m_tc;
		PlayerBuilder* m_builder;
		OpeningBook* m_book;
		TournamentPlayer* m_player;
};

tst_TournamentPlayer::tst_TournamentPlayer()
{
	m_tc = TimeControl("10+1");
	m_builder = new MockPlayerBuilder("orig_name");
	m_book = new MockOpeningBook();
	m_player = new TournamentPlayer(m_builder, m_tc, m_book, 10);
}

void tst_TournamentPlayer::cleanupTestCase()
{
	delete m_player;
	delete m_builder;
	delete m_book;
}

void tst_TournamentPlayer::initialValues()
{
	QCOMPARE(m_player->book(), m_book);
	QCOMPARE(m_player->bookDepth(), 10);
	QCOMPARE(m_player->builder(), m_builder);
	QCOMPARE(m_player->name(), QString("orig_name"));
	QCOMPARE(m_player->timeControl(), m_tc);
	QCOMPARE(m_player->wins(), 0);
	QCOMPARE(m_player->draws(), 0);
	QCOMPARE(m_player->losses(), 0);
	QCOMPARE(m_player->gamesFinished(), 0);
	QCOMPARE(m_player->whiteWins(), 0);
	QCOMPARE(m_player->whiteDraws(), 0);
	QCOMPARE(m_player->whiteLosses(), 0);
	QCOMPARE(m_player->blackWins(), 0);
	QCOMPARE(m_player->blackDraws(), 0);
	QCOMPARE(m_player->blackLosses(), 0);
}

void tst_TournamentPlayer::setName()
{
	m_player->setName("new_name");
	QCOMPARE(m_player->name(), QString("new_name"));
}

void tst_TournamentPlayer::addScore()
{
	QCOMPARE(m_player->gamesFinished(), 0);

	m_player->addScore(Chess::Side::White, 0);
	QCOMPARE(m_player->gamesFinished(), 1);
	QCOMPARE(m_player->score(), 0);
	QCOMPARE(m_player->draws(), 0);
	QCOMPARE(m_player->wins(), 0);
	QCOMPARE(m_player->losses(), 1);
	QCOMPARE(m_player->draws(), 0);
	QCOMPARE(m_player->whiteWins(), 0);
	QCOMPARE(m_player->whiteLosses(), 1);
	QCOMPARE(m_player->whiteDraws(), 0);
	QCOMPARE(m_player->blackWins(), 0);
	QCOMPARE(m_player->blackLosses(), 0);
	QCOMPARE(m_player->blackDraws(), 0);

	m_player->addScore(Chess::Side::White, 1);
	QCOMPARE(m_player->gamesFinished(), 2);
	QCOMPARE(m_player->score(), 1);
	QCOMPARE(m_player->draws(), 1);
	QCOMPARE(m_player->wins(), 0);
	QCOMPARE(m_player->losses(), 1);
	QCOMPARE(m_player->whiteWins(), 0);
	QCOMPARE(m_player->whiteLosses(), 1);
	QCOMPARE(m_player->whiteDraws(), 1);
	QCOMPARE(m_player->blackWins(), 0);
	QCOMPARE(m_player->blackLosses(), 0);
	QCOMPARE(m_player->blackDraws(), 0);

	m_player->addScore(Chess::Side::White, 2);
	QCOMPARE(m_player->gamesFinished(), 3);
	QCOMPARE(m_player->score(), 3);
	QCOMPARE(m_player->draws(), 1);
	QCOMPARE(m_player->wins(), 1);
	QCOMPARE(m_player->losses(), 1);
	QCOMPARE(m_player->whiteWins(), 1);
	QCOMPARE(m_player->whiteLosses(), 1);
	QCOMPARE(m_player->whiteDraws(), 1);
	QCOMPARE(m_player->blackWins(), 0);
	QCOMPARE(m_player->blackLosses(), 0);
	QCOMPARE(m_player->blackDraws(), 0);

	m_player->addScore(Chess::Side::Black, 0);
	QCOMPARE(m_player->gamesFinished(), 4);
	QCOMPARE(m_player->score(), 3);
	QCOMPARE(m_player->draws(), 1);
	QCOMPARE(m_player->wins(), 1);
	QCOMPARE(m_player->losses(), 2);
	QCOMPARE(m_player->whiteWins(), 1);
	QCOMPARE(m_player->whiteLosses(), 1);
	QCOMPARE(m_player->whiteDraws(), 1);
	QCOMPARE(m_player->blackWins(), 0);
	QCOMPARE(m_player->blackLosses(), 1);
	QCOMPARE(m_player->blackDraws(), 0);

	m_player->addScore(Chess::Side::Black, 1);
	QCOMPARE(m_player->gamesFinished(), 5);
	QCOMPARE(m_player->score(), 4);
	QCOMPARE(m_player->draws(), 2);
	QCOMPARE(m_player->wins(), 1);
	QCOMPARE(m_player->losses(), 2);
	QCOMPARE(m_player->whiteWins(), 1);
	QCOMPARE(m_player->whiteLosses(), 1);
	QCOMPARE(m_player->whiteDraws(), 1);
	QCOMPARE(m_player->blackWins(), 0);
	QCOMPARE(m_player->blackLosses(), 1);
	QCOMPARE(m_player->blackDraws(), 1);

	m_player->addScore(Chess::Side::Black, 2);
	QCOMPARE(m_player->gamesFinished(), 6);
	QCOMPARE(m_player->score(), 6);
	QCOMPARE(m_player->draws(), 2);
	QCOMPARE(m_player->wins(), 2);
	QCOMPARE(m_player->losses(), 2);
	QCOMPARE(m_player->whiteWins(), 1);
	QCOMPARE(m_player->whiteLosses(), 1);
	QCOMPARE(m_player->whiteDraws(), 1);
	QCOMPARE(m_player->blackWins(), 1);
	QCOMPARE(m_player->blackLosses(), 1);
	QCOMPARE(m_player->blackDraws(), 1);
}

QTEST_MAIN(tst_TournamentPlayer)
#include "tst_tournamentplayer.moc"

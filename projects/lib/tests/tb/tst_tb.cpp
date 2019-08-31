#include <QtTest/QtTest>
#include <QDir>
#include <board/standardboard.h>
#include <board/syzygytablebase.h>


class tst_Tb: public QObject
{
	Q_OBJECT

	private slots:
		void initTestCase();
		
		void tbInitialized();
		
		void positions_data() const;
		void positions();
		
		void cleanupTestCase();
		
	private:
		Chess::StandardBoard m_board;
};


void tst_Tb::initTestCase()
{
	const auto path = QLatin1String("tb_path");
	QDir dir(path);
	if (!dir.exists())
		QSKIP("Syzygy tablebases not available");

	SyzygyTablebase::initialize(path);
}

void tst_Tb::cleanupTestCase()
{
}

void tst_Tb::tbInitialized()
{
	QVERIFY2(SyzygyTablebase::tbAvailable(3),
	         "3-piece tablebases unavailable");
	QVERIFY2(SyzygyTablebase::tbAvailable(4),
	         "4-piece tablebases unavailable");
}

void tst_Tb::positions_data() const
{
	QTest::addColumn<QString>("fen");
	QTest::addColumn<QString>("result");
	QTest::addColumn<int>("dtz");
	
	QTest::newRow("startpos")
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "*"
		<< 0;
	QTest::newRow("pos2")
		<< "7k/8/8/8/5KP1/8/8/8 w - - 0 1"
		<< "1-0"
		<< 7;
	QTest::newRow("pos3")
		<< "7k/8/8/6P1/5K2/8/8/8 w - - 0 1"
		<< "1/2-1/2"
		<< 0;
	QTest::newRow("pos4")
		<< "8/2k5/8/6N1/5K2/1r6/8/8 w - - 0 1"
		<< "1/2-1/2"
		<< 0;
	QTest::newRow("pos5")
		<< "1n6/8/8/8/8/8/6R1/2K1k3 w - - 0 1"
		<< "1-0"
		<< 48;
	QTest::newRow("pos6")
		<< "3n4/8/8/8/7R/8/8/2K1k3 w - - 0 1"
		<< "1-0"
		<< 48;
	QTest::newRow("pos7")
		<< "8/8/3n4/8/8/8/4R3/2K2k2 w - - 0 1"
		<< "1-0"
		<< 44;
	QTest::newRow("pos8")
		<< "8/8/7R/n7/8/8/8/2K2k2 w - - 0 1"
		<< "1-0"
		<< 44;
	QTest::newRow("pos9")
		<< "4n3/8/8/8/7R/8/8/3K1k2 w - - 0 1"
		<< "1-0"
		<< 46;
	QTest::newRow("pos10")
		<< "2B5/8/8/8/8/2K2k2/6p1/8 b - - 0 1"
		<< "0-1"
		<< 1;
	QTest::newRow("pos11")
		<< "8/B7/8/8/8/2K2k2/6p1/8 b - - 0 1"
		<< "1/2-1/2"
		<< 0;
	QTest::newRow("pos12")
		<< "2K4N/8/8/8/7p/5k2/8/8 w - - 0 1"
		<< "0-1"
		<< 4;
	QTest::newRow("pos13")	// TCEC9 superfinal game 17.
		<< "K5Q1/8/8/8/5bb1/6k1/8/8 b - - 0 72"
		<< "1/2-1/2"
		<< 124;
}

void tst_Tb::positions()
{
	QFETCH(QString, fen);
	QFETCH(QString, result);
	QFETCH(int, dtz);

	QVERIFY(m_board.setFenString(fen));
	
	unsigned int tbDtz = 0;
	QCOMPARE(m_board.tablebaseResult(&tbDtz).toShortString(), result);
	QCOMPARE(int(tbDtz), dtz);
}

QTEST_MAIN(tst_Tb)
#include "tst_tb.moc"

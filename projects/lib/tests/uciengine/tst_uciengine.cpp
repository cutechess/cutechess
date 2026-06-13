#include <QtTest/QTest>
#include <uciengine.h>
#include <board/board.h>
#include <board/boardfactory.h>
#include <moveevaluation.h>
#include <enginebuttonoption.h>
#include <enginecheckoption.h>
#include <enginespinoption.h>
#include <enginetextoption.h>
#include <enginecombooption.h>

class tst_UciEngine: public UciEngine
{
	Q_OBJECT

	public:
		tst_UciEngine();

	private slots:
		void testParseInfo_data();
		void testParseInfo();
		void testParseOptionButton_data();
		void testParseOptionButton();
		void testParseOptionCheck_data();
		void testParseOptionCheck();
		void testParseOptionSpin_data();
		void testParseOptionSpin();
		void testParseOptionString_data();
		void testParseOptionString();
		void testParseOptionCombo_data();
		void testParseOptionCombo();
};

tst_UciEngine::tst_UciEngine()
    : UciEngine()
{
}

void tst_UciEngine::testParseInfo_data()
{
	QTest::addColumn<QString>("infoString");
	QTest::addColumn<MoveEvaluation>("eval");

	// "string" info is not parsed and yields an empty evaluation
	QTest::newRow("string")
	    << "info string NNUE evaluation using nn-5af11540bbfe.nnue enabled"
	    << MoveEvaluation();

	MoveEvaluation eval1;
	eval1.setDepth(1);
	eval1.setSelectiveDepth(1);
	eval1.setPvNumber(1);
	eval1.setScore(2);
	eval1.setNodeCount(20);
	eval1.setNps(20000);
	eval1.setHashUsage(0);
	eval1.setTbHits(0);
	eval1.setTime(1);
	eval1.setPv("Nf3");
	QTest::newRow("depth 1")
	    << "info depth 1 seldepth 1 multipv 1 score cp 2 nodes 20 "
	       "nps 20000 hashfull 0 tbhits 0 time 1 pv g1f3"
	    << eval1;

	MoveEvaluation eval2;
	eval2.setDepth(5);
	eval2.setSelectiveDepth(3);
	eval2.setPvNumber(1);
	eval2.setScore(42);
	eval2.setNodeCount(131);
	eval2.setNps(131000);
	eval2.setHashUsage(0);
	eval2.setTbHits(0);
	eval2.setTime(1);
	eval2.setPv("e4 Nf6");
	QTest::newRow("short pv")
	    << "info depth 5 seldepth 3 multipv 1 score cp 42 nodes 131 "
	       "nps 131000 hashfull 0 tbhits 0 time 1 pv e2e4 g8f6"
	    << eval2;

	MoveEvaluation eval3;
	eval3.setDepth(9);
	eval3.setSelectiveDepth(8);
	eval3.setPvNumber(1);
	eval3.setScore(48);
	eval3.setNodeCount(4500);
	eval3.setNps(500000);
	eval3.setHashUsage(1);
	eval3.setTbHits(0);
	eval3.setTime(9);
	eval3.setPv("e4 e5 Nf3 Nf6 Nxe5 Nxe4 d4 Nc6");
	QTest::newRow("long pv")
	    << "info depth 9 seldepth 8 multipv 1 score cp 48 nodes 4500 "
	       "nps 500000 hashfull 1 tbhits 0 time 9 "
	       "pv e2e4 e7e5 g1f3 g8f6 f3e5 f6e4 d2d4 b8c6"
	    << eval3;

	// Mate scores: a positive mate in N is mapped close to MATE_SCORE
	MoveEvaluation eval4;
	eval4.setDepth(20);
	eval4.setSelectiveDepth(25);
	eval4.setPvNumber(1);
	eval4.setScore(MoveEvaluation::MATE_SCORE + 1 - 5 * 2);
	eval4.setNodeCount(1000);
	eval4.setNps(1000);
	eval4.setHashUsage(0);
	eval4.setTbHits(0);
	eval4.setTime(100);
	eval4.setPv("e4");
	QTest::newRow("mate")
	    << "info depth 20 seldepth 25 multipv 1 score mate 5 nodes 1000 "
	       "nps 1000 hashfull 0 tbhits 0 time 100 pv e2e4"
	    << eval4;

	// A negative mate (getting mated) maps close to -MATE_SCORE
	MoveEvaluation eval5;
	eval5.setDepth(20);
	eval5.setSelectiveDepth(25);
	eval5.setPvNumber(1);
	eval5.setScore(-MoveEvaluation::MATE_SCORE - (-2) * 2);
	eval5.setNodeCount(1000);
	eval5.setNps(1000);
	eval5.setHashUsage(0);
	eval5.setTbHits(0);
	eval5.setTime(100);
	eval5.setPv("e4");
	QTest::newRow("mated")
	    << "info depth 20 seldepth 25 multipv 1 score mate -2 nodes 1000 "
	       "nps 1000 hashfull 0 tbhits 0 time 100 pv e2e4"
	    << eval5;

	// Secondary PVs (multipv > 1) are parsed the same way
	MoveEvaluation eval6;
	eval6.setDepth(5);
	eval6.setSelectiveDepth(3);
	eval6.setPvNumber(2);
	eval6.setScore(30);
	eval6.setNodeCount(200);
	eval6.setNps(200000);
	eval6.setHashUsage(0);
	eval6.setTbHits(0);
	eval6.setTime(1);
	eval6.setPv("d4");
	QTest::newRow("multipv 2")
	    << "info depth 5 seldepth 3 multipv 2 score cp 30 nodes 200 "
	       "nps 200000 hashfull 0 tbhits 0 time 1 pv d2d4"
	    << eval6;
}

void tst_UciEngine::testParseInfo()
{
	QFETCH(QString, infoString);
	QFETCH(MoveEvaluation, eval);

	// parseInfo() converts the PV to SAN, which requires a board
	Chess::Board* board = Chess::BoardFactory::create("standard");
	QVERIFY(board != nullptr);
	QVERIFY(board->setFenString(board->defaultFenString()));
	setBoard(board);

	QCOMPARE(parseInfo(firstToken(infoString)), eval);

	setBoard(nullptr);
	delete board;
}

void tst_UciEngine::testParseOptionButton_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");

	QTest::newRow("button 1")
	    << "option name Clear Hash type button"
	    << "Clear Hash";
}

void tst_UciEngine::testParseOptionButton()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);

	auto opt = dynamic_cast<EngineButtonOption*>(parseOption(firstToken(optionString)));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	delete opt;
}

void tst_UciEngine::testParseOptionCheck_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<bool>("value");

	QTest::newRow("check false")
	    << "option name Ponder type check default false"
	    << "Ponder"
	    << false;
	QTest::newRow("check true")
	    << "option name Some Option type check default true"
	    << "Some Option"
	    << true;
}

void tst_UciEngine::testParseOptionCheck()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(bool, value);

	auto opt = dynamic_cast<EngineCheckOption*>(parseOption(firstToken(optionString)));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	delete opt;
}

void tst_UciEngine::testParseOptionSpin_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<int>("value");
	QTest::addColumn<int>("min");
	QTest::addColumn<int>("max");

	QTest::newRow("spin Hash")
	    << "option name Hash type spin default 16 min 1 max 33554432"
	    << "Hash"
	    << 16
	    << 1
	    << 33554432;
	QTest::newRow("spin Threads")
	    << "option name Threads type spin default 1 min 1 max 1024"
	    << "Threads"
	    << 1
	    << 1
	    << 1024;
	QTest::newRow("spin Skill Level")
	    << "option name Skill Level type spin default 20 min 0 max 20"
	    << "Skill Level"
	    << 20
	    << 0
	    << 20;
}

void tst_UciEngine::testParseOptionSpin()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(int, value);
	QFETCH(int, min);
	QFETCH(int, max);

	auto opt = dynamic_cast<EngineSpinOption*>(parseOption(firstToken(optionString)));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	QCOMPARE(opt->min(), QVariant(min));
	QCOMPARE(opt->max(), QVariant(max));
	delete opt;
}

void tst_UciEngine::testParseOptionString_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("value");

	QTest::newRow("string with value")
	    << "option name SyzygyPath type string default <empty>"
	    << "SyzygyPath"
	    << "<empty>";
	QTest::newRow("string empty default")
	    << "option name Debug Log File type string default "
	    << "Debug Log File"
	    << "";
}

void tst_UciEngine::testParseOptionString()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(QString, value);

	auto opt = dynamic_cast<EngineTextOption*>(parseOption(firstToken(optionString)));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	delete opt;
}

void tst_UciEngine::testParseOptionCombo_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("value");
	QTest::addColumn<QStringList>("choices");

	QTest::newRow("combo 1")
	    << "option name Style type combo default Normal var Solid var Normal var Risky"
	    << "Style"
	    << "Normal"
	    << QStringList{ "Solid", "Normal", "Risky" };
}

void tst_UciEngine::testParseOptionCombo()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(QString, value);
	QFETCH(QStringList, choices);

	auto opt = dynamic_cast<EngineComboOption*>(parseOption(firstToken(optionString)));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	QCOMPARE(opt->choices(), choices);
	delete opt;
}

QTEST_MAIN(tst_UciEngine)
#include "tst_uciengine.moc"

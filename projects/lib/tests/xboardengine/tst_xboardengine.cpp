#include <QtTest/QtTest>
#include <xboardengine.h>

class tst_XboardEngine: public XboardEngine
{
	Q_OBJECT

	public:
		tst_XboardEngine();

	private slots:
		void testParseFeatures_data() const;
		void testParseFeatures();
		void testParsePv_data() const;
		void testParsePv();
};

tst_XboardEngine::tst_XboardEngine()
    : XboardEngine()
{
}

void tst_XboardEngine::testParseFeatures_data() const
{
	QTest::addColumn<QString>("featureString");
	QTest::addColumn<QList<XboardFeature>>("features");

	QTest::newRow("simple")
	    << "myname=\"Fairy-Max 5.0b\""
	    << QList<XboardFeature>{ std::make_pair("myname", "Fairy-Max 5.0b")};
	QTest::newRow("multiple")
	    << "memory=1 exclude=1"
	    << QList<XboardFeature>{ std::make_pair("memory", "1"), std::make_pair("exclude", "1")};
	QTest::newRow("option")
	    << "option=\"Resign Threshold -spin 800 200 1200\""
	    << QList<XboardFeature>{ std::make_pair("option", "Resign Threshold -spin 800 200 1200")};
}

void tst_XboardEngine::testParseFeatures()
{
	QFETCH(QString, featureString);
	QFETCH(QList<XboardFeature>, features);

	QCOMPARE(parseFeatures(featureString), features);
}

void tst_XboardEngine::testParsePv_data() const
{
	QTest::addColumn<QString>("pvString");
	QTest::addColumn<MoveEvaluation>("pv");

	QTest::newRow("search depth")
	    << "4"
	    << MoveEvaluation();
	QTest::newRow("search depth and evaluation")
	    << "4    109"
	    << MoveEvaluation();
	QTest::newRow("search depth, evaluation and search time")
	    << "4    109      14"
	    << MoveEvaluation();
	QTest::newRow("search depth, evaluation, search time and node count")
	    << "4    109      14   1435"
	    << MoveEvaluation();

	MoveEvaluation eval1;
	eval1.setDepth(4);
	eval1.setScore(109);
	eval1.setTime(14 * 10);
	eval1.setNodeCount(1435);
	eval1.setPv("1. e4 d5 2. Qf3 dxe4 3. Qxe4 Nc6");
	QTest::newRow("search depth, evaluation, search time, node count and pv")
	    << "4    109      14   1435  1. e4 d5 2. Qf3 dxe4 3. Qxe4 Nc6"
	    << eval1;

	MoveEvaluation eval2;
	eval2.setDepth(12);
	eval2.setScore(-18);
	eval2.setTime(518 * 10);
	eval2.setNodeCount(12708664);
	eval2.setPv("Nf6 Nc3 d5 exd5 Nxd5 Nf3 Nc6 Be2 Nf4 O-O Bf5 d3");
	QTest::newRow("search depth, evaluation, search time, node count and pv 2")
	    << "12 -18 518 12708664 Nf6 Nc3 d5 exd5 Nxd5 Nf3 Nc6 Be2 Nf4 O-O Bf5 d3"
	    << eval2;
}

void tst_XboardEngine::testParsePv()
{
	QFETCH(QString, pvString);
	QFETCH(MoveEvaluation, pv);

	QStringRef pvStringRef(firstToken(pvString));

	QCOMPARE(parsePv(pvStringRef), pv);
}

QTEST_MAIN(tst_XboardEngine)
#include "tst_xboardengine.moc"

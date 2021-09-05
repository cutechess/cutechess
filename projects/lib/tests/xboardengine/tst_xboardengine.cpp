#include <QtTest/QtTest>
#include <xboardengine.h>
#include <enginebuttonoption.h>
#include <enginecheckoption.h>
#include <enginespinoption.h>
#include <enginetextoption.h>
#include <enginecombooption.h>

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
		void testParseOptionButton_data();
		void testParseOptionButton();
		void testParseOptionCheck_data();
		void testParseOptionCheck();
		void testParseOptionSpin_data();
		void testParseOptionSpin();
		void testParseOptionText_data();
		void testParseOptionText();
		void testParseOptionCombo_data();
		void testParseOptionCombo();
};

Q_DECLARE_METATYPE(EngineTextOption::EditorType)

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

void tst_XboardEngine::testParseOptionButton_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");

	QTest::newRow("button 1")
	    << "Info -button"
	    << "Info";
	QTest::newRow("button 2")
	    << "Clear Hash -button"
	    << "Clear Hash";
	QTest::newRow("save 1")  // -save is represented as -button
	    << "Another Type -save"
	    << "Another Type";
}

void tst_XboardEngine::testParseOptionButton()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);

	auto opt = dynamic_cast<EngineButtonOption*>(parseOption(optionString));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	delete opt;
}

void tst_XboardEngine::testParseOptionCheck_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<bool>("value");

	QTest::newRow("check 1")
	    << "Resign -check 0"
	    << "Resign"
	    << false;
	QTest::newRow("check 2")
	    << "Resign Always -check 1"
	    << "Resign Always"
	    << true;
}

void tst_XboardEngine::testParseOptionCheck()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(bool, value);

	auto opt = dynamic_cast<EngineCheckOption*>(parseOption(optionString));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	delete opt;
}

void tst_XboardEngine::testParseOptionSpin_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<int>("value");
	QTest::addColumn<int>("min");
	QTest::addColumn<int>("max");

	QTest::newRow("spin 1")
	    << "Multi-PV Margin -spin 0 0 1000"
	    << "Multi-PV Margin"
	    << 0
	    << 0
	    << 1000;
	QTest::newRow("spin 2")
	    << "Resign Threshold -spin 800 200 1200"
	    << "Resign Threshold"
	    << 800
	    << 200
	    << 1200;
	QTest::newRow("slider 1")  // -slider is represented as -spin
	    << "Dummy Slider Example -slider 20 0 100"
	    << "Dummy Slider Example"
	    << 20
	    << 0
	    << 100;
}

void tst_XboardEngine::testParseOptionSpin()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(int, value);
	QFETCH(int, min);
	QFETCH(int, max);

	auto opt = dynamic_cast<EngineSpinOption*>(parseOption(optionString));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	QCOMPARE(opt->min(), QVariant(min));
	QCOMPARE(opt->max(), QVariant(max));
	delete opt;
}

void tst_XboardEngine::testParseOptionText_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("value");
	QTest::addColumn<EngineTextOption::EditorType>("editorType");

	QTest::newRow("string 1")
	    << "Dummy String Example -string happy birthday!"
	    << "Dummy String Example"
	    << "happy birthday!"
	    << EngineTextOption::LineEdit;
	QTest::newRow("file 1")
	    << "Ini File -file /usr/share/games/engine/engine.ini"
	    << "Ini File"
	    << "/usr/share/games/engine/engine.ini"
	    << EngineTextOption::FileDialog;
	QTest::newRow("path 1")
	    << "Dummy Path Example -path ."
	    << "Dummy Path Example"
	    << "."
	    << EngineTextOption::FolderDialog;
}

void tst_XboardEngine::testParseOptionText()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(QString, value);
	QFETCH(EngineTextOption::EditorType, editorType);

	auto opt = dynamic_cast<EngineTextOption*>(parseOption(optionString));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	QCOMPARE(opt->editorType(), editorType);
	delete opt;
}

void tst_XboardEngine::testParseOptionCombo_data()
{
	QTest::addColumn<QString>("optionString");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("value");
	QTest::addColumn<QStringList>("choices");

	QTest::newRow("combo 1")
	    << "Variant -combo FIDE-Clobberers /// Clobberers-FIDE /// FIDE-Nutters /// Nutters-FIDE"
	    << "Variant"
	    << "FIDE-Clobberers"
	    << QStringList{ "FIDE-Clobberers", "Clobberers-FIDE", "FIDE-Nutters", "Nutters-FIDE" };
	QTest::newRow("combo 2")
	    << "Variant -combo FIDE-Clobberers /// Clobberers-FIDE /// *FIDE-Nutters /// Nutters-FIDE"
	    << "Variant"
	    << "FIDE-Nutters"
	    << QStringList{ "FIDE-Clobberers", "Clobberers-FIDE", "FIDE-Nutters", "Nutters-FIDE" };
}

void tst_XboardEngine::testParseOptionCombo()
{
	QFETCH(QString, optionString);
	QFETCH(QString, name);
	QFETCH(QString, value);
	QFETCH(QStringList, choices);

	auto opt = dynamic_cast<EngineComboOption*>(parseOption(optionString));
	QVERIFY(opt != nullptr);
	QCOMPARE(opt->name(), name);
	QCOMPARE(opt->value(), QVariant(value));
	QCOMPARE(opt->defaultValue(), QVariant(value));
	QCOMPARE(opt->choices(), choices);
	delete opt;
}

QTEST_MAIN(tst_XboardEngine)
#include "tst_xboardengine.moc"

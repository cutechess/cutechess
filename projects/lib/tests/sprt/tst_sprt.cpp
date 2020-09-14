#include <QtTest/QtTest>
#include <sprt.h>


class tst_Sprt: public QObject
{
	Q_OBJECT

	private slots:
		void sprt_data() const;
		void sprt();

	private:
		bool fuzzyCompare(double val1, double val2);
};


bool tst_Sprt::fuzzyCompare(double val1, double val2)
{
	double delta = 0.01;
	return (val1 - delta <= val2 && val1 + delta >= val2);
}

void tst_Sprt::sprt_data() const
{
	QTest::addColumn<double>("elo0");
	QTest::addColumn<double>("elo1");
	QTest::addColumn<double>("alpha");
	QTest::addColumn<double>("beta");
	QTest::addColumn<int>("wins");
	QTest::addColumn<int>("losses");
	QTest::addColumn<int>("draws");
	QTest::addColumn<double>("llr");
	QTest::addColumn<double>("lbound");
	QTest::addColumn<double>("ubound");

	QTest::newRow("test1")
		<< 0.0
		<< 10.0
		<< 0.01
		<< 0.01
		<< 1477
		<< 1351
		<< 2942
		<< 2.52
		<< -4.6
		<< 4.6;

	QTest::newRow("test2")
		<< 0.0
		<< 10.0
		<< 0.01
		<< 0.01
		<< 555
		<< 555
		<< 298
		<< -0.74
		<< -4.6
		<< 4.6;

	QTest::newRow("test3")
		<< 0.0
		<< 10.0
		<< 0.01
		<< 0.01
		<< 3
		<< 149
		<< 1
		<< -4.69
		<< -4.6
		<< 4.6;
}

void tst_Sprt::sprt()
{
	QFETCH(double, elo0);
	QFETCH(double, elo1);
	QFETCH(double, alpha);
	QFETCH(double, beta);
	QFETCH(int, wins);
	QFETCH(int, losses);
	QFETCH(int, draws);
	QFETCH(double, llr);
	QFETCH(double, lbound);
	QFETCH(double, ubound);

	Sprt sprt;
	sprt.initialize(elo0, elo1, alpha, beta);

	for (int i = 0; i < wins; i++)
		sprt.addGameResult(Sprt::Win);
	for (int i = 0; i < losses; i++)
		sprt.addGameResult(Sprt::Loss);
	for (int i = 0; i < draws; i++)
		sprt.addGameResult(Sprt::Draw);

	Sprt::Status status = sprt.status();
	QVERIFY(fuzzyCompare(status.llr, llr));
	QVERIFY(fuzzyCompare(status.lBound, lbound));
	QVERIFY(fuzzyCompare(status.uBound, ubound));
}

QTEST_MAIN(tst_Sprt)
#include "tst_sprt.moc"

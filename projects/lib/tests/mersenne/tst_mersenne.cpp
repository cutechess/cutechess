#include <QtTest/QtTest>
#include <mersenne.h>

class tst_Mersenne: public QObject
{
	Q_OBJECT

	private slots:
		void numbers_data();
		void numbers();
};

void tst_Mersenne::numbers_data()
{
	QTest::addColumn<quint32>("seed");
	QTest::addColumn<quint32>("random1");
	QTest::addColumn<quint32>("random2");

	QTest::newRow("0")
		<< quint32(0U)
		<< quint32(2357136044U)
		<< quint32(1745961492U);

	QTest::newRow("0xFFFFFFFF")
		<< quint32(0xFFFFFFFFU)
		<< quint32(485482874U)
		<< quint32(4213522654U);
}

void tst_Mersenne::numbers()
{
	QFETCH(quint32, seed);
	QFETCH(quint32, random1);
	QFETCH(quint32, random2);

	Mersenne::initialize(seed);
	QCOMPARE(Mersenne::random(), random1);
	QCOMPARE(Mersenne::random(), random2);
}


QTEST_MAIN(tst_Mersenne)
#include "tst_mersenne.moc"

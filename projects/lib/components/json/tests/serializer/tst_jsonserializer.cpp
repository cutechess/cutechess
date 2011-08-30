/*
    Copyright (c) 2010 Ilari Pihlajisto

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QtTest/QtTest>
#include <jsonparser.h>
#include <jsonserializer.h>

class tst_JsonSerializer: public QObject
{
	Q_OBJECT

	private slots:
		void test_data() const;
		void test() const;

	private:
		QVariant sample1() const;
		QVariant sample2() const;
};
Q_DECLARE_METATYPE(QVariant)


QVariant tst_JsonSerializer::sample1() const
{
	QVariantList list;
	list << "GML" << "XML";

	QVariantMap glossDef;
	glossDef["para"] = "A meta-markup language, used to create markup "
			   "languages such as DocBook.";
	glossDef["GlossSeeAlso"] = list;

	QVariantMap glossEntry;
	glossEntry["ID"] = "SGML";
	glossEntry["SortAs"] = "SGML";
	glossEntry["GlossTerm"] = "Standard Generalized Markup Language";
	glossEntry["Acronym"] = "SGML";
	glossEntry["Abbrev"] = "ISO 8879:1986";
	glossEntry["GlossDef"] = glossDef;
	glossEntry["GlossSee"] = "markup";

	QVariantMap glossList;
	glossList["GlossEntry"] = glossEntry;

	QVariantMap glossDiv;
	glossDiv["title"] = "S";
	glossDiv["GlossList"] = glossList;

	QVariantMap glossary;
	glossary["title"] = "example glossary";
	glossary["GlossDiv"] = glossDiv;

	QVariantMap map;
	map["glossary"] = glossary;

	return QVariant(map);
}

QVariant tst_JsonSerializer::sample2() const
{
	QVariantList list;

	QVariantMap map;

	QVariantMap tmp;
	tmp["id"] = "1001";
	tmp["type"] = "Regular";
	list << tmp;

	tmp["id"] = "1002";
	tmp["type"] = "Chocolate";
	list << tmp;

	tmp["id"] = "1003";
	tmp["type"] = "Blueberry";
	list << tmp;

	tmp["id"] = "1004";
	tmp["type"] = "Devil's Food";
	list << tmp;

	QVariantMap batter;
	batter["batter"] = list;

	map["id"] = "0001";
	map["type"] = "donut";
	map["name"] = "Cake";
	map["ppu"] = 0.55;
	map["batters"] = batter;


	list.clear();
	tmp["id"] = "5001";
	tmp["type"] = "None";
	list << tmp;

	tmp["id"] = "5002";
	tmp["type"] = "Glazed";
	list << tmp;

	tmp["id"] = "5005";
	tmp["type"] = "Sugar";
	list << tmp;

	tmp["id"] = "5007";
	tmp["type"] = "Powdered Sugar";
	list << tmp;

	tmp["id"] = "5006";
	tmp["type"] = "Chocolate with Sprinkles";
	list << tmp;

	tmp["id"] = "5003";
	tmp["type"] = "Chocolate";
	list << tmp;

	tmp["id"] = "5004";
	tmp["type"] = "Maple";
	list << tmp;

	map["topping"] = list;

	list.clear();
	list << map;

	return QVariant(list);
}

void tst_JsonSerializer::test_data() const
{
	QTest::addColumn<QVariant>("input");

	QTest::newRow("null") << QVariant();

	QTest::newRow("true") << QVariant(true);
	QTest::newRow("false") << QVariant(false);

	QTest::newRow("int") << QVariant(1234567890);
	QTest::newRow("negative int") << QVariant(-1234567890);
	QTest::newRow("64-bit int") << QVariant(Q_INT64_C(3567830610840546163));
	QTest::newRow("negative 64-bit int") << QVariant(Q_INT64_C(-3567830610840546163));

	QTest::newRow("double") << QVariant(0.012);
	QTest::newRow("negative double") << QVariant(-0.012);
	QTest::newRow("exponent double #1") << QVariant(123456789.1234);
	QTest::newRow("exponent double #2") << QVariant(123456789.1234);
	QTest::newRow("exponent double #3") << QVariant(123456789.1234);
	QTest::newRow("exponent double #4") << QVariant(0.0000371);

	QTest::newRow("string #1") << QVariant(QString());
	QTest::newRow("string #2") << QVariant("JSON string");
	QTest::newRow("string #3") << QVariant("line 1\nline 2\nline 3\n");
	QTest::newRow("string #4") << QVariant("Path = \"C:\\Program files\\foo\"");
	QTest::newRow("string #5") << QVariant("/\b\f\n\r\t");
	QTest::newRow("string #6") << QVariant(QString("%1%2%3%4")
		.arg(QChar(0x2654))
		.arg(QChar(0x2659))
		.arg(QChar(0x265A))
		.arg(QChar(0x265F)));


	QVariantMap obj;

	QTest::newRow("object #1") << QVariant(obj);

	obj["foo"] = "bar";
	obj["number"] = -25;
	obj["state"] = QVariant();
	QTest::newRow("object #2") << QVariant(obj);

	obj.clear();
	obj["empty array"] = QVariantList();
	QTest::newRow("object #3") << QVariant(obj);

	QStringList stringList;
	stringList << "aA" << "bB" << "cC" << "dD";
	obj.clear();
	obj["alphabet"] = stringList;
	QTest::newRow("stringlist") << QVariant(obj);

	QVariantList list;

	QTest::newRow("array #1") << QVariant(list);

	list << QVariant();
	QTest::newRow("array #2") << QVariant(list);

	list.clear();
	list << QVariant() << QVariantMap() << "string data" << 1234567890;
	QTest::newRow("array #3") << QVariant(list);

	QTest::newRow("complex #1") << sample1();
	QTest::newRow("complex #2") << sample2();
}

void tst_JsonSerializer::test() const
{
	QFETCH(QVariant, input);

	JsonSerializer serializer(input);
	QString str;
	QTextStream stream(&str, QIODevice::Text | QIODevice::WriteOnly);
	serializer.serialize(stream);
	QVERIFY(!serializer.hasError());

	stream.setString(&str, QIODevice::ReadOnly);
	JsonParser parser(stream);
	QVariant result(parser.parse());
	QVERIFY(!parser.hasError());

	QCOMPARE(result, input);
}

QTEST_MAIN(tst_JsonSerializer)
#include "tst_jsonserializer.moc"

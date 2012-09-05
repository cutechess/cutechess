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

class tst_JsonParser: public QObject
{
	Q_OBJECT

	private slots:
		void basics_data() const;
		void basics() const;

		void invalid_data() const;
		void invalid() const;

		void advanced_data() const;
		void advanced() const;

	private:
		QVariant sample1() const;
		QVariant sample2() const;
};
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QVariant::Type)


void tst_JsonParser::basics_data() const
{
	QTest::addColumn<QString>("input");
	QTest::addColumn<QVariant::Type>("type");
	QTest::addColumn<QVariant>("expected");

	QTest::newRow("null")
		<< "null"
		<< QVariant::Invalid
		<< QVariant();

	QTest::newRow("true")
		<< "true"
		<< QVariant::Bool
		<< QVariant(true);
	QTest::newRow("false")
		<< "false"
		<< QVariant::Bool
		<< QVariant(false);

	QTest::newRow("int")
		<< "1234567890"
		<< QVariant::Int
		<< QVariant(1234567890);
	QTest::newRow("negative int")
		<< "-1234567890"
		<< QVariant::Int
		<< QVariant(-1234567890);
	QTest::newRow("64-bit int")
		<< "3567830610840546163"
		<< QVariant::LongLong
		<< QVariant(Q_INT64_C(3567830610840546163));
	QTest::newRow("negative 64-bit int")
		<< "-3567830610840546163"
		<< QVariant::LongLong
		<< QVariant(Q_INT64_C(-3567830610840546163));

	QTest::newRow("double")
		<< "0.012"
		<< QVariant::Double
		<< QVariant(0.012);
	QTest::newRow("negative double")
		<< "-0.012"
		<< QVariant::Double
		<< QVariant(-0.012);
	QTest::newRow("exponent double #1")
		<< "1.234567891234E8"
		<< QVariant::Double
		<< QVariant(123456789.1234);
	QTest::newRow("exponent double #2")
		<< "1.234567891234e8"
		<< QVariant::Double
		<< QVariant(123456789.1234);
	QTest::newRow("exponent double #3")
		<< "1.234567891234E+8"
		<< QVariant::Double
		<< QVariant(123456789.1234);
	QTest::newRow("exponent double #4")
		<< "3.71E-05"
		<< QVariant::Double
		<< QVariant(0.0000371);

	QTest::newRow("string #1")
		<< "\"\""
		<< QVariant::String
		<< QVariant(QString());
	QTest::newRow("string #2")
		<< "\"JSON string\""
		<< QVariant::String
		<< QVariant("JSON string");
	QTest::newRow("string #3")
		<< "\"line 1\\nline 2\\nline 3\\n\""
		<< QVariant::String
		<< QVariant("line 1\nline 2\nline 3\n");
	QTest::newRow("string #4")
		<< "\"Path = \\\"C:\\\\Program files\\\\foo\\\"\""
		<< QVariant::String
		<< QVariant("Path = \"C:\\Program files\\foo\"");
	QTest::newRow("string #5")
		<< "\"\\/\\b\\f\\n\\r\\t\""
		<< QVariant::String
		<< QVariant("/\b\f\n\r\t");
	QTest::newRow("string #6")
		<< "\"\\u2654\\u2659\\u265A\\u265f\""
		<< QVariant::String
		<< QVariant(QString("%1%2%3%4")
			.arg(QChar(0x2654))
			.arg(QChar(0x2659))
			.arg(QChar(0x265A))
			.arg(QChar(0x265F)));


	QVariantMap obj;

	QTest::newRow("object #1")
		<< "{}"
		<< QVariant::Map
		<< QVariant(obj);

	obj["foo"] = "bar";
	obj["number"] = -25;
	obj["state"] = QVariant();
	QTest::newRow("object #2")
		<< "{\"foo\" : \"bar\", \"number\" : -25, \"state\" : null}"
		<< QVariant::Map
		<< QVariant(obj);

	obj.clear();
	obj["empty array"] = QVariantList();
	QTest::newRow("object #3")
		<< "{\"empty array\" : []}"
		<< QVariant::Map
		<< QVariant(obj);


	QVariantList list;

	QTest::newRow("array #1")
		<< "[]"
		<< QVariant::List
		<< QVariant(list);

	list << QVariant();
	QTest::newRow("array #2")
		<< "[null]"
		<< QVariant::List
		<< QVariant(list);

	list.clear();
	list << QVariant() << QVariantMap() << "string data" << 1234567890;
	QTest::newRow("array #3")
		<< "[null, {}, \"string data\", 1234567890]"
		<< QVariant::List
		<< QVariant(list);
}

void tst_JsonParser::basics() const
{
	QFETCH(QString, input);
	QFETCH(QVariant::Type, type);
	QFETCH(QVariant, expected);

	QTextStream stream(&input, QIODevice::ReadOnly);
	JsonParser parser(stream);
	QVariant data(parser.parse());

	QCOMPARE(data.type(), type);
	QCOMPARE(data, expected);
}

void tst_JsonParser::invalid_data() const
{
	QTest::addColumn<QString>("input");

	QTest::newRow("invalid #1") << "random text";
	QTest::newRow("invalid #2") << "\"endquote missing";
	QTest::newRow("invalid #3") << "+256";
	QTest::newRow("invalid #4") << "256x";
	QTest::newRow("invalid #5") << "100.3.4";
	QTest::newRow("invalid #6") << "\"\\u005 \"";
	QTest::newRow("invalid #7") << "\"\\uffgg\"";
	QTest::newRow("invalid #8") << "{";
	QTest::newRow("invalid #9") << "[";
	QTest::newRow("invalid #10") << "}";
	QTest::newRow("invalid #11") << "]";
	QTest::newRow("invalid #12") << "{ ]";
	QTest::newRow("invalid #13") << "[ }";
	QTest::newRow("invalid #14") << "{ null }";
	QTest::newRow("invalid #15") << "{ null, null }";
	QTest::newRow("invalid #16") << "{ \"id\" : 1, }";
	QTest::newRow("invalid #17") << "{ \"id\" : ,0 }";
	QTest::newRow("invalid #18") << "{ , }";
	QTest::newRow("invalid #19") << "[ , ]";
	QTest::newRow("invalid #20") << "[ \"id\" : 1 ]";
	QTest::newRow("invalid #21") << "[ null, ]";
}

void tst_JsonParser::invalid() const
{
	QFETCH(QString, input);

	QTextStream stream(&input, QIODevice::ReadOnly);
	JsonParser parser(stream);
	QVariant data(parser.parse());

	QVERIFY(data.isNull());
	QVERIFY(parser.hasError());
}

QVariant tst_JsonParser::sample1() const
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

QVariant tst_JsonParser::sample2() const
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

void tst_JsonParser::advanced_data() const
{
	QTest::addColumn<QString>("filename");
	QTest::addColumn<QVariant::Type>("type");
	QTest::addColumn<QVariant>("expected");

	QTest::newRow("advanced #1")
		<< "sample1.json"
		<< QVariant::Map
		<< sample1();
	QTest::newRow("advanced #2")
		<< "sample2.json"
		<< QVariant::List
		<< sample2();
}

void tst_JsonParser::advanced() const
{
	QFETCH(QString, filename);
	QFETCH(QVariant::Type, type);
	QFETCH(QVariant, expected);

	QFile file(filename);
	QVERIFY(file.open(QIODevice::Text | QIODevice::ReadOnly));
	QTextStream stream(&file);
	JsonParser parser(stream);
	QVariant data(parser.parse());

	QCOMPARE(data.type(), type);
	QCOMPARE(data, expected);
}

QTEST_MAIN(tst_JsonParser)
#include "tst_jsonparser.moc"

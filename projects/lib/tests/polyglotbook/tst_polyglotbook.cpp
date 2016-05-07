#include <QtTest/QtTest>
#include <QMap>
#include <polyglotbook.h>
#include <board/standardboard.h>

class tst_PolyglotBook: public QObject
{
	Q_OBJECT

	private slots:
		void initialValues();
		void startPos();

	private:
		QMap<QString,quint16> entries(const OpeningBook* book,
					      Chess::Board* board) const;
};

void tst_PolyglotBook::initialValues()
{
	auto book = PolyglotBook(OpeningBook::Ram);

	QCOMPARE(book.read("foo.bin"), false);
	QVERIFY(book.move(1234).isNull());
	QVERIFY(book.entries(1234).isEmpty());
}

QMap<QString,quint16> tst_PolyglotBook::entries(const OpeningBook* book,
						Chess::Board* board) const
{
	QMap<QString,quint16> ret;

	for (auto entry: book->entries(board->key()))
	{
		auto move = board->moveFromGenericMove(entry.move);
		QString san = board->moveString(move, Chess::Board::StandardAlgebraic);
		ret[san] = entry.weight;
	}

	return ret;
}

void tst_PolyglotBook::startPos()
{
	QMap<QString,quint16> expect;
	expect["b4"] = 3;
	expect["c3"] = 3;
	expect["d3"] = 3;
	expect["Nc3"] = 4;
	expect["f4"] = 20;
	expect["b3"] = 52;
	expect["g3"] = 83;
	expect["c4"] = 2071;
	expect["Nf3"] = 2824;
	expect["e4"] = 11476;
	expect["d4"] = 11712;

	auto book = PolyglotBook(OpeningBook::Ram);
	QVERIFY(book.read("book_small.bin"));

	Chess::StandardBoard board;
	board.initialize();
	board.setFenString(board.defaultFenString());
	QCOMPARE(board.key(), Q_UINT64_C(0x463b96181691fc9c));

	auto entries = this->entries(&book, &board);
	QCOMPARE(entries, expect);

	// Same test with direct disk access
	book = PolyglotBook(OpeningBook::Disk);
	QVERIFY(book.read("book_small.bin"));

	entries = this->entries(&book, &board);
	QCOMPARE(entries, expect);
}

QTEST_MAIN(tst_PolyglotBook)
#include "tst_polyglotbook.moc"

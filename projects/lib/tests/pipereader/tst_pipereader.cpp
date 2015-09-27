#include <QtTest/QtTest>
#include <QSignalSpy>
#include <pipereader_win.h>

class tst_Pipereader: public QObject
{
	Q_OBJECT

	private slots:
		void initTestCase();
		
		void initialized();
		void readText_data();
		void readText();
		
		void cleanupTestCase();
		
	private:
		qint64 write(const char* data);

		HANDLE m_read;
		HANDLE m_write;
		PipeReader* m_reader;
};


void tst_Pipereader::initTestCase()
{
	m_read = INVALID_HANDLE_VALUE;
	m_write = INVALID_HANDLE_VALUE;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	CreatePipe(&m_read, &m_write, &sa, 0);

	m_reader = new PipeReader(m_read, this);
	m_reader->start();
}

void tst_Pipereader::cleanupTestCase()
{
	QSignalSpy spy(m_reader, SIGNAL(finished()));
	CloseHandle(m_write);

	while (!spy.count())
		QTest::qWait(50);
	CloseHandle(m_read);
}

qint64 tst_Pipereader::write(const char* data)
{
	DWORD maxSize = qstrlen(data);
	DWORD dwWritten = 0;
	if (!WriteFile(m_write, data, maxSize, &dwWritten, 0))
		return -1;
	return qint64(dwWritten);
}

void tst_Pipereader::initialized()
{
	QCOMPARE(m_reader->bytesAvailable(), 0);
	QCOMPARE(m_reader->canReadLine(), false);
}

void tst_Pipereader::readText_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<int>("maxSize");
	QTest::addColumn<QString>("expect");

	QTest::newRow("one-liner") << "simple line 1\n" << 100 << "";
	QTest::newRow("two-liner") << "simple line 2\nsimple line 3" << 100 << "";
	QTest::newRow("two-liner2") << "simple line 4\nsimple line 5\n" << 100 << "";
	QTest::newRow("buffer1") << "simpleline6\n" << 6 << "simple";
	QTest::newRow("buffer2") << "\n" << 100 << "line6\n\n";
}

void tst_Pipereader::readText()
{
	QFETCH(QString, text);
	QFETCH(int, maxSize);
	QFETCH(QString, expect);
	QByteArray bytes = text.toLatin1();
	char data[1024];
	qint64 ms = qint64(maxSize);

	QSignalSpy spy(m_reader, SIGNAL(readyRead()));
	qint64 bytesWritten = write(bytes.constData());

	while (!spy.count())
		QTest::qWait(50);
	QVERIFY(m_reader->canReadLine());
	QVERIFY(m_reader->bytesAvailable() >= bytesWritten);
	qint64 readBytes = m_reader->readData(data, ms);

	data[readBytes] = 0; // null termination needed for comparison
	if (expect.isEmpty())
	{
		bytes.truncate(ms);
		QVERIFY(data == bytes);
	}
	else
	{
		QVERIFY(data == expect);
	}
}


QTEST_MAIN(tst_Pipereader)
#include "tst_pipereader.moc"

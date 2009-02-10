#include <QtTest/QtTest>
#include <chessboard/chessboard.h>


class TestBoard: public QObject
{
	Q_OBJECT
	
	private slots:
		void zobristKeys_data() const;
		void zobristKeys();
		
		void moveStrings_data() const;
		void moveStrings();
		
		void perft_data() const;
		void perft();
	
	private:
		Chess::Board m_board;
};

static quint64 perftVal(Chess::Board& board, int depth)
{
	quint64 nodeCount = 0;
	QVector<Chess::Move> moves(board.legalMoves());
	if (depth == 1 || moves.size() == 0)
		return moves.size();

	QVector<Chess::Move>::const_iterator it;
	for (it = moves.begin(); it != moves.end(); ++it)
	{
		board.makeMove(*it);
		nodeCount += perftVal(board, depth - 1);
		board.undoMove();
	}

	return nodeCount;
}

static quint64 smpPerft(Chess::Board& board, int depth)
{
	QVector<Chess::Move> moves(board.legalMoves());
	if (depth <= 1)
		return moves.size();
	
	QVector< QFuture<quint64> > futures(moves.size());
	
	for (int i = 0; i < moves.size(); i++)
	{
		board.makeMove(moves[i]);
		futures[i] = QtConcurrent::run(perftVal, board, depth - 1);
		board.undoMove();
	}
	
	quint64 nodeCount = 0;
	foreach (const QFuture<quint64>& future, futures)
		nodeCount += future.result();

	return nodeCount;
}


void TestBoard::zobristKeys_data() const
{
	QTest::addColumn<int>("variant");
	QTest::addColumn<QString>("fen");
	QTest::addColumn<quint64>("key");
	
	int variant = Chess::Variant::Standard;

	QTest::newRow("startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< Q_UINT64_C(0x463b96181691fc9c);
	QTest::newRow("e2e4")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3"
		<< Q_UINT64_C(0x823c9b50fd114196);
	QTest::newRow("e2e4 d7d5")
		<< variant
		<< "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6"
		<< Q_UINT64_C(0x0756b94461c50fb0);
	QTest::newRow("e2e4 d7d5 e4e5")
		<< variant
		<< "rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq -"
		<< Q_UINT64_C(0x662fafb965db29d4);
	QTest::newRow("e2e4 d7d5 e4e5 f7f5")
		<< variant
		<< "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6"
		<< Q_UINT64_C(0x22a48b5a8e47ff78);
	QTest::newRow("e2e4 d7d5 e4e5 f7f5 e1e2")
		<< variant
		<< "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq -"
		<< Q_UINT64_C(0x652a607ca3f242c1);
	QTest::newRow("e2e4 d7d5 e4e5 f7f5 e1e2 e8f7")
		<< variant
		<< "rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - -"
		<< Q_UINT64_C(0x00fdd303c946bdd9);
	QTest::newRow("a2a4 b7b5 h2h4 b5b4 c2c4")
		<< variant
		<< "rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3"
		<< Q_UINT64_C(0x3c8123ea7b067637);
	QTest::newRow("a2a4 b7b5 h2h4 b5b4 c2c4 b4c3 a1a3")
		<< variant
		<< "rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq -"
		<< Q_UINT64_C(0x5c3f9b829b279560);
}

void TestBoard::zobristKeys()
{
	QFETCH(int, variant);
	QFETCH(QString, fen);
	QFETCH(quint64, key);

	m_board.setVariant((Chess::Variant::Code)variant);
	QVERIFY(m_board.setBoard(fen));
	QCOMPARE(m_board.key(), key);
}

void TestBoard::moveStrings_data() const
{
	QTest::addColumn<int>("variant");
	QTest::addColumn<QString>("moves");
	QTest::addColumn<QString>("startfen");
	QTest::addColumn<QString>("endfen");
	
	int variant = Chess::Variant::Standard;
	
	QTest::newRow("san")
		<< variant
		<< "e4 Nc6 e5 d5 exd6 Be6 Nf3 Qd7 Bb5 O-O-O dxc7 a6 O-O Qxd2 "
		   "cxd8=N Qxc1 Bxc6 Qxd1 Nxb7 Qxf1+ Kxf1 Bxa2 Rxa2 Kc7"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "5bnr/1Nk1pppp/p1B5/8/8/5N2/RPP2PPP/1N3K2 w - - 1 13";
	QTest::newRow("coord")
		<< variant
		<< "e2e4 b8c6 e4e5 d7d5 e5d6 c8e6 g1f3 d8d7 f1b5 e8c8 d6c7 "
		   "a7a6 e1g1 d7d2 c7d8n d2c1 b5c6 c1d1 d8b7 d1f1 g1f1 e6a2 "
		   "a1a2 c8c7"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "5bnr/1Nk1pppp/p1B5/8/8/5N2/RPP2PPP/1N3K2 w - - 1 13";
}

void TestBoard::moveStrings()
{
	QFETCH(int, variant);
	QFETCH(QString, moves);
	QFETCH(QString, startfen);
	QFETCH(QString, endfen);

	m_board.setVariant((Chess::Variant::Code)variant);
	QVERIFY(m_board.setBoard(startfen));

	foreach (const QString& moveStr, moves.split(' '))
	{
		Chess::Move move = m_board.moveFromString(moveStr);
		QVERIFY(m_board.isLegalMove(move));
		m_board.makeMove(move);
	}
	
	QCOMPARE(m_board.startingFen(), startfen);
	QCOMPARE(m_board.fenString(), endfen);
}

void TestBoard::perft_data() const
{
	QTest::addColumn<int>("variant");
	QTest::addColumn<QString>("fen");
	QTest::addColumn<int>("depth");
	QTest::addColumn<quint64>("nodecount");
	
	int variant = Chess::Variant::Standard;
	
	QTest::newRow("startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5
		<< Q_UINT64_C(4865609);
	QTest::newRow("pos2")
		<< variant
		<< "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
		<< 4
		<< Q_UINT64_C(4085603);
	QTest::newRow("pos3")
		<< variant
		<< "8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - -"
		<< 2
		<< Q_UINT64_C(279);
	QTest::newRow("pos4")
		<< variant
		<< "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
		<< 6
		<< Q_UINT64_C(11030083);
	
	variant = Chess::Variant::Gothic;
	QTest::newRow("gothic startpos")
		<< variant
		<< "rnbqckabnr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNBQCKABNR w KQkq - 0 1"
		<< 4
		<< Q_UINT64_C(808984);
	QTest::newRow("goth2")
		<< variant
		<< "r1b1c2rk1/p4a1ppp/1ppq2pn2/3p1p4/3A1Pn3/1PN3PN2/P1PQP1BPPP/3RC2RK1 w - -"
		<< 4
		<< Q_UINT64_C(7917813);
	QTest::newRow("goth3")
		<< variant
		<< "r1b2k2nr/p1ppq1ppbp/n1Pcpa2p1/5p4/5P4/1p1PBCPN2/PP1QP1BPPP/RN3KA2R w KQkq -"
		<< 4
		<< Q_UINT64_C(4869569);
}

void TestBoard::perft()
{
	QFETCH(int, variant);
	QFETCH(QString, fen);
	QFETCH(int, depth);
	QFETCH(quint64, nodecount);

	m_board.setVariant((Chess::Variant::Code)variant);
	QVERIFY(m_board.setBoard(fen));
	QCOMPARE(smpPerft(m_board, depth), nodecount);
}

QTEST_MAIN(TestBoard)
#include "test_board.moc"


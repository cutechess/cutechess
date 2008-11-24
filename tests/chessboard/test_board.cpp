#include <QtTest/QtTest>
#include <base/chessboard/chessboard.h>


class TestBoard: public QObject
{
	Q_OBJECT
	
	private slots:
		void zobristKeys() const;
		void standardPerft() const;
		void gothicPerft() const;
};

static quint64 perft(Chess::Board& board, int depth)
{
	quint64 nodeCount = 0;
	QVector<Chess::Move> moves(board.legalMoves());
	if (depth == 1 || moves.size() == 0)
		return moves.size();

	QVector<Chess::Move>::const_iterator it;
	for (it = moves.begin(); it != moves.end(); ++it)
	{
		board.makeMove(*it);
		nodeCount += perft(board, depth - 1);
		board.undoMove();
	}

	return nodeCount;
}


void TestBoard::zobristKeys() const
{
	Chess::Board b(Chess::StandardChess);

	QCOMPARE(b.setBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);
	QCOMPARE(b.key(), Q_UINT64_C(0x463b96181691fc9c));

	QCOMPARE(b.setBoard("rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4"), true);
	QCOMPARE(b.key(), Q_UINT64_C(0x00fdd303c946bdd9));
	
	QCOMPARE(b.setBoard("rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3 0 3"), true);
	QCOMPARE(b.key(), Q_UINT64_C(0x3c8123ea7b067637));
	
	QCOMPARE(b.setBoard("rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq - 0 4"), true);
	QCOMPARE(b.key(), Q_UINT64_C(0x5c3f9b829b279560));
	
	QCOMPARE(b.setBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"), true);
	QCOMPARE(b.key(), Q_UINT64_C(0xc3ce103f01d15e1d));
}

void TestBoard::standardPerft() const
{
	Chess::Board b(Chess::StandardChess);

	QCOMPARE(b.setBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);
	QCOMPARE(perft(b, 1), Q_UINT64_C(20));
	QCOMPARE(perft(b, 2), Q_UINT64_C(400));
	QCOMPARE(perft(b, 3), Q_UINT64_C(8902));
	QCOMPARE(perft(b, 4), Q_UINT64_C(197281));
	QCOMPARE(perft(b, 5), Q_UINT64_C(4865609));

	QCOMPARE(b.setBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"), true);
	QCOMPARE(perft(b, 1), Q_UINT64_C(48));
	QCOMPARE(perft(b, 2), Q_UINT64_C(2039));
	QCOMPARE(perft(b, 3), Q_UINT64_C(97862));
	QCOMPARE(perft(b, 4), Q_UINT64_C(4085603));

	QCOMPARE(b.setBoard("8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67"), true);
	QCOMPARE(perft(b, 1), Q_UINT64_C(50));
	QCOMPARE(perft(b, 2), Q_UINT64_C(279));

	QCOMPARE(b.setBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"), true);
	QCOMPARE(perft(b, 6), Q_UINT64_C(11030083));
}

void TestBoard::gothicPerft() const
{
	Chess::Board b(Chess::CapablancaChess);

	QCOMPARE(b.setBoard("rnbqckabnr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNBQCKABNR w KQkq - 0 1"), true);
	QCOMPARE(perft(b, 1), Q_UINT64_C(28));
	QCOMPARE(perft(b, 2), Q_UINT64_C(784));
	QCOMPARE(perft(b, 3), Q_UINT64_C(25283));
	QCOMPARE(perft(b, 4), Q_UINT64_C(808984));

	QCOMPARE(b.setBoard("r1b1c2rk1/p4a1ppp/1ppq2pn2/3p1p4/3A1Pn3/1PN3PN2/P1PQP1BPPP/3RC2RK1 w - - 0 15"), true);
	QCOMPARE(perft(b, 1), Q_UINT64_C(50));
	QCOMPARE(perft(b, 2), Q_UINT64_C(2801));
	QCOMPARE(perft(b, 3), Q_UINT64_C(143032));
	QCOMPARE(perft(b, 4), Q_UINT64_C(7917813));

	QCOMPARE(b.setBoard("r1b2k2nr/p1ppq1ppbp/n1Pcpa2p1/5p4/5P4/1p1PBCPN2/PP1QP1BPPP/RN3KA2R w KQkq - 6 12"), true);
	QCOMPARE(perft(b, 1), Q_UINT64_C(41));
	QCOMPARE(perft(b, 2), Q_UINT64_C(2107));
	QCOMPARE(perft(b, 3), Q_UINT64_C(93962));
	QCOMPARE(perft(b, 4), Q_UINT64_C(4869569));
}

QTEST_MAIN(TestBoard)
#include "test_board.moc"


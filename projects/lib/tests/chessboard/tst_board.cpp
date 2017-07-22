#include <QtTest/QtTest>
#include <QtConcurrentRun>
#include <board/board.h>
#include <board/boardfactory.h>


class tst_Board: public QObject
{
	Q_OBJECT

	public:
		tst_Board();
	
	private slots:
		void zobristKeys_data() const;
		void zobristKeys();
		
		void moveStrings_data() const;
		void moveStrings();
		
		void results_data() const;
		void results();

		void perft_data() const;
		void perft();

		void cleanupTestCase();
	
	private:
		void setVariant(const QString& variant);
		Chess::Board* m_board;
};


tst_Board::tst_Board()
	: m_board(0)
{
}

void tst_Board::cleanupTestCase()
{
	delete m_board;
}

void tst_Board::setVariant(const QString& variant)
{
	if (m_board == 0 || m_board->variant() != variant)
	{
		delete m_board;
		m_board = Chess::BoardFactory::create(variant);
	}
	QVERIFY(m_board != 0);
}

static quint64 perftVal(Chess::Board* board, int depth)
{
	quint64 nodeCount = 0;
	QVector<Chess::Move> moves(board->legalMoves());
	if (depth == 1 || moves.size() == 0)
		return moves.size();

	QVector<Chess::Move>::const_iterator it;
	for (it = moves.begin(); it != moves.end(); ++it)
	{
		board->makeMove(*it);
		nodeCount += perftVal(board, depth - 1);
		board->undoMove();
	}

	return nodeCount;
}

static quint64 perftRoot(const Chess::Board* board,
			 const Chess::Move& move,
			 int depth)
{
	Chess::Board* tmp = board->copy();
	Q_ASSERT(tmp != 0);
	tmp->makeMove(move);

	quint64 val = perftVal(tmp, depth - 1);
	delete tmp;
	return val;
}

static quint64 smpPerft(Chess::Board* board, int depth)
{
	const auto moves = board->legalMoves();
	if (depth <= 1)
		return moves.size();
	
	QVector< QFuture<quint64> > futures;
	for (const auto& move : moves)
		futures << QtConcurrent::run(perftRoot, board, move, depth);

	quint64 nodeCount = 0;
	// TODO: use qAsConst() from Qt 5.7
	foreach (const QFuture<quint64>& future, futures)
		nodeCount += future.result();

	return nodeCount;
}


void tst_Board::zobristKeys_data() const
{
	QTest::addColumn<QString>("variant");
	QTest::addColumn<QString>("fen");
	QTest::addColumn<quint64>("key");
	
	QString variant = "standard";

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

void tst_Board::zobristKeys()
{
	QFETCH(QString, variant);
	QFETCH(QString, fen);
	QFETCH(quint64, key);

	setVariant(variant);
	QVERIFY(m_board->setFenString(fen));
	QCOMPARE(m_board->key(), key);
}

void tst_Board::moveStrings_data() const
{
	QTest::addColumn<QString>("variant");
	QTest::addColumn<QString>("moves");
	QTest::addColumn<QString>("startfen");
	QTest::addColumn<QString>("endfen");
	
	QTest::newRow("san1")
		<< "standard"
		<< "e4 Nc6 e5 d5 exd6 Be6 Nf3 Qd7 Bb5 O-O-O dxc7 a6 O-O Qxd2 "
		   "cxd8=N Qxc1 Bxc6 Qxd1 Nxb7 Qxf1+ Kxf1 Bxa2 Rxa2 Kc7"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "5bnr/1Nk1pppp/p1B5/8/8/5N2/RPP2PPP/1N3K2 w - - 1 13";
	QTest::newRow("coord1")
		<< "standard"
		<< "e2e4 b8c6 e4e5 d7d5 e5d6 c8e6 g1f3 d8d7 f1b5 e8c8 d6c7 "
		   "a7a6 e1g1 d7d2 c7d8n d2c1 b5c6 c1d1 d8b7 d1f1 g1f1 e6a2 "
		   "a1a2 c8c7"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "5bnr/1Nk1pppp/p1B5/8/8/5N2/RPP2PPP/1N3K2 w - - 1 13";
	QTest::newRow("san2")
		<< "standard"
		<< "Qd4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 1";
	QTest::newRow("coord2")
		<< "standard"
		<< "g7d4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 1";
	QTest::newRow("san3")
		<< "standard"
		<< "Qf6"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pbq1p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 1";
	QTest::newRow("coord3")
		<< "standard"
		<< "g7f6"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pbq1p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 1";
	QTest::newRow("crazyhouse1")
		<< "crazyhouse"
		<< "Qd4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K[-] b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K[-] w - - 3 1";
	QTest::newRow("crazyhouse2")
		<< "crazyhouse"
		<< "d4 h6 Bxh6 gxh6 g4 h5 gxh5"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[-] w KQkq - 0 1"
		<< "rnbqkbnr/pppppp2/8/7P/3P4/8/PPP1PP1P/RN1QKBNR[PPb] b KQkq - 0 4";
	QTest::newRow("crazyhouse3")
		<< "crazyhouse"
		<< "N@f3+ gxf3"
		<< "r1bqkbnr/pp3ppp/2ppp3/8/2BQP3/2N5/PPP2PPP/R1B2RK1[NPn] b kq - 0 1"
		<< "r1bqkbnr/pp3ppp/2ppp3/8/2BQP3/2N2P2/PPP2P1P/R1B2RK1[NNP] b kq - 0 2";
	QTest::newRow("crazyhouse4")
		<< "crazyhouse"
		<< ""
		<< "r1bqkbnr/pp3ppp/2ppp3/8/2BQP3/2N5/PPP2PPP/R1B2RK1 b kq - 0 1"
		<< "r1bqkbnr/pp3ppp/2ppp3/8/2BQP3/2N5/PPP2PPP/R1B2RK1[-] b kq - 0 1";
	QTest::newRow("crazyhouse5")
		<< "crazyhouse"
		<< ""
		<< "r1bqkbnr/pp3ppp/2ppp3/8/2BQP3/2N5/PPP2PPP/R1B2RK1[] b kq - 0 1"
		<< "r1bqkbnr/pp3ppp/2ppp3/8/2BQP3/2N5/PPP2PPP/R1B2RK1[-] b kq - 0 1";
	QTest::newRow("berolina san1a")
		<< "berolina"
		<< "e2c4 Nc6 c4d5 e7c5 d5xd6 Bxd6 Qe2+ Nge7 a2c4 O-O"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bq1rk1/ppppnppp/2nb4/8/2P5/8/1PPPQPPP/RNB1KBNR w KQ - 0 6";
	QTest::newRow("berolina san1b")
		<< "berolina"
		<< "ec4 Nc6 cd5 ec5 dxd6 Bxd6 Qe2+ Nge7 ac4 O-O"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bq1rk1/ppppnppp/2nb4/8/2P5/8/1PPPQPPP/RNB1KBNR w KQ - 0 6";
	QTest::newRow("berolina san1c")
		<< "berolina"
		<< "ec4 Nc6 d5 ec5 dxd6 Bxd6 Qe2 Nge7 c4 O-O"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bq1rk1/ppppnppp/2nb4/8/2P5/8/1PPPQPPP/RNB1KBNR w KQ - 0 6";
	QTest::newRow("berolina coord1")
		<< "berolina"
		<< "e2c4 b8c6 c4d5 e7c5"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bqkbnr/pppp1ppp/2n5/2pP4/8/8/PPPP1PPP/RNBQKBNR w KQkq d6c5 0 3";
	QTest::newRow("berolina san2")
		<< "berolina"
		<< "Qd4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 1";
	QTest::newRow("3check san1")
		<< "3check"
		<< "e4 e5 Nf3 Nc6 d4 exd4"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 3+3 0 1"
		<< "r1bqkbnr/pppp1ppp/2n5/8/3pP3/5N2/PPP2PPP/RNBQKB1R w KQkq - 3+3 0 4";
	QTest::newRow("3check san2")
		<< "3check"
		<< "e4 e5 Nf3 d6 Bb5+ c6 Bxc6+"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 3+3 0 1"
		<< "rnbqkbnr/pp3ppp/2Bp4/4p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 1+3 0 4";
	QTest::newRow("janus castling san1")
		<< "janus"
		<< "Kb8 Be3 Ng6 Ki1"
		<< "r3kqbnjr/pppp1ppppp/2jn1b4/4p5/4P5/3Q1PNJ2/PPPP2PPPP/RJNBK1B2R b KQkq - 0 1"
		<< "1kr2qb1jr/pppp1ppppp/2jn1bn3/4p5/4P5/3QBPNJ2/PPPP2PPPP/RJNB3RK1 b - - 0 3";
	QTest::newRow("janus castling san2")
		<< "janus"
		<< "O-O Be3 Ng6 O-O-O"
		<< "r3kqbnjr/pppp1ppppp/2jn1b4/4p5/4P5/3Q1PNJ2/PPPP2PPPP/RJNBK1B2R b KQkq - 0 1"
		<< "1kr2qb1jr/pppp1ppppp/2jn1bn3/4p5/4P5/3QBPNJ2/PPPP2PPPP/RJNB3RK1 b - - 0 3";
	QTest::newRow("janus castling lan")
		<< "janus"
		<< "e8b8 g1e3 h8g6 e1i1"
		<< "r3kqbnjr/pppp1ppppp/2jn1b4/4p5/4P5/3Q1PNJ2/PPPP2PPPP/RJNBK1B2R b KQkq - 0 1"
		<< "1kr2qb1jr/pppp1ppppp/2jn1bn3/4p5/4P5/3QBPNJ2/PPPP2PPPP/RJNB3RK1 b - - 0 3";
	QTest::newRow("knightmate castling san1")
		<< "knightmate"
		<< "O-O Be6 Bxe6 Qxe6 Re1 O-O-O"
		<< "r1b1kbmr/pmp2ppp/1p1p1q2/4p3/2B1P3/1P6/P1PPMPPP/RMBQK2R w KQkq - 0 1"
		<< "2kr1bmr/pmp2ppp/1p1pq3/4p3/4P3/1P6/P1PPMPPP/RMBQR1K1 w - - 0 4";
	QTest::newRow("knightmate castling san2")
		<< "knightmate"
		<< "Kg1 Be6 Bxe6 Qxe6 Re1 O-O-O"
		<< "r1b1kbmr/pmp2ppp/1p1p1q2/4p3/2B1P3/1P6/P1PPMPPP/RMBQK2R w KQkq - 0 1"
		<< "2kr1bmr/pmp2ppp/1p1pq3/4p3/4P3/1P6/P1PPMPPP/RMBQR1K1 w - - 0 4";
}

void tst_Board::moveStrings()
{
	QFETCH(QString, variant);
	QFETCH(QString, moves);
	QFETCH(QString, startfen);
	QFETCH(QString, endfen);

	setVariant(variant);
	QVERIFY(m_board->setFenString(startfen));

	const auto moveList = moves.split(' ', QString::SkipEmptyParts);
	for (const auto& moveStr : moveList)
	{
		Chess::Move move = m_board->moveFromString(moveStr);
		QVERIFY(m_board->isLegalMove(move));
		m_board->makeMove(move);
	}
	QCOMPARE(m_board->fenString(), endfen);

	if (!moveList.isEmpty())
	{
		for (int i = 0; i < moveList.size(); i++)
			m_board->undoMove();
		QCOMPARE(m_board->fenString(), startfen);
	}
	else
		QCOMPARE(m_board->fenString(), endfen);
}

void tst_Board::results_data() const
{
	QTest::addColumn<QString>("variant");
	QTest::addColumn<QString>("fen");
	QTest::addColumn<QString>("result");

	QString variant = "standard";

	QTest::newRow("standard startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "*";
	QTest::newRow("unfinished #1")
		<< variant
		<< "2K1k3/8/2r5/8/8/8/8/8 w - - 0 1"
		<< "*";
	QTest::newRow("mate #1")
		<< variant
		<< "2K1r3/8/2k5/8/8/8/8/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("stalemate #1")
		<< variant
		<< "k7/8/K7/8/1R6/8/8/8 b - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("Kk")
		<< variant
		<< "2K5/8/2k5/8/8/8/8/8 w - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("KNk")
		<< variant
		<< "2K5/4N3/2k5/8/8/8/8/8 b - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("KBk")
		<< variant
		<< "2K5/4B3/2k5/8/8/8/8/8 b - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("KBNk")
		<< variant
		<< "2K5/4BN2/2k5/8/8/8/8/8 b - - 0 1"
		<< "*";
	QTest::newRow("KBBk #1")
		<< variant
		<< "2K5/4BB2/2k5/8/8/8/8/8 b - - 0 1"
		<< "*";
	QTest::newRow("KBBkb #1")
		<< variant
		<< "2K5/4B3/2k2B2/8/8/3b4/8/8 b - - 0 1"
		<< "*";
	QTest::newRow("KBBk #2")
		<< variant
		<< "2K5/4B3/2k2B2/8/8/8/8/8 b - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("KBBkb #2")
		<< variant
		<< "2K5/4B3/2k2B2/8/8/4b3/8/8 b - - 0 1"
		<< "1/2-1/2";

	variant = "kingofthehill";

	QTest::newRow("KOTH unfinished #1")
		<< variant
		<< "r1b3nr/1pp2ppp/p7/2kp4/BP1p4/2PK1N2/P4PPP/R1R5 b - - 0 18"
		<< "*";
	QTest::newRow("KOTH center #1")
		<< variant
		<< "r1b3nr/1pp2ppp/pk6/3p4/BP1K4/2P2N2/P4PPP/R1R5 b - - 0 19"
		<< "1-0";
	QTest::newRow("KOTH center #2")
		<< variant
		<< "1q6/8/2B2p2/4k3/P1p5/1p1pn3/5K2/8 w - - 1 71"
		<< "0-1";

	variant = "racingkings";

	QTest::newRow("Race black win")
		<< variant
		<< "6k1/3K4/3B4/8/8/8/8/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("Race unfinished")
		<< variant
		<< "6K1/3k4/3b4/8/8/8/8/8 b - - 0 1"
		<< "*";
	QTest::newRow("Race white win")
		<< variant
		<< "6K1/8/3bk3/8/8/8/8/8 b - - 0 1"
		<< "1-0";
	QTest::newRow("Race drawn")
		<< variant
		<< "3k2K1/8/3b4/8/8/8/8/8 w - - 1 1"
		<< "1/2-1/2";

	variant = "horde";

	QTest::newRow("Horde black win")
		<< variant
		<< "8/8/4k3/8/8/8/8/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("Horde draw stalemate")
		<< variant
		<< "8/8/1k6/5p2/5P2/5P2/8/8 w - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("Horde white win")
		<< variant
		<< "2k4R/R7/8/3PP3/8/8/8/8 b - - 0 1"
		<< "1-0";

	variant = "extinction";

	QTest::newRow("extinction white win")
		<< variant
		<< "6k1/5p2/7p/3p4/p2pr3/b1BQ2Pq/r1B1P1RP/N6K b - - 0 34"
		<< "1-0";
	QTest::newRow("extinction black win")
		<< variant
		<< "rn1qkbnr/ppp1pppp/3p4/8/4P1b1/8/PPPP1PPP/RNB1KBNR w KQkq - 0 3"
		<< "0-1";

	variant = "kinglet";

	QTest::newRow("kinglet white win")
		<< variant
		<< "8/8/4P3/2Q5/1P6/1P6/8/8 b - - 0 1"
		<< "1-0";
	QTest::newRow("kinglet black win")
		<< variant
		<< "2k5/1pp3R1/6p1/r7/8/4K3/8/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("kinglet two")
		<< variant
		<< "8/8/6p1/8/8/4P3/8/8 w - - 0 1"
		<< "*";

	variant = "checkless";

	QTest::newRow("checkless white win")
		<< variant
		<< "K5kr/5Q2/8/8/8/8/8/8 b - - 0 1"
		<< "1-0";
}

void tst_Board::results()
{
	QFETCH(QString, variant);
	QFETCH(QString, fen);
	QFETCH(QString, result);

	setVariant(variant);
	QVERIFY(m_board->setFenString(fen));
	QCOMPARE(m_board->result().toShortString(), result);
}

void tst_Board::perft_data() const
{
	QTest::addColumn<QString>("variant");
	QTest::addColumn<QString>("fen");
	QTest::addColumn<int>("depth");
	QTest::addColumn<quint64>("nodecount");
	
	QString variant = "standard";
	
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

	variant = "capablanca";
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

	variant = "fischerandom";
	QTest::newRow("frc1")
		<< variant
		<< "1rk3r1/8/8/8/8/8/8/1RK1R3 w EBgb -"
		<< 2
		<< Q_UINT64_C(464);
	QTest::newRow("frc2")
		<< variant
		<< "bnrbnkrq/pppppppp/8/8/8/8/PPPPPPPP/BNRBNKRQ w KQkq - 0 1"
		<< 4
		<< Q_UINT64_C(233585);
	QTest::newRow("frc3")
		<< variant
		<< "2rkr3/5PP1/8/5Q2/5q2/8/5pp1/2RKR3 w KQkq - 0 1"
		<< 3
		<< Q_UINT64_C(71005);
	QTest::newRow("frc4")
		<< variant
		<< "2Rnb1kr/5ppp/8/q3p3/p3P3/4P3/6PP/1Q3BKR b Hh - 0 15"
		<< 3
		<< Q_UINT64_C(24750);

	variant = "crazyhouse";
	QTest::newRow("crazyhouse startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5
		<< Q_UINT64_C(4888832);
	QTest::newRow("crazyhouse promo1")
		<< variant
		<< "3q1bkr/2p1pBp1/q1n3p1/1N2p3/1Pp5/P4Q~2/BBPp1PPP/R2K2NR[RPPn] b - - 0 28"
		<< 3
		<< Q_UINT64_C(6386);

	variant = "loop";
	QTest::newRow("loop startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[-] w KQkq - 0 1"
		<< 5  //4 plies: 197281, 5 plies: 4888832
		<< Q_UINT64_C(4888832);
	QTest::newRow("loop2")
		<< variant
		<< "5R2/2p1Nb2/2B4k/6p1/8/P3PP2/1PPqR3/3R1BKn[QBNPPPPrrrnppp] b - - 1 1"
		<< 3 // 1 ply:157, 2 plies: 31983, 3 plies: 4144334
		<< Q_UINT64_C(4144334);

	variant = "chessgi";
	QTest::newRow("chessgi startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[-] w KQkq - 0 1"
		<< 5  //4 plies: 197281, 5 plies: 4889167
		<< Q_UINT64_C(4889167);
	QTest::newRow("chessgi2")
		<< variant
		<< "5Rp1/2p1Nb2/2B4k/6p1/8/P3PP2/1PPqR3/3R1BKn[QBNPPPPrrrnpp] b - - 1 48"
		<< 3 // 1 ply:162, 2 plies: 33032, 3 plies: 4493963
		<< Q_UINT64_C(4493963);
		// TBD sjaakii (/wo dbl steps from first rank) 1 ply:161, 2 plies: 32816, 3 plies: 4434101

	variant = "berolina";
	QTest::newRow("berolina startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 4  //4 plies: 882717, 5 plies: 29119802
		<< Q_UINT64_C(882717);
	QTest::newRow("berolina1")
		<< variant
		<< "3k1r2/1pp3p1/3P2p1/PR4p1/6p1/2P2Pn1/b3r1BP/3KN3 b - - 1 26"
		<< 4  //4 plies: 909365, 5 plies: 37601709
		<< Q_UINT64_C(909365);

	variant = "racingkings";
	QTest::newRow("racingkings startpos")
		<< variant
		<< "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1"
		<< 4  //4 plies: 296242, 5 plies: 9472927
		<< Q_UINT64_C(296242);

	variant = "horde";
	QTest::newRow("horde v2 startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1"
		<< 6  //5 plies: 265223, 6 plies: 5396554
		<< Q_UINT64_C(5396554);
	QTest::newRow("horde dunsany startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP b kq - 0 1"
		<< 5  //5 plies: 775839, 6 plies: 59231536
		<< Q_UINT64_C(775839);
	QTest::newRow("horde3")
		<< variant
		<< "rnbqkbnr/6p1/2p1Pp1P/P1PPPP2/Pp4PP/1p2PPPP/1P2PPPP/PP1nPPPP b kq a3 0 18"
		<< 5  //4 plies: 197287, 5 plies: 6429490
		<< Q_UINT64_C(6429490);

	variant = "andernach";
	QTest::newRow("andernach startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5  //4 plies: 197410, 5 plies: 4870137
		<< Q_UINT64_C(4870137);
	QTest::newRow("andernach pos1")
		<< variant
		<< "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"
		<< 4  //4 plies: 523348, 5 plies: 16330793
		<< Q_UINT64_C(523348);

	variant = "checkless";
	QTest::newRow("checkless startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5
		<< Q_UINT64_C(4835050);

	variant = "extinction";
	QTest::newRow("extinction startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5
		<< Q_UINT64_C(4897256);

	variant = "janus";
	QTest::newRow("janus startpos")
		<< variant
		<< "rjnbkqbnjr/pppppppppp/10/10/10/10/PPPPPPPPPP/RJNBKQBNJR w KQkq - 0 1"
		<< 4 //4 plies: 772074, 5 plies: 26869186
		<< Q_UINT64_C(772074);

	variant = "knightmate";
	QTest::newRow("knightmate startpos")
		<< variant
		<< "rmbqkbmr/pppppppp/8/8/8/8/PPPPPPPP/RMBQKBMR w KQkq - 0 1"
		<< 5 //4 plies: 139774, 5 plies: 3249033, 6 plies: 74568983
		<< Q_UINT64_C(3249033);
}

void tst_Board::perft()
{
	QFETCH(QString, variant);
	QFETCH(QString, fen);
	QFETCH(int, depth);
	QFETCH(quint64, nodecount);

	setVariant(variant);
	QVERIFY(m_board->setFenString(fen));
	QCOMPARE(smpPerft(m_board, depth), nodecount);
}

QTEST_MAIN(tst_Board)
#include "tst_board.moc"

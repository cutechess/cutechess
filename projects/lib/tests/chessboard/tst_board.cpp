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
	for (const QFuture<quint64>& future : qAsConst(futures))
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
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 2";
	QTest::newRow("coord2")
		<< "standard"
		<< "g7d4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 2";
	QTest::newRow("san3")
		<< "standard"
		<< "Qf6"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pbq1p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 2";
	QTest::newRow("coord3")
		<< "standard"
		<< "g7f6"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 11"
		<< "3r1rk1/8/1p1pbq1p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 12";
	QTest::newRow("fifty moves draw")
		<< "standard"
		<< "h2a2"
		<< "8/k7/2K5/8/8/8/7R/8 w - - 99 61"
		<< "8/k7/2K5/8/8/8/R7/8 b - - 100 61";
	QTest::newRow("fifty moves mate")
		<< "standard"
		<< "h2a2"
		<< "8/8/k1K5/8/8/8/7R/8 w - - 99 1"
		<< "8/8/k1K5/8/8/8/R7/8 b - - 100 1";
	QTest::newRow("atomic1")
		<< "atomic"
		<< "Rxh3"
		<< "8/8/8/7K/8/6kp/6N1/7R w - - 16 1"
		<< "8/8/8/7K/8/8/8/8 b - - 0 1";
	QTest::newRow("crazyhouse1")
		<< "crazyhouse"
		<< "Qd4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K[-] b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K[-] w - - 3 2";
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
		<< "r1bq1rk1/ppppnppp/2nb4/8/2P5/8/1PPPQPPP/RNB1KBNR w KQ - 1 6";
	QTest::newRow("berolina san1b")
		<< "berolina"
		<< "ec4 Nc6 cd5 ec5 dxd6 Bxd6 Qe2+ Nge7 ac4 O-O"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bq1rk1/ppppnppp/2nb4/8/2P5/8/1PPPQPPP/RNB1KBNR w KQ - 1 6";
	QTest::newRow("berolina san1c")
		<< "berolina"
		<< "ec4 Nc6 d5 ec5 dxd6 Bxd6 Qe2 Nge7 c4 O-O"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bq1rk1/ppppnppp/2nb4/8/2P5/8/1PPPQPPP/RNB1KBNR w KQ - 1 6";
	QTest::newRow("berolina coord1")
		<< "berolina"
		<< "e2c4 b8c6 c4d5 e7c5"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "r1bqkbnr/pppp1ppp/2n5/2pP4/8/8/PPPP1PPP/RNBQKBNR w KQkq d6c5 0 3";
	QTest::newRow("berolina san2")
		<< "berolina"
		<< "Qd4"
		<< "3r1rk1/6q1/1p1pb2p/p1p2np1/P1P2p2/1PNP4/1Q2PPBP/1R2R2K b - - 2 1"
		<< "3r1rk1/8/1p1pb2p/p1p2np1/P1Pq1p2/1PNP4/1Q2PPBP/1R2R2K w - - 3 2";
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
	QTest::newRow("embassy castling san1")
		<< "embassy"
		<< "O-O-O O-O-O Ng4"
		<< "r3kcab1r/pppq1p1ppp/2np2pn2/4pb4/4B5/2NPB1PN2/PPPQPP1PPP/R3KCA2R b KQkq - 3 1"
		<< "1kr2cab1r/pppq1p1ppp/2np2p3/4pb4/4B1n3/2NPB1PN2/PPPQPP1PPP/1KR2CA2R w - - 6 3";
	QTest::newRow("embassy castling san2")
		<< "embassy"
		<< "Cf1 O-O g3"
		<< "r2qk4r/ppp4cpp/3p1ppba1/4nb1p2/2P1N1nB2/P1KP2CNP1/1P2P1PP1P/R5AB1R w kq - 0 1"
		<< "r2q2rk2/ppp4cpp/3p1ppba1/4nb1p2/2P1N1nB2/P1KP2PNP1/1P2P2P1P/R4CAB1R b - - 0 2";
	QTest::newRow("embassy castling lan")
		<< "embassy"
		<< "e8b8 e1b1 h6g4"
		<< "r3kcab1r/pppq1p1ppp/2np2pn2/4pb4/4B5/2NPB1PN2/PPPQPP1PPP/R3KCA2R b KQkq - 3 1"
		<< "1kr2cab1r/pppq1p1ppp/2np2p3/4pb4/4B1n3/2NPB1PN2/PPPQPP1PPP/1KR2CA2R w - - 6 3";
	QTest::newRow("janus castling san1")
		<< "janus"
		<< "Kb8 Be3 Ng6 Ki1"
		<< "r3kqbnjr/pppp1ppppp/2jn1b4/4p5/4P5/3Q1PNJ2/PPPP2PPPP/RJNBK1B2R b KQkq - 0 1"
		<< "1kr2qb1jr/pppp1ppppp/2jn1bn3/4p5/4P5/3QBPNJ2/PPPP2PPPP/RJNB3RK1 b - - 4 3";
	QTest::newRow("janus castling san2")
		<< "janus"
		<< "O-O Be3 Ng6 O-O-O!"
		<< "r3kqbnjr/pppp1ppppp/2jn1b4/4p5/4P5/3Q1PNJ2/PPPP2PPPP/RJNBK1B2R b KQkq - 0 1"
		<< "1kr2qb1jr/pppp1ppppp/2jn1bn3/4p5/4P5/3QBPNJ2/PPPP2PPPP/RJNB3RK1 b - - 4 3";
	QTest::newRow("janus castling lan")
		<< "janus"
		<< "e8b8 g1e3 h8g6 e1i1"
		<< "r3kqbnjr/pppp1ppppp/2jn1b4/4p5/4P5/3Q1PNJ2/PPPP2PPPP/RJNBK1B2R b KQkq - 0 1"
		<< "1kr2qb1jr/pppp1ppppp/2jn1bn3/4p5/4P5/3QBPNJ2/PPPP2PPPP/RJNB3RK1 b - - 4 3";
	QTest::newRow("knightmate castling san1")
		<< "knightmate"
		<< "O-O Be6 Bxe6 Qxe6 Re1 O-O-O!"
		<< "r1b1kbmr/pmp2ppp/1p1p1q2/4p3/2B1P3/1P6/P1PPMPPP/RMBQK2R w KQkq - 0 1"
		<< "2kr1bmr/pmp2ppp/1p1pq3/4p3/4P3/1P6/P1PPMPPP/RMBQR1K1 w - - 2 4";
	QTest::newRow("knightmate castling san2")
		<< "knightmate"
		<< "Kg1 Be6 Bxe6 Qxe6 Re1 O-O-O"
		<< "r1b1kbmr/pmp2ppp/1p1p1q2/4p3/2B1P3/1P6/P1PPMPPP/RMBQK2R w KQkq - 0 1"
		<< "2kr1bmr/pmp2ppp/1p1pq3/4p3/4P3/1P6/P1PPMPPP/RMBQR1K1 w - - 2 4";
	QTest::newRow("chancellor castling san")
		<< "chancellor"
		<< "O-O O-O-O"
		<< "r3k3r/ppp3ppp/9/9/9/9/9/PPP3PPP/R3K3R w KQkq - 0 1"
		<< "2kr4r/ppp3ppp/9/9/9/9/9/PPP3PPP/R4RK2 w - - 2 2";
	QTest::newRow("chancellor castling lan")
		<< "chancellor"
		<< "e1g1 e9c9"
		<< "r3k3r/ppp3ppp/9/9/9/9/9/PPP3PPP/R3K3R w KQkq - 0 1"
		<< "2kr4r/ppp3ppp/9/9/9/9/9/PPP3PPP/R4RK2 w - - 2 2";
	QTest::newRow("modern castling san1")
		<< "modern"
		<< "O-Q-O O-M-O"
		<< "r1bqk3r/ppp3ppp/2n3n2/2mbpp3/3p5/3B1P3/P1NQPMN2/1PPP2PPP/R1B1K3R w KQkq - 0 1"
		<< "r1bq1rk2/ppp3ppp/2n3n2/2mbpp3/3p5/3B1P3/P1NQPMN2/1PPP2PPP/R1B2RK2 w - - 2 2";
	QTest::newRow("modern castling san2")
		<< "modern"
		<< "O-O O-O"
		<< "r1bqk3r/ppp3ppp/2n3n2/2mbpp3/3p5/3B1P3/P1NQPMN2/1PPP2PPP/R1B1K3R w KQkq - 0 1"
		<< "r1bq1rk2/ppp3ppp/2n3n2/2mbpp3/3p5/3B1P3/P1NQPMN2/1PPP2PPP/R1B2RK2 w - - 2 2";
	QTest::newRow("modern castling lan")
		<< "modern"
		<< "e1g1 e9g9"
		<< "r1bqk3r/ppp3ppp/2n3n2/2mbpp3/3p5/3B1P3/P1NQPMN2/1PPP2PPP/R1B1K3R w KQkq - 0 1"
		<< "r1bq1rk2/ppp3ppp/2n3n2/2mbpp3/3p5/3B1P3/P1NQPMN2/1PPP2PPP/R1B2RK2 w - - 2 2";
	QTest::newRow("circular gryphon san")
		<< "circulargryphon"
		<< "Qxa7 e3 a5 Kb4 Nc6 Nc4+ Ke7 Ba6 Ng4 h4 Be2"
		<< "8/R4k2/5n2/8/5n2/2K5/q3P2P/8 b - - 0 1"
		<< "8/4k3/R1b5/8/1K3n1N/8/4r3/8 w - - 1 7";
	QTest::newRow("giveaway san1")
		<< "giveaway"
		<< "e3"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< "rnbqkbnr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR b KQkq - 0 1";
	QTest::newRow("suicide san1")
		<< "suicide"
		<< "e3"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"
		<< "rnbqkbnr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR b - - 0 1";
	QTest::newRow("twokings castling san1")
		<< "twokings"
		<< "d4 d5 Nc3 Nf6 Bg5 Ne4 Nxe4 dxe4 Bf4 Be6 f3 Qd5 Bxc7 Nd7 "
		   "fxe4 Qxe4 Qd3 Qd5 e4 Qc6 Ba5 Bc4 Qf3 Bxf1 Qxf1 Qxe4+ Qe2 "
		   "Qxd4 Bc3 Qh4+ g3 Qa4 Nf3 Qa6 Qxa6 bxa6 Ba5 h5 O-O-O"
		<< "rnbqkknr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKKNR w KQkq - 0 1"
		<< "r3kk1r/p2nppp1/p7/B6p/8/5NP1/PPP4P/2KR3R b kq - 1 20";
	QTest::newRow("twokings castling san2")
		<< "twokings"
		<< "d4 d5 Nc3 Nf6 Bg5 Ne4 Nxe4 dxe4 Bf4 Be6 f3 Qd5 Bxc7 Nd7 "
		   "fxe4 Qxe4 Qd3 Qd5 e4 Qc6 Ba5 Bc4 Qf3 Bxf1 Qxf1 Qxe4+ Qe2 "
		   "Qxd4 Bc3 Qh4+ g3 Qa4 Nf3 Qa6 Qxa6 bxa6 Ba5 h5 O-O"
		<< "rnbqkknr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKKNR w KQkq - 0 1"
		<< "r3kk1r/p2nppp1/p7/B6p/8/5NP1/PPP4P/R4RK1 b kq - 1 20";
	QTest::newRow("grand lan1")
		<< "grand"
		<< "d10d9 f5f6 g8g6 f6g7"
		<< "3r6/1nb1kcabn1/ppprpppppp/3q6/3p6/5P4/5C4/PPPPPRPPPP/1NBQKRABN1/10 b - - 0 1"
		<< "10/1nbrkcabn1/ppprpp1ppp/3q2P3/3p6/10/5C4/PPPPPRPPPP/1NBQKRABN1/10 b - - 0 3";
	QTest::newRow("grand san1")
		<< "grand"
		<< "R10d9 f6 g6 fxg7"
		<< "3r6/1nb1kcabn1/ppprpppppp/3q6/3p6/5P4/5C4/PPPPPRPPPP/1NBQKRABN1/10 b - - 0 1"
		<< "10/1nbrkcabn1/ppprpp1ppp/3q2P3/3p6/10/5C4/PPPPPRPPPP/1NBQKRABN1/10 b - - 0 3";
	QTest::newRow("grand san2")
		<< "grand"
		<< "Rc6 c3=C+ Rxc3 Rxc3"
		<< "10/10/p9/10/1p5R2/1P1k6/P1p7/n4r4/K9/10 w - - 0 1"
		<< "10/10/p9/10/1p8/1P1k6/P9/n1r7/K9/10 w - - 0 3";
	QTest::newRow("grand san3")
		<< "grand"
		<< "R10a9 R10b9 Ra10 Rg9 R1a7 Rbb9 R7a8+ Kc9 Ke4 Ra9 R10xa9+ "
		   "Kd10 Ra10+ Kd9 Rj8 Re9+ Kd5 Kc9 Rjj10 Rd9+ Kc6 Kc8 Rjc10+"
		<< "Rr8/10/1r1k6/10/10/10/10/5K4/10/R9 w - - 0 1"
		<< "R1R7/3r6/2k7/10/2K7/10/10/10/10/10 b - - 12 12";
	QTest::newRow("seirawan san1")
		<< "seirawan"
		<< "e4 e5 Nf3 Nf6/H Bc4 Hh6 O-O/Eh1 Bd6 Nc3/H O-O/Ee8"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[EHeh] w bcdfgBCDFGKQkq - 0 1"
		<< "rnbqerk1/pppp1ppp/3b1n1h/4p3/2B1P3/2N2N2/PPPP1PPP/RHBQ1RKE[-] w - - 0 6";
	QTest::newRow("seirawan castling san1")
		<< "seirawan"
		<< "O-O/Ee1 O-O/Hh8"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r4rkh/pppppppp/8/8/8/8/PPPPPPPP/R3ERK1[He] w aA - 0 2";
	QTest::newRow("seirawan castling san2")
		<< "seirawan"
		<< "O-O/Eh1 O-O-O/He8"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "2krh2r/pppppppp/8/8/8/8/PPPPPPPP/R4RKE[He] w hA - 0 2";
	QTest::newRow("seirawan castling san3")
		<< "seirawan"
		<< "O-O-O/Ea1 O-O-O/Ea8"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "e1kr3r/pppppppp/8/8/8/8/PPPPPPPP/E1KR3R[Hh] w hH - 0 2";
	QTest::newRow("seirawan castling san4")
		<< "seirawan"
		<< "O-O-O/He1 O-O/Ee8"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r3erk1/pppppppp/8/8/8/8/PPPPPPPP/2KRH2R[Eh] w aH - 0 2";
	QTest::newRow("seirawan castling san4b")
		<< "seirawan"
		<< "O-O-O/He O-O/Ee"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r3erk1/pppppppp/8/8/8/8/PPPPPPPP/2KRH2R[Eh] w aH - 0 2";
	QTest::newRow("seirawan castling san4c")
		<< "seirawan"
		<< "O-O-O/H O-O/E"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r3erk1/pppppppp/8/8/8/8/PPPPPPPP/2KRH2R[Eh] w aH - 0 2";
	QTest::newRow("seirawan castling san5")
		<< "seirawan"
		<< "O-O-O O-O"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r4rk1/pppppppp/8/8/8/8/PPPPPPPP/2KR3R[EHeh] w aH - 2 2";
	QTest::newRow("seirawan castling san6")
		<< "seirawan"
		<< "O-O-O O-O Rhg1/E Rac8/H"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "h1r2rk1/pppppppp/8/8/8/8/PPPPPPPP/2KR2RE[He] w - - 0 3";
	QTest::newRow("seirawan lan1")
		<< "seirawan"
		<< "e2e4 e7e5 g1f3 g8f6h f1c4 g8h6 h1e1e f8d6 b1c3h e8g8e"
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[EHeh] w bcdfgBCDFGKQkq - 0 1"
		<< "rnbqerk1/pppp1ppp/3b1n1h/4p3/2B1P3/2N2N2/PPPP1PPP/RHBQ1RKE[-] w - - 0 6";
	QTest::newRow("seirawan castling lan1")
		<< "seirawan"
		<< "e1g1e h8e8h"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r4rkh/pppppppp/8/8/8/8/PPPPPPPP/R3ERK1[He] w aA - 0 2";
	QTest::newRow("seirawan castling lan2")
		<< "seirawan"
		<< "h1e1e e8c8h"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "2krh2r/pppppppp/8/8/8/8/PPPPPPPP/R4RKE[He] w hA - 0 2";
	QTest::newRow("seirawan castling lan3")
		<< "seirawan"
		<< "a1e1e a8e8e"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "e1kr3r/pppppppp/8/8/8/8/PPPPPPPP/E1KR3R[Hh] w hH - 0 2";
	QTest::newRow("seirawan castling lan4")
		<< "seirawan"
		<< "e1c1h e8g8e"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r3erk1/pppppppp/8/8/8/8/PPPPPPPP/2KRH2R[Eh] w aH - 0 2";
	QTest::newRow("seirawan castling lan5")
		<< "seirawan"
		<< "e1c1 e8g8"
		<< "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R[EHeh] w KQkq - 0 1"
		<< "r4rk1/pppppppp/8/8/8/8/PPPPPPPP/2KR3R[EHeh] w aH - 2 2";
	QTest::newRow("makruk lan1a")
		<< "makruk"
		<< "e3e4 b8d7 b3b4 a6a5 c1b2 h6h5 f1g2 h5h4 g3h4 h8h4 g2g3 h4h7"
		<< "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - 0 0 1"
		<< "r1smksn1/3n3r/1pppppp1/p7/1P2P3/P1PP1PSP/1S6/RN1KM1NR w - 0 0 7";
	QTest::newRow("makruk san1a")
		<< "makruk"
		<< "e4 Nd7 b4 a5 Sb2 h5 Sg2 h4 gxh4 Rxh4 Sg3 Rh7"
		<< "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - 0 0 1"
		<< "r1smksn1/3n3r/1pppppp1/p7/1P2P3/P1PP1PSP/1S6/RN1KM1NR w - 0 0 7";
	QTest::newRow("makruk san1b")
		<< "makruk"
		<< "e4 Nd7 b4 a5 Sb2 h5 Sg2 h4 gxh4 Rxh4 Sg3 Rh7"
		<< "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - - 0 1"
		<< "r1smksn1/3n3r/1pppppp1/p7/1P2P3/P1PP1PSP/1S6/RN1KM1NR w - - 2 7";
	QTest::newRow("makruk pawn promotion, board's honour counting")
		<< "makruk"
		<< "h3=M"
		<< "6r1/3M2k1/1M1K4/3N1M1R/7p/6m1/8/8 b - 0 0 93"
		<< "6r1/3M2k1/1M1K4/3N1M1R/8/6mm/8/8 w - 128 0 94";
	QTest::newRow("makruk board's to pieces' honour counting")
		<< "makruk"
		<< "Rxd6+ Nxd6"
		<< "8/5M2/2rM1K1k/5M2/4N1R1/8/8/8 b - 128 33 110"
		<< "8/5M2/3N1K1k/5M2/6R1/8/8/8 b - 32 12 111";
	QTest::newRow("cambodian san1")
		<< "cambodian"
		<< "e4 d5 Me3 Md6 Kb2 Kg7"
		<< "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w DEde 0 0 1"
		<< "rns2snr/6k1/pppmpppp/3p4/4P3/PPPPMPPP/1K6/RNS2SNR w - 0 0 4";
	QTest::newRow("asean san1b")
		<< "asean"
		<< "e4 Nd7 b4 a5 Bb2 h5 Bg2 h4 gxh4 Rxh4 Bg3 Rh7"
		<< "rnbqkbnr/8/pppppppp/8/8/PPPPPPPP/8/RNBKQBNR w - - 0 1"
		<< "r1bqkbn1/3n3r/1pppppp1/p7/1P2P3/P1PP1PBP/1B6/RN1KQ1NR w - - 2 7";
	QTest::newRow("asean san2 promotion")
		<< "asean"
		<< "h8=R Kc3"
		<< "8/7P/6K1/5R2/1k6/8/4b3/8 w - - 0 1"
		<< "7R/8/6K1/5R2/8/2k5/4b3/8 w - - 1 2";
	QTest::newRow("asean san3 pawn move")
		<< "asean"
		<< "h3 Nf1"
		<< "6k1/8/6p1/N5b1/5b1p/8/3NK3/8 b - - 0 1"
		<< "6k1/8/6p1/N5b1/5b2/7p/4K3/5N2 b - - 1 2";
	QTest::newRow("asean san4 no threefold rep adjudicated")
		<< "asean"
		<< "Kf5 Kb2 Ke5 Kb3 Kf5 Kb2 Ke5 Kb3 Kf5 Kb2 Ke5 Kb3 Kf5 Kb2 Ke5 Kb3"
		<< "8/8/8/4K3/8/1kq5/8/8 w - - 0 1"
		<< "8/8/8/4K3/8/1kq5/8/8 w - - 16 9";
	QTest::newRow("sittuyin lan1 promotion staying on square")
		<< "sittuyin"
		<< "g3g3f"
		<< "8/8/6R1/s3r3/P5R1/1KP3p1/1F2kr2/8[-] b - 0 0 72"
		<< "8/8/6R1/s3r3/P5R1/1KP3f1/1F2kr2/8[-] w - 0 0 73";
	QTest::newRow("sittuyin san1 promotion staying on square")
		<< "sittuyin"
		<< "g3=F"
		<< "8/8/6R1/s3r3/P5R1/1KP3p1/1F2kr2/8[-] b - 0 0 72"
		<< "8/8/6R1/s3r3/P5R1/1KP3f1/1F2kr2/8[-] w - 0 0 73";
	QTest::newRow("sittuyin san2 promotion by move")
		<< "sittuyin"
		<< "f2=F"
		<< "5R2/8/8/8/5n2/5k2/3K4/4p3[-] b - 0 0 81"
		<< "5R2/8/8/8/5n2/5k2/3K1f2/8[-] w - 0 0 82";
	QTest::newRow("sittuyin san3 no threefold rep adjudicated")
		<< "sittuyin"
		<< "Kf5 Kb2 Ke5 Kb3 Kf5 Kb2 Ke5 Kb3 Kf5 Kb2 Ke5 Kb3 Kf5 Kb2 Ke5 Kb3"
		<< "8/8/8/4K3/8/1kf5/8/8[-] w - - 0 1"
		<< "8/8/8/4K3/8/1kf5/8/8[-] w - - 16 9";
	QTest::newRow("losalamos san1")
		<< "losalamos"
		<< "d3 d4"
		<< "rnqknr/pppppp/6/6/PPPPPP/RNQKNR w - - 0 1"
		<< "rnqknr/ppp1pp/3p2/3P2/PPP1PP/RNQKNR w - - 0 2";

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
	QTest::newRow("No fifty moves yet #1")
		<< variant
		<< "8/k7/2K5/8/8/8/R7/8 b - - 99 1"
		<< "*";
	QTest::newRow("Fifty moves draw")
		<< variant
		<< "8/k7/2K5/8/8/8/R7/8 b - - 100 1"
		<< "1/2-1/2";
	QTest::newRow("Fifty moves mate")
		<< variant
		<< "8/8/k1K5/8/8/8/R7/8 b - - 100 1"
		<< "1-0";


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

	variant = "coregal";

	QTest::newRow("coregal white win #1")
		<< variant
		<< "4Rqk1/p2p2pp/1pp2p2/8/8/3P4/PP3PPP/4Q1K1 b - - 0 1"
		<< "1-0";
	QTest::newRow("coregal white win #2")
		<< variant
		<< "q3r1k1/R1Q2ppp/8/4n3/4PN2/3P3P/5PP1/6K1 b - - 0 27"
		<< "1-0";

	variant = "grid";

	QTest::newRow("grid white win")
		<< variant
		<< "r6r/2kRN2p/pp6/2p5/5B2/3P1P2/PP3P1P/6K1 b - - 0 1"
		<< "1-0";
	QTest::newRow("grid black win")
		<< variant
		<< "8/8/1p3R2/pkp5/2b1P1r1/5N2/Pb3P1P/3R2K1 w - - 2 28"
		<< "0-1";
	QTest::newRow("grid black win2")
		<< variant
		<< "Q1bk1bnr/8/3p2p1/p3p3/8/4P3/PPPP2PP/RNB1K1qR w KQ - 0 1"
		<< "0-1";
	QTest::newRow("grid white win2")
		<< variant
		<< "r1bqkbnr/p1p1pppp/1pBp4/8/8/4P3/PPPP1PPP/RNBQK1NR b KQkq - 0 4"
		<< "1-0";
	QTest::newRow("grid draw stalemate")
		<< variant
		<< "8/8/8/8/pk6/KP6/2r5/8 w - - 0 1"
		<< "1/2-1/2";

	variant = "gryphon";

	QTest::newRow("gryphon black win")
		<< variant
		<< "r1b4r/ppp2pp1/3n1b2/1RB1nK1n/2k1K3/5B2/PPPP1PPP/RNB5 w - - 0 1"
		<< "0-1";

	variant = "twokings";

	QTest::newRow("twokings black win1")
		<< variant
		<< "8/2k5/4b1Pp/2p3K1/8/2P1p3/2k2q1K/B7 w - - 0 59"
		<< "0-1";
	QTest::newRow("twokings black win2")
		<< variant
		<< "8/8/8/8/2k5/4kK2/5K2/2q5 w - - 0 1"
		<< "0-1";

	variant = "shatranj";

	QTest::newRow("shatranj ongoing bk bare")
		<< variant
		<< "8/4k3/5R2/8/4K3/8/8/8 b - - 0 1"
		<< "*";
	QTest::newRow("shatranj white win bk bare1")
		<< variant
		<< "8/4k3/5R2/8/4K3/8/8/8 w - - 0 1"
		<< "1-0";
	QTest::newRow("shatranj draw both kings bare")
		<< variant
		<< "8/8/5k2/8/4K3/8/8/8 w - - 0 1"
		<< "1/2-1/2";
	QTest::newRow("shatranj white win bk bare2")
		<< variant
		<< "8/3k4/5R2/8/4K3/8/8/8 b - - 0 1"
		<< "1-0";
	QTest::newRow("shatranj white win bk bare3")
		<< variant
		<< "8/4k3/5R2/8/4K3/2P5/8/8 b - - 0 1"
		<< "1-0";
	QTest::newRow("shatranj black win wk bare1")
		<< variant
		<< "8/8/7k/8/2K5/8/2q5/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("shatranj black win wk bare2")
		<< variant
		<< "8/8/7k/8/2K5/2q5/8/4q3 w - - 0 1"
		<< "0-1";
	QTest::newRow("shatranj white mates")
		<< variant
		<< "r3k3/2PB1RN1/1n4P1/4PK2/1p3p2/1P6/Pp3P2/8 b - - 0 1"
		<< "1-0";
	QTest::newRow("shatranj black mates")
		<< variant
		<< "7r/8/4bkqK/8/8/8/7R/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("shatranj white king stalemated")
		<< variant
		<< "8/8/5kqK/8/8/8/8/8 w - - 0 1"
		<< "0-1";
	QTest::newRow("shatranj black king stalemated")
		<< variant
		<< "8/8/8/8/8/8/5KQk/8 b - - 0 1"
		<< "1-0";

	variant = "courier";

	QTest::newRow("courier ongoing bk bare")
		<< variant
		<< "12/4k7/5W6/12/4K7/12/12/12 b - - 0 1"
		<< "*";
	QTest::newRow("courier white win bk bare1")
		<< variant
		<< "12/4k7/5W6/12/4K7/12/12/12 w - - 0 1"
		<< "1-0";
	QTest::newRow("courier white win bk bare2")
		<< variant
		<< "12/4k7/5W6/12/4K7/11E/12/12 b - - 0 1"
		<< "1-0";

	variant = "makruk";

	QTest::newRow("makruk white win /w pawns: no counting")
		<< variant
		<< "7R/6Mk/5S2/7P/Pp6/8/2mnr3/K7 b - 0 0 129"
		<< "1-0";

	QTest::newRow("makruk black win KNMvsK")
		<< variant
		<< "8/8/8/8/8/5nk1/6m1/6K1 w - 128 56 213"
		<< "0-1";

	QTest::newRow("makruk full count black win KRvKM")
		<< variant
		<< "8/8/8/8/8/6k1/8/M1r3K1 w - 128 128 115"
		<< "0-1";

	QTest::newRow("makruk board's honour draw KRvKM")
		<< variant
		<< "2r5/8/8/8/8/1k6/6K1/M7 b - 128 128 125"
		<< "1/2-1/2";

	QTest::newRow("makruk board's honour draw KSMvKS")
		<< variant
		<< "8/8/8/4S3/2k1K3/1s6/7M/8 b - 128 128 236"
		<< "1/2-1/2";

	QTest::newRow("makruk KNvK insufficient material")
		<< variant
		<< "8/8/1k6/3K4/8/3N4/8/8 b - 128 6 105"
		<< "1/2-1/2";

	QTest::newRow("makruk KMMvK insufficient material")
		<< variant
		<< "8/8/6MM/8/8/2K5/k7/8 b - 128 8 99"
		<< "1/2-1/2";

	QTest::newRow("makruk KMMvK win overrides insufficient material")
		<< variant
		<< "8/5KMk/6M1/8/8/8/8/8 b - 128 8 75"
		<< "1-0";

	QTest::newRow("makruk KSMvK pieces' honour white win")
		<< variant
		<< "8/5KMk/7S/8/8/8/8/8 b - 88 52 174"
		<< "1-0";

	QTest::newRow("makruk KRRvK pieces' honour white win")
		<< variant
		<< "8/8/8/2K1k3/8/4RR2/8/8 b - 16 14 128"
		<< "1-0";

	QTest::newRow("makruk KRRvK pieces' honour ongoing")
		<< variant
		<< "8/8/8/2K1k3/8/3R1R2/8/8 w - 16 13 128"
		<< "*";

	variant = "karouk";

	QTest::newRow("karouk KRRvKM black wins by check")
		<< variant
		<< "8/8/8/2K1k3/3m4/3R1R2/8/8 w - 128 13 128"
		<< "0-1";

	variant = "asean";

	QTest::newRow("asean KQvK insufficient material")
		<< variant
		<< "8/8/8/5K2/1q6/8/8/k7 w - - 0 71"
		<< "1/2-1/2";

	variant = "ai-wok";

	QTest::newRow("ai-wok black win")
		<< variant
		<< "8/8/8/2sp2k1/8/3P4/4K1a1/7r w - - 0 1"
		<< "0-1";
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

	variant = "atomic";
	QTest::newRow("atomic startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 3 plies: 8902, 4 plies: 197326, 5 plies: 4865037, 6 plies: 118951457
		<< Q_UINT64_C(4865037);
	QTest::newRow("atomic pos1")
		<< variant
		<< "8/8/8/8/8/8/3k4/rR4K1 w Q - 0 1"
		<< 5 // 3 plies: 1797, 4 plies: 27928, 5 plies: 453449, 6 plies: 7667595
		<< Q_UINT64_C(453449);
	QTest::newRow("atomic pos2")
		<< variant
		<< "r4b1r/2kb1N2/p2Bpnp1/8/2Pp3p/1P1PPP2/P5PP/R3K2R b KQ -"
		<< 2 // 1 ply: 4, 2 plies: 148
		<< Q_UINT64_C(148);

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
		<< 5 // 4 plies: 197281, 5 plies: 4888832, 6 plies: 120812942
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
		<< 5 // 4 plies: 197281, 5 plies: 4889167, 6 plies: 120835923
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
		<< 4 // 4 plies: 882717, 5 plies: 29119802, 6 plies: 947730871
		<< Q_UINT64_C(882717);
	QTest::newRow("berolina1")
		<< variant
		<< "3k1r2/1pp3p1/3P2p1/PR4p1/6p1/2P2Pn1/b3r1BP/3KN3 b - - 1 26"
		<< 4 // 4 plies: 909365, 5 plies: 37601709
		<< Q_UINT64_C(909365);

	variant = "racingkings";
	QTest::newRow("racingkings startpos")
		<< variant
		<< "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1"
		<< 4 // 4 plies: 296242, 5 plies: 9472927, 6 plies: 298712641
		<< Q_UINT64_C(296242);

	variant = "horde";
	QTest::newRow("horde v2 startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1"
		<< 6 // 5 plies: 265223, 6 plies: 5396554
		<< Q_UINT64_C(5396554);
	QTest::newRow("horde dunsany startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP b kq - 0 1"
		<< 5 // 5 plies: 775839, 6 plies: 59231536
		<< Q_UINT64_C(775839);
	QTest::newRow("horde3")
		<< variant
		<< "rnbqkbnr/6p1/2p1Pp1P/P1PPPP2/Pp4PP/1p2PPPP/1P2PPPP/PP1nPPPP b kq a3 0 18"
		<< 5 // 4 plies: 197287, 5 plies: 6429490
		<< Q_UINT64_C(6429490);

	variant = "andernach";
	QTest::newRow("andernach startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 197410, 5 plies: 4870137, 6 plies: 119527044
		<< Q_UINT64_C(4870137);
	QTest::newRow("andernach pos1")
		<< variant
		<< "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"
		<< 4 // 4 plies: 523348, 5 plies: 16330793
		<< Q_UINT64_C(523348);

	variant = "checkless";
	QTest::newRow("checkless startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 196772, 5 plies: 4835050, 6 plies: 118060698
		<< Q_UINT64_C(4835050);

	variant = "embassy";
	QTest::newRow("embassy startpos")
		<< variant
		<< "rnbqkcabnr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNBQKCABNR w KQkq - 0 1"
		<< 4 // 4 plies: 809539, 5 plies: 28937546, 6 plies: 1023746640
		<< Q_UINT64_C(809539);

	variant = "coregal";
	QTest::newRow("coregal startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 195896, 5 plies: 4756867, 6 plies: 114110257
		<< Q_UINT64_C(4756867);

	variant = "extinction";
	QTest::newRow("extinction startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 197742, 5 plies: 4897256, 6 plies: 120921506
		<< Q_UINT64_C(4897256);

	variant = "grid";
	QTest::newRow("grid startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 173165, 5 plies: 3814913, 6 plies: 83288561
		<< Q_UINT64_C(3814913);
	QTest::newRow("grid pos2")
		<< variant
		<< "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
		<< 4
		<< Q_UINT64_C(1853429);
	QTest::newRow("grid pos3")
		<< variant
		<< "8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - -"
		<< 2
		<< Q_UINT64_C(142);
	QTest::newRow("grid pos4")
		<< variant
		<< "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
		<< 6
		<< Q_UINT64_C(1477948);
	QTest::newRow("grid pos5")
		<< variant
		<< "r1bqkbnr/pppppppp/n7/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq -"
		<< 5 // 3 plies: 8768, 4 plies: 177309, 5 plies: 4069318
		<< Q_UINT64_C(4069318);

	variant = "displacedgrid";
	QTest::newRow("displacedgrid startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 31057, 5 plies: 537400, 6 plies: 9222549
		<< Q_UINT64_C(537400);

	variant = "slippedgrid";
	QTest::newRow("slippedgrid startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 176529, 5 plies: 3953321, 6 plies: 87774558
		<< Q_UINT64_C(3953321);

	variant = "gridolina";
	QTest::newRow("gridolina startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 4 // 4 plies: 798011, 5 plies: 24033022, 6 plies: 712513491
		<< Q_UINT64_C(798011);

	variant = "janus";
	QTest::newRow("janus startpos")
		<< variant
		<< "rjnbkqbnjr/pppppppppp/10/10/10/10/PPPPPPPPPP/RJNBKQBNJR w KQkq - 0 1"
		<< 4 // 4 plies: 772074, 5 plies: 26869186, 6 plies: 923380321
		<< Q_UINT64_C(772074);

	variant = "knightmate";
	QTest::newRow("knightmate startpos")
		<< variant
		<< "rmbqkbmr/pppppppp/8/8/8/8/PPPPPPPP/RMBQKBMR w KQkq - 0 1"
		<< 5 // 4 plies: 139774, 5 plies: 3249033, 6 plies: 74568983
		<< Q_UINT64_C(3249033);

	variant = "chancellor";
	QTest::newRow("chancellor startpos")
		<< variant
		<< "rnbqkcnbr/ppppppppp/9/9/9/9/9/PPPPPPPPP/RNBQKCNBR w KQkq - 0 1"
		<< 4 // 4 plies: 436656, 5 plies: 13466196, 6 plies: 412625522
		<< Q_UINT64_C(436656);
	QTest::newRow("chancellor promotion")
		<< variant
		<< "4k4/1P7/K8/9/9/9/9/9/9 w - - 0 1"
		<< 2
		<< Q_UINT64_C(37);

	variant = "modern";
	QTest::newRow("modern startpos")
		<< variant
		<< "rnbqkmbnr/ppppppppp/9/9/9/9/9/PPPPPPPPP/RNBMKQBNR w KQkq - 0 1"
		<< 4 // 4 plies: 433729, 5 plies: 13403293, 6 plies: 411178941
		<< Q_UINT64_C(433729);
	QTest::newRow("modern promotion")
		<< variant
		<< "4k4/1P7/K8/9/9/9/9/9/9 w - - 0 1"
		<< 2
		<< Q_UINT64_C(39);

	variant = "pocketknight";
	QTest::newRow("pocketknight startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[Nn] w KQkq - 0 1"
		<< 4 // 4 plies: 3071267, 5 plies: 99614985, 6 plies: 3228439195 
		<< Q_UINT64_C(3071267);

	variant = "gryphon";
	QTest::newRow("gryphon startpos")
		<< variant
		<< "rnbq1bnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQ1BNR w - - 0 1"
		<< 5 // 4 plies: 280477, 5 plies: 6778804, 6 plies: 162243917
		<< Q_UINT64_C(6778804);
	QTest::newRow("gryphon middlegame")
		<< variant
		<< "r2q2nr/p5pp/1n2rn2/2nB4/5N2/R2R2q1/P2P3P/R2Q3R w - - 0 1"
		<< 4 // 2 plies: 982, 3 plies: 27370, 4 plies: 1014040
		<< Q_UINT64_C(1014040);

	variant = "circulargryphon";
	QTest::newRow("circulargryphon startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"
		<< 5 // 4 plies: 243435, 5 plies: 5766940, 6 plies: 134596721
		<< Q_UINT64_C(5766940);

	variant = "simplifiedgryphon";
	QTest::newRow("simplifiedgryphon startpos")
		<< variant
		<< "4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1"
		<< 5 // 4 plies: 92350, 5 plies: 1993716, 6 plies: 42279298
		<< Q_UINT64_C(1993716);

	variant = "losers";
	QTest::newRow("losers startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 152955, 5 plies: 2723795, 6 plies: 46038682, 7 plies: 757349642 
		<< Q_UINT64_C(2723795);

	variant = "antichess";
	QTest::newRow("antichess startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"
		<< 5 // 4 plies: 153299, 5 plies: 2732672, 6 plies: 46264162, 7 plies: 762010688
		<< Q_UINT64_C(2732672);

	variant = "giveaway";

	QTest::newRow("giveaway startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		<< 5 // 4 plies: 153299, 5 plies: 2732672, 6 plies: 46264162, 7 plies: 762096669
		<< Q_UINT64_C(2732672);

	variant = "suicide";

	QTest::newRow("suicide startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"
		<< 5 // 4 plies: 153299, 5 plies: 2732672, 6 plies: 46264162
		<< Q_UINT64_C(2732672);
	QTest::newRow("suicide endgame1")
		<< variant
		<< "8/2b5/8/3B4/8/8/2P5/8 b - - 0 1"
		<< 5 // 5 plies: 116051, 6 plies: 1218696
		<< Q_UINT64_C(116051);
	QTest::newRow("suicide endgame2")
		<< variant
		<< "8/1kKP4/8/8/8/8/8/6n1 b - - 0 1"
		<< 3 // 3 plies: 5, 4 plies: 0
		<< Q_UINT64_C(5);
	QTest::newRow("suicide endgame3")
		<< variant
		<< "8/k1KP4/8/8/8/8/8/6n1 b - - 0 1"
		<< 7 // 7 plies: 2891980
		<< Q_UINT64_C(2891980);

	variant = "threekings";
	QTest::newRow("threekings startpos")
		<< variant
		<< "knbqkbnk/pppppppp/8/8/8/8/PPPPPPPP/KNBQKBNK w - - 0 1"
		<< 5 // 4 plies: 199514, 5 plies: 4971357, 6 plies: 123493813
		<< Q_UINT64_C(4971357);

	variant = "shatranj";
	QTest::newRow("shatranj startpos")
		<< variant
		<< "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w - - 0 1"
		<< 5 // 4 plies: 68122, 5 plies: 1164248, 6 plies: 19864709
		<< Q_UINT64_C(1164248);

	variant = "courier";
	QTest::newRow("courier startpos")
		<< variant
		<< "rnebmkfwbenr/pppppppppppp/12/12/12/12/PPPPPPPPPPPP/RNEBMKFWBENR w - - 0 1"
		<< 5 // 4 plies: 180571, 5 plies: 4139349, 6 plies: 94789147
		<< Q_UINT64_C(4139349);
	QTest::newRow("courier traditional")
		<< variant
		<< "rnebmk1wbenr/1ppppp1pppp1/6f5/p5p4p/P5P4P/6F5/1PPPPP1PPPP1/RNEBMK1WBENR w - - 0 1"
		<< 4 // 4 plies: 500337, 5 plies: 14144849, 6 plies: 400324148
		<< Q_UINT64_C(500337);

	variant = "makruk";
	QTest::newRow("makruk startpos")
		<< variant
		<< "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - 0 0 1"
		<< 5 // 4 plies: 273026, 5 plies: 6223994, 6 plies: 142078049
		<< Q_UINT64_C(6223994);

	variant = "cambodian";
	QTest::newRow("cambodian startpos")
		<< variant
		<< "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w DEde 0 0 1"
		<< 4 // 4 plies: 361793, 5 plies: 8601434, 6 plies: 204755574
		<< Q_UINT64_C(361793);
	QTest::newRow("cambodian check1")
		<< variant
		<< "r1s1ks1r/3nm3/pppNpppp/3n4/5P2/PPPPPNPP/8/R1SKMS1R b DEe 0 0 5"
		<< 2 // 1 ply: 2, 2 plies: 72 (sjaakii-1.4.1 dito)
		<< Q_UINT64_C(72);

	variant = "asean";
	QTest::newRow("asean startpos")
		<< variant
		<< "rnbqkbnr/8/pppppppp/8/8/PPPPPPPP/8/RNBQKBNR w - - 0 1"
		<< 5 // 4 plies: 273026, 5 plies: 6223994, 6 plies: 142078057
		<< Q_UINT64_C(6223994);

	variant = "sittuyin";
	QTest::newRow("sittuyin startpos")
		<< variant
		<< "8/8/4pppp/pppp4/4PPPP/PPPP4/8/8[KFRRSSNNkfrrssnn] w - 0 0 1"
		<< 3 // 1 ply: 88, 2 plies: 7744, 3 plies: 580096, 4 plies: 43454464
		<< Q_UINT64_C(580096);
	QTest::newRow("sittuyin midgame")
		<< variant
		<< "8/8/6R1/s3r3/P5R1/1KP3p1/1F2kr2/8[-] b - 0 0 72"
		<< 4 // 1 ply: 35, 2 plies: 825, 3 plies: 26791, 4 plies: 657824
		<< Q_UINT64_C(657824);

	variant = "ai-wok";
	QTest::newRow("ai-wok startpos")
		<< variant
		<< "rnsaksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKASNR w 0 1"
		<< 4 // 3 plies: 18102, 4 plies: 485045, 5 plies: 13275068
		<< Q_UINT64_C(485045);
	QTest::newRow("ai-wok endgame")
		<< variant
		<< "8/8/8/2sp2k1/7p/3P4/6K1/7r w - - 0 1"
		<< 5 // 4 plies: 6855, 5 plies: 30055, 6 plies: 631293
		<< Q_UINT64_C(30055);

	variant = "twokings";
	QTest::newRow("twokings startpos")
		<< variant
		<< "rnbqkknr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKKNR w KQkq - 0 1"
		//asymmetrical variant 4 plies: 192312, 5 plies: 4629168, 6 plies: 110762251
		// symmetrical variant 4 plies: 192332, 5 plies: 4629764, 6 plies: 110829475
		<< 5
		<< Q_UINT64_C(4629168);
	QTest::newRow("twokings endgame1")
		<< variant
		<< "8/8/p1k5/1p1r1K1p/1P5P/P1K5/8/8 b - - 0 121"
		<< 4 // 3 plies:3076, 4 plies: 36828
		<< Q_UINT64_C(36828);

	variant = "twokingssymmetric";
	QTest::newRow("twokings symmetrical variant, startpos")
		<< variant
		<< "rnbqkknr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKKNR w KQkq - 0 1"
		//asymmetrical variant 4 plies: 192312, 5 plies: 4629168, 6 plies: 110762251
		// symmetrical variant 4 plies: 192332, 5 plies: 4629764, 6 plies: 110829475
		<< 5
		<< Q_UINT64_C(4629764);

	variant = "grand";
	QTest::newRow("grand startpos")
		<< variant
		<< "r8r/1nbqkcabn1/pppppppppp/10/10/10/10/PPPPPPPPPP/1NBQKCABN1/R8R w - - 0 1"
		<< 3 // 3plies: 259514, 4 plies: 15921643, 5 plies: 959883584
		<< Q_UINT64_C(259514);
	QTest::newRow("grand endgame1")
		<< variant
		<< "10/4k5/6P3/10/10/10/10/10/1p2K5/10 w - - 0 1"
		<< 3 // 1 ply: 15, 2 plies: 165, 3 plies: 2446
		<< Q_UINT64_C(2446);

	variant = "seirawan";
	QTest::newRow("seirawan startpos")
		<< variant
		<< "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[EHeh] w BCDFGbcdfgKQkq - 0 1"
		<< 4 // 4 plies: 782599, 5 plies: 27639803, 6 plies: 967587141 (sjaakii: 967584909)
		<< Q_UINT64_C(782599);

	variant = "losalamos";
	QTest::newRow("losalamos startpos")
		<< variant
		<< "rnqknr/pppppp/6/6/PPPPPP/RNQKNR w - - 0 1"
		<< 5 // 4 plies: 14332, 5 plies: 191846, 6 plies: 2549164
		<< Q_UINT64_C(191846);
	QTest::newRow("losalamos promotion")
		<< variant
		<< "6/2P3/6/1K1k2/6/6 w - - 0 1"
		<< 5 // 4 plies: 3117, 5 plies: 39171, 6 plies: 187431
		<< Q_UINT64_C(39171);

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

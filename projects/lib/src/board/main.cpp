#include "standardboard.h"
#include "capablancaboard.h"
#include "losersboard.h"
#include "atomicboard.h"
#include "crazyhouseboard.h"
#include "shogiboard.h"
#include "frcboard.h"
#include "caparandomboard.h"
#include "result.h"
#include <QtCore/QCoreApplication>
#include <QTranslator>
#include <QTime>

static quint64 perft(Chess::Board* board, int depth, bool divide = true)
{
	quint64 nodeCount = 0;
	QVector<Chess::Move> moves(board->legalMoves());
	if (depth == 1 || moves.size() == 0)
		return moves.size();

	QVector<Chess::Move>::const_iterator it;
	for (it = moves.begin(); it != moves.end(); ++it)
	{
		QString str = board->moveString(*it, Chess::Board::StandardAlgebraic);
		board->makeMove(*it);
		quint64 n = perft(board, depth - 1, false);
		if (divide)
			qDebug("%s: %llu", qPrintable(str), n);
		nodeCount += n;
		board->undoMove();
	}

	return nodeCount;
}

static void mmove(Chess::Board& board, const QString& str)
{
	Chess::Move move = board.moveFromString(str);
	board.makeMove(move);
	qDebug() << &board;
	qDebug() << board.result().toVerboseString();
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	//QTranslator translator;
	//translator.load("omniboard_fi");
	//app.installTranslator(&translator);

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	//Chess::ShogiBoard board;
	//Chess::CrazyhouseBoard board;
	//Chess::AtomicBoard board;
	//Chess::LosersBoard board;
	Chess::StandardBoard board;
	//Chess::CapablancaBoard board;
	//Chess::FrcBoard board;
	//Chess::CaparandomBoard board;

	board.reset();

	//if (!board.setFenString("r1b2k2nr/p1ppq1ppbp/n1Pcpa2p1/5p4/5P4/1p1PBCPN2/PP1QP1BPPP/RN3KA2R w KQkq -"))
	//if (!board.setFenString("1rk3r1/8/8/8/8/8/8/1RK1R3 w EBgb -"))
	//if (!board.setFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"))
	//if (!board.setFenString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"))
	//if (!board.setFenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"))
	//if (!board.setFenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - KQkq - 0 1"))
	//if (!board.setFenString("lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"))
	//if (!board.setFenString("7lk/9/8S/9/9/9/9/7L1/8K b P"))
	//{
	//	qDebug("setBoard failed");
	//	return 1;
	//}
	qDebug() << &board;

	// Three-fold rep
	/*
	mmove(board, "b1a3");
	mmove(board, "b8a6");
	mmove(board, "a3b1");
	mmove(board, "a6b8");
	mmove(board, "b1a3");
	mmove(board, "b8a6");
	mmove(board, "a3b1");
	mmove(board, "a6b8");
	return 0;
	*/

	// Scholar's mate
	// 1.e4 e5 2.Qh5 Nc6 3.Bc4 Nf6 4.Qxf7#
	/*
	mmove(board, "e2e4");
	mmove(board, "e7e5");
	mmove(board, "d1h5");
	mmove(board, "b8c6");
	mmove(board, "f1c4");
	mmove(board, "g8f6");
	mmove(board, "h5f7");
	return 0;
	*/

	/*
	mmove(board, "e2e4");
	mmove(board, "d7d5");
	mmove(board, "e4e5");
	mmove(board, "f7f5");
	mmove(board, "e1e2");
	mmove(board, "e8f7");
	board.undoMove();
	board.undoMove();
	board.undoMove();
	board.undoMove();
	board.undoMove();
	board.undoMove();
	qDebug() << &board;

	mmove(board, "a2a4");
	mmove(board, "b7b5");
	mmove(board, "h2h4");
	mmove(board, "b5b4");
	mmove(board, "c2c4");
	mmove(board, "b4c3");
	mmove(board, "a1a3");
	*/

	/* int source = board.squareIndex(Chess::Square("e1"));
	int target = board.squareIndex(Chess::Square("a1"));
	Chess::Move move(source, target);
	board.makeMove(move);
	qDebug() << &board;
	source = board.squareIndex(Chess::Square("e8"));
	target = board.squareIndex(Chess::Square("a8"));
	move = Chess::Move(source, target);
	board.makeMove(move);
	qDebug() << &board;
	board.undoMove();
	qDebug() << &board;

	return 0; */

	qDebug("Perft: %llu", perft(&board, 5));
	qDebug() << &board;

	return 0;
}

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QtGlobal>
#include <QList>
#include <QLinkedList>
#include "castlingrights.h"

class QChar;
class QString;
class ChessMove;
class CompleteChessMove;
class ChessPiece;
class MoveData;

enum MoveNotation
{
	LongAlgebraic, StandardAlgebraic
};

class Chessboard
{
	public:
		enum ChessSide
		{
			NoSide = -1, White, Black
		};
		
		enum Variant
		{
			StandardChess, FischerRandomChess, CapablancaChess
		};
		
		/**
		* The result of a chess game.
		*/
		enum Result
		{
			/** White wins by mating the black player. */
			WhiteMates,
			/** Black wins by mating the white player. */
			BlackMates,
			/** Draw by a stalemate. */
			Stalemate,
			/** Draw by insufficient mating material on both sides. */
			DrawByMaterial,
			/** Draw by 3 repetitions of the same position. */
			DrawByRepetition,
			/** Draw by 50 consecutive reversible moves. */
			DrawByFiftyMoves,
			/** No result. The game may continue. */
			NoResult
		};
		
		friend class Fen;
		friend class Zobrist;
		Chessboard(int width, int height);
		virtual ~Chessboard();

		QString fenString() const;
		virtual bool setFenString(const QString& fen);

		void print() const;
		virtual Variant variant() const = 0;
		
		void makeMove(const ChessMove& move);
		void makeMove(const CompleteChessMove& move);
		void undoMove();

		void addPiece(ChessPiece* piece);
		void removePiece(ChessPiece* piece);
		bool canSideAttackSquare(ChessSide side, int square);
		bool isSideInCheck(ChessSide side);
		
		CompleteChessMove completeMove(const ChessMove& move);
		void generateMoves(QList<CompleteChessMove>* moves) const;
		Result result();
		
		//QList<ChessMove> legalMoves();

		ChessSide side() const;
		int squareToIndex(int square) const;
		int width() const;
		int height() const;
		int arrayWidth() const;
		ChessPiece* pieceAt(int square) const;
		void setSquare(int square, ChessPiece* piece);
		const CastlingRights* castlingRights(ChessSide side = NoSide) const;
		void setCastlingRights(ChessSide side, const CastlingRights& castlingRights);
		void disableCastlingRights(ChessSide side, int castlingSide);
		int enpassantSquare() const;
		void setEnpassantSquare(int square);

		qint64 perft(int depth, bool divide = true);

		bool isValidSquare(int square) const;
		int squareFromString(const QString& str) const;
		void clear();
		QString moveString(const ChessMove& move, MoveNotation notation);
		ChessMove moveFromString(const QString& str);
		bool isMoveLegal(const ChessMove& move);
		void updateZobristKey(quint64 component);

	protected:
		ChessPiece** m_squares;
		int m_width;
		int m_height;
		int m_boardSize;
		int m_arraySize;
	
	private:
		bool isMoveLegal(const CompleteChessMove& move);
		bool isMoveCheck(const CompleteChessMove& move);
		bool isMoveMate(const CompleteChessMove& move);
		QString stringFromSquare(int square) const;
		QString coordStringFromMove(const CompleteChessMove& move);
		CompleteChessMove moveFromCoordString(const QString& str);
		CompleteChessMove moveFromSanString(const QString& str);
		QString sanStringFromMove(const CompleteChessMove& move);
		int repeatCount(int maxRepeats = 3) const;
		int reversibleMoveCount() const;
		void setReversibleMoveCount(int count);
		quint64 zobristKey() const;
		void setZobristKey(quint64 key);
		
		ChessSide m_side;
		QLinkedList<ChessPiece*> m_pieces[2];
		QList<MoveData> m_history;
		ChessPiece* m_king[2];
		int m_moveCount;
};


#endif // CHESSBOARD_H

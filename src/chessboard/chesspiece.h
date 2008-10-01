#ifndef CHESSPIECE_H
#define CHESSPIECE_H


#include <QChar>
#include "chessboard.h"

class ChessPiece;
class CompleteChessMove;

class ChessPiece
{
	public:
		enum PieceType
		{
			PT_None,
			PT_Pawn,
			PT_Knight,
			PT_Bishop,
			PT_Rook,
			PT_Archbishop,
			PT_Chancellor,
			PT_Queen,
			PT_King
		};
		
		ChessPiece(Chessboard::ChessSide side, int square, Chessboard* board);
		virtual ~ChessPiece() {}
		
		virtual void generateMoves(QList<CompleteChessMove>* moves) const;
		virtual void makeMove(const CompleteChessMove& move);
		virtual void undoMove(const CompleteChessMove& move);
		virtual bool canAttack(int square) const;
	
		static ChessPiece* fromType(Chessboard::ChessSide side, PieceType type, int square, Chessboard* board);
		static ChessPiece* fromChar(const QChar& c, int square, Chessboard* board);
	
		virtual PieceType type() const = 0;
		QString toString() const;
	
		virtual void enable();
		virtual void disable();
		void setSquare(int square);
	
		Chessboard::ChessSide side() const;
		int square() const;
		bool enabled() const;
	
		static QChar charFromPieceType(PieceType type);
		static PieceType pieceTypeFromChar(const QChar& c);
	
	protected:
		Chessboard::ChessSide m_side;
		int m_square;
		bool m_enabled;
		Chessboard* m_board;
		int m_hopOffsets[8];
		int m_hopOffsetCount;
		int m_slideOffsets[8];
		int m_slideOffsetCount;
};

#endif // CHESSPIECE

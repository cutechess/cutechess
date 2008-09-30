#ifndef FEN_H
#define FEN_H

#include <QString>
#include <QVector>
#include "chessboard.h"
#include "castlingrights.h"

class Fen
{
	public:
		Fen(Chessboard* board, const QString& fenString);
		bool isValid() const;
		void apply();
		static QString fenString(const Chessboard* board);

	private:
		Chessboard* m_board;
		bool m_isValid;
		QVector<int> m_squares;
		CastlingRights m_castlingRights[2];
		Chessboard::ChessSide m_side;
		int m_enpassantSquare;
		int m_moveCount;
		int m_reversibleMoveCount;
		int m_boardSize;
		int m_kingSquares[2];
		int m_rookSquares[2][2];
		
		bool getSquares(const QString& fen);
		void getRookSquares();
		bool addCastlingRights(Chessboard::ChessSide side, int castlingSide);
};

#endif // FEN

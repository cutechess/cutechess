#include "standardchessboard.h"
#include "fen.h"
#include "chesspiece.h"


Chessboard::Variant StandardChessboard::variant() const
{
	return m_variant;
}

/*
   0     1   2   3   4   5   6   7   8     9
  10    11  12  13  14  15  16  17  18    19
  20    21  22  23  24  25  26  27  28    29
  30    31  32  33  34  35  36  37  38    39
  40    41  42  43  44  45  46  47  48    49
  50    51  52  53  54  55  56  57  58    59
  60    61  62  63  64  65  66  67  68    69
  70    71  72  73  74  75  76  77  78    79
*/

bool StandardChessboard::setFenString(const QString& fen)
{
	if (!Chessboard::setFenString(fen))
		return false;

	m_variant = FischerRandomChess;

	const int kingSquares[2] = { 75, 5 };
	const int rookSquares[2][2] = { {71, 78}, {1, 8} };
	for (int side = 0; side < 2; side++)
	{
		const CastlingRights* cr = castlingRights((Chessboard::ChessSide)side);
		for (int cside = 0; cside < 2; cside++)
		{
			if (!cr->canCastle(cside))
				continue;
			ChessPiece* king = pieceAt(kingSquares[side]);
			if (king == 0 || king->type() != ChessPiece::PT_King)
				return true;
			
			int rookSquare = cr->rook(cside)->square();
			if (rookSquare != rookSquares[side][cside])
				return true;
		}
	}
	m_variant = StandardChess;
	
	return true;
}

StandardChessboard::StandardChessboard()
: Chessboard(8, 8)
{
	m_squares = m_standardChessSquares;

	bool success = setFenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	Q_ASSERT(success);
}

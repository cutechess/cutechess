#include <QString>
#include "capablancachessboard.h"


Chessboard::Variant CapablancaChessboard::variant() const
{
	return CapablancaChess;
}

CapablancaChessboard::CapablancaChessboard()
: Chessboard(10, 8)
{
	m_squares = m_capablancaChessSquares;

	bool success = setFenString("rnabqkbcnr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNABQKBCNR w KQkq - 0 1");
	Q_ASSERT(success);
}

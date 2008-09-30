#ifndef CASTLING_RIGHTS_H
#define CASTLING_RIGHTS_H

class ChessPiece;

/* The king always ends up at the C file on the left and G file on the right,
   and the rook's place is always next to the king.
   The rook squares just tell where the rook is before the castling, or -1
   if castling isn't possible. */
class CastlingRights
{
	public:
		CastlingRights();
		
		ChessPiece* rook(int side) const;
		void setRook(ChessPiece* rook, int side);
		bool canCastle(int side) const;

		void enable(int side);
		void disable(int side);

		int kingTarget(int side) const;
		void setKingTarget(int square, int side);

		static const int LeftSide = 0;
		static const int RightSide = 1;
	private:
		ChessPiece* m_rook[2];
		int m_kingTarget[2];
		bool m_canCastle[2];
};

#endif // CASTLING_RIGHTS

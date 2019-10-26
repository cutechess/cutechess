/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sittuyinboard.h"

namespace Chess {

SittuyinBoard::SittuyinBoard()
	: MakrukBoard(),
	  m_inSetUp(true)
{
	// Movements as in makruk, the Elephant has Silver General Movement
	setPieceType(Pawn, tr("pawn"), "P");                      //!< Ne: Feudal Lord P
	setPieceType(Knight, tr("knight"), "N", KnightMovement);  //!< Myin: Horse N
	setPieceType(Elephant, tr("elephant"), "S", SilverGeneralMovement, "E"); //! Sin: Elephant E
	setPieceType(Rook, tr("rook"), "R", RookMovement);        //! Yahhta: Chariot R
	setPieceType(General, tr("general"), "F", FerzMovement);  //! Sit-ke: General G
	setPieceType(King, tr("king"), "K");                      //! Min-gyi: King K
}

Board* SittuyinBoard::copy() const
{
	return new SittuyinBoard(*this);
}

QString SittuyinBoard::variant() const
{
	return "sittuyin";
}

QString SittuyinBoard::defaultFenString() const
{
	return "8/8/4pppp/pppp4/4PPPP/PPPP4/8/8[KFRRSSNNkfrrssnn] w - 0 0 1";
	//Federation notation, ref. Maung Maung Lwin
	//return "8/8/4pppp/pppp4/4PPPP/PPPP4/8/8[KGRREENNkgrreenn] w - 0 0 1";
}

bool SittuyinBoard::variantHasDrops() const
{
	return true;
}

QList< Piece > SittuyinBoard::reservePieceTypes() const
{
	QList< Piece > types;
	for (Side side: { Side::White, Side::Black} )
	{
		for (int type = Knight; type <= King; type++)
			types << Piece(side, type);
	}
	return types;
}

MakrukBoard::CountingRules SittuyinBoard::countingRules() const
{
	return BareKing;
}

bool SittuyinBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	Piece whiteKing(Side::White, King);
	Piece blackKing(Side::Black, King);
	return whiteKings + reserveCount(whiteKing) == 1 && blackKings + reserveCount(blackKing) == 1;
}

int SittuyinBoard::promotionRank(int file) const
{
	return std::max(file , height() - 1 - file);
}

void SittuyinBoard::addPromotions(int sourceSquare, int targetSquare, QVarLengthArray< Move >& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, General));
}

void SittuyinBoard::generatePawnMoves(int square,
				     QVarLengthArray<Move>& moves) const
{
	// Generate moves for Pawn (Feudal Lord)
	QVarLengthArray< Move > moves1;
	ShatranjBoard::generateMovesForPiece(moves1, Pawn, square);

	// Add normal moves and demote shatranj promotions
	for (const Move& m: moves1)
		moves.append(Move(m.sourceSquare(), m.targetSquare()));

	// No promotions if General still on board
	Side side = sideToMove();
	if (pieceCount(side, General) > 0)
		return;

	Square sq = chessSquare(square);
	int file = sq.file();
	int rank = sq.rank();
	int rrank = (side == Side::White) ? rank : height() - 1 - rank;

	// Must promote from promotion rank (except the last pawn)
	if (rrank != promotionRank(file)
	&&  pieceCount(sideToMove(), Pawn) != 1)
		return;

	// Generate promotion to General when on promotion square
	addPromotions(square, square, moves);

	// Generate General's moves, must not capture by promotion
	QVarLengthArray< Move > moves2;
	generateMovesForPiece(moves2, General, square);
	for (const Move& move: moves2)
	{
		if (captureType(move) == Piece::NoPiece)
			addPromotions(square, move.targetSquare(), moves);
	}
}

void SittuyinBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	// Normal game moves
	if (!m_inSetUp)
	{
		// No drops after set up
		if (square == 0)
			return;
		// Pawn moves, special promotions, see above
		if (pieceType == Pawn)
		{
			generatePawnMoves(square, moves);
			return;
		}
		MakrukBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	// Set-up: generate drops onto own half
	if (square != 0)
		return;

	Side side = sideToMove();
	const int size = arraySize();
	const int len = (size - (width() + 2)) / 2;
	const bool isBlack = (side == Side::Black);
	const int start = 2 * (width() + 2) + 1;

	// Loop index i will have Black side perspective
	for (int i = start; i < len; i++)
	{
		int index = isBlack ? i : size - 1 - i;
		Piece tmp = pieceAt(index);
		if (!tmp.isEmpty())
			continue;

		// Rooks must be placed on the base rank
		if (pieceType == Rook
		&&  chessSquare(i).rank() != height() - 1)
			continue;

		moves.append(Move(0, index, pieceType));
	}
}

bool SittuyinBoard::inSetup() const
{
	// Set-up phase (sit-tee) ends when no pieces are left in reserve
	const QList<Piece> reserveTypes = reservePieceTypes();
	for (const Piece piece: reserveTypes)
	{
		if (reserveCount(piece) > 0)
			return true;
	}
	return false;
}

bool SittuyinBoard::vSetFenString(const QStringList& fen)
{
	int ret = MakrukBoard::vSetFenString(fen);
	m_inSetUp = inSetup();
	return ret;
}

Move SittuyinBoard::moveFromSanString(const QString& str)
{
	Move move = MakrukBoard::moveFromSanString(str);

	// Avoid problems with promotion moves in LAN(!) format
	if (move.sourceSquare() != 0
	&&  move.promotion() != Piece::NoPiece && !str.contains("=") )
		return Move();

	return move;
}

void SittuyinBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	MakrukBoard::vMakeMove(move, transition);

	if (move.sourceSquare() == 0)
		removeFromReserve(Piece(sideToMove(), move.promotion()));

	m_inSetUp = inSetup();
}

void SittuyinBoard::vUndoMove(const Move& move)
{
	MakrukBoard::vUndoMove(move);

	if (move.sourceSquare() == 0)
		addToReserve(Piece(sideToMove(), move.promotion()));

	m_inSetUp = inSetup();
}

bool SittuyinBoard::vIsLegalMove(const Move& move)
{
	int promotion = move.promotion();
	Side side = sideToMove();
	Side opp = side.opposite();
	int oppKingSquare = kingSquare(opp);

	// Pawn promotion: must not give check
	if (!m_inSetUp && promotion == General)
	{
		QVarLengthArray<Move> moves;
		generateMovesForPiece(moves, General, move.targetSquare());
		for (const Move& m: moves)
		{
			if (m.targetSquare() == oppKingSquare)
				return false;
		}
	}
	return MakrukBoard::vIsLegalMove(move);
}

bool SittuyinBoard::isLegalPosition()
{
	Side side = sideToMove();
	Side opp = side.opposite();

	if (kingSquare(opp) != 0 && inCheck(opp))
		return false;

	if (m_inSetUp)
		return true;

	// A side may have one General at most
	if (pieceCount(opp, General) > 1
	||  pieceCount(side, General) > 1)
		return false;

	// Do not allow (discovered) checks by a promotion move
	if (plyCount() > 0 && lastMove().promotion() != 0 && inCheck(side))
		return false;

	return true;
}

int SittuyinBoard::countingLimit() const
{
	// analoguous to ASEAN-Chess Article 5.2e
	Side side = sideToMove();
	int rooks = pieceCount(side, Rook);
	if (rooks > 0)
		return 16;

	int elephants = pieceCount(side, Elephant);
	int generals = pieceCount(side,Queen);
	if (elephants > 0 && generals > 0)
		return 44;

	int knights = pieceCount(side, Knight);
	if (knights > 0 && generals > 0)
		return 64;

	return 1000; // intentional limit
}


/*! Returns true if the side to move can make any non-promotion moves. */
bool SittuyinBoard::canMakeNormalMove()
{
	QVarLengthArray<Move> moves;
	generateMoves(moves);

	for (int i = 0; i < moves.size(); i++)
	{
		if (vIsLegalMove(moves[i]) && moves[i].promotion() == Piece::NoPiece)
			return true;
	}

	return false;
}

Result SittuyinBoard::result()
{
	QString str;
	Side side = sideToMove();
	Side opp = side.opposite();

	// Checkmate/Stalemate
	if (!canMove())
	{
		if (inCheck(side))
		{
			str = tr("%1 mates").arg(opp.toString());
			return Result(Result::Win, opp, str);
		}
		else
		{
			// Federation rule 5.2a: stalemate is draw
			str = tr("Draw by stalemate");
			return Result(Result::Draw, Side::NoSide, str);
		}
	}
	if (!m_inSetUp && pieceCount(side, Pawn) == 1 && !canMakeNormalMove())
	{
		// Federation rule 3.9c7: promotion cannot be forced
		str = tr("Draw by promotion stalemate");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 50 move rule
	if(reversibleMoveCount() >= 100)
	{
		str = tr("Draw by fifty moves rule");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// Insufficent material: KvK, KGvK, KNvK
	if (insufficientMaterial())
	{
		str = tr("Draw by insufficient mating material");
		return Result(Result::Draw, Side::NoSide, str);
	}
	// apply counting rules
	return resultFromCounting();
}

} // namespace Chess

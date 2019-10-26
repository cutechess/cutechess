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

#include "makrukboard.h"

namespace Chess {

MakrukBoard::MakrukBoard()
	: ShatranjBoard(),
	  m_promotionRank(5),
	  m_rules(Makruk),
	  m_useWesternCounting(false),
	  m_history()
{
	// King, Ferz, Rook, Knight and Pawn as in chaturanga and shatranj
	setPieceType(Bia, tr("bia"), "P");                    //! Cowry Shell, Chip
	setPieceType(Ma, tr("ma"), "N", KnightMovement);      //! Horse
	//! Khon (Base) replaces Alfil (Bishop): moves as Shogi's Silver General
	setPieceType(Khon, tr("khon"), "S", SilverGeneralMovement, "E");
	setPieceType(Rua, tr("rua"), "R", RookMovement);      //! Boat
	setPieceType(Met, tr("met"), "M", FerzMovement, "F"); //! Grain: Advisor
	setPieceType(Khun, tr("khun"), "K");                  //! Leader, Lord
}

Board* MakrukBoard::copy() const
{
	return new MakrukBoard(*this);
}

QString MakrukBoard::variant() const
{
	return "makruk";
}

QString MakrukBoard::defaultFenString() const
{
	return "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - 0 0 1";
}

int MakrukBoard::promotionRank(int) const
{
	return 5; // counting from zero
}

MakrukBoard::CountingRules MakrukBoard::countingRules() const
{
	return Makruk;
}

inline void rotateAndStoreOffsets(QVarLengthArray<int> a[2])
{
	a[Side::Black].resize(a[Side::White].size());
	for (int i = 0; i < a[Side::White].count(); i++)
		a[Side::Black][i] = -a[Side::White][i];
}

void MakrukBoard::initHistory()
{
	MoveData md {false, 0, 0, 0, {}};
	m_history.clear();
	m_history.append(md);
}

void MakrukBoard::vInitialize()
{
	ShatranjBoard::vInitialize();

	m_promotionRank = promotionRank();
	m_rules = countingRules();
	initHistory();

	/*
	 * The Silver General does not have dyad (C_2, 180 degrees) rotation
	 * symmetric movement, so offsets are different for White and Black
	 */
	int arwidth = width() + 2;
	m_silverGeneralOffsets[Side::White].resize(5);
	m_silverGeneralOffsets[Side::White][0] = -arwidth - 1;
	m_silverGeneralOffsets[Side::White][1] = -arwidth + 1;
	m_silverGeneralOffsets[Side::White][2] = arwidth - 1;
	m_silverGeneralOffsets[Side::White][3] = arwidth + 1;
	m_silverGeneralOffsets[Side::White][4] = -arwidth;

	rotateAndStoreOffsets(m_silverGeneralOffsets);
}

void MakrukBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	if (pieceHasMovement(pieceType, SilverGeneralMovement))
		generateHoppingMoves(square, m_silverGeneralOffsets[sideToMove()], moves);

	if (pieceType != Bia)
	{
		ShatranjBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	generatePawnMoves(square, moves);
}

void MakrukBoard::generatePawnMoves(int square,
				    QVarLengthArray< Move >& moves) const
{
	// Generate moves for pawn (bia)
	QVarLengthArray< Move > moves1;
	ShatranjBoard::generateMovesForPiece(moves1, Bia, square);

	Side side = sideToMove();
	int arwidth = width() + 2;

	// Add moves, promote pawn (bia) to ferz (met) when reaching the
	// promotion rank
	for (const Move& m: moves1)
	{
		int rank = height() + 1 - m.targetSquare() / arwidth;
		int rrank = (side == Side::White) ? rank : height() - 1 - rank;

		if (rrank < m_promotionRank)
			moves.append(m);
		else if (m.promotion() != 0)
			moves.append(m);
		else
			addPromotions(square, m.targetSquare(), moves);
	}
}

bool MakrukBoard::inCheck(Side side, int square) const
{
	Piece piece;
	Side opSide = side.opposite();
	if (square == 0)
		square = kingSquare(side);

	// Silver General Attacks attacks (by Khon)
	for (int i = 0; i < m_silverGeneralOffsets[side].size(); i++)
	{
		piece = pieceAt(square + m_silverGeneralOffsets[side][i]);
		if (piece.side() == opSide
		&&  pieceHasMovement(piece.type(), SilverGeneralMovement))
			return true;
	}
	return ShatranjBoard::inCheck(side, square);
}

void MakrukBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	int capture = captureType(move);
	Piece piece = pieceAt(move.sourceSquare());
	int type = piece.type();
	if (move.sourceSquare() == move.targetSquare())
		capture = Piece::NoPiece;

	ShatranjBoard::vMakeMove(move, transition);

	m_history.append(m_history.last());
	MoveData& md = m_history.last();

	md.totalPlies++;
	if (md.countingLimit > 0)
		md.plyCount++;

	Side side = sideToMove();
	Side opp = side.opposite();

	// update piece counters
	if (capture != Piece::NoPiece)
	{
		md.pieceCount[capture][opp]--;
		md.pieceCount[Piece::NoPiece][opp]--;
	}

	int promotion = move.promotion();
	if (promotion != Piece::NoPiece)
	{
		if (move.sourceSquare() == 0)
			md.pieceCount[Piece::NoPiece][side]++; //drop
		else
			md.pieceCount[type][side]--;
		md.pieceCount[promotion][side]++;
	}

	// Makruk: Allow counting only if there are no Pawns (Chip, Bia)
	bool noPawns = (0 == pieceCount(Side::NoSide, Bia));

	// Bare King: Start Pieces' Honour Counting
	if (!md.piecesHonour
	&&  (pieceCount(opp) < 2 || pieceCount(side) < 2)
	&&  (m_rules == BareKing || noPawns))
	{
		md.piecesHonour = true;
		md.plyCount = 2 * pieceCount();
		md.countingLimit = noPawns ? 2 * countingLimit() : 2 * 64;
	}

	// No Pawns: Start Board's Honour Counting for Makruk
	if (!md.piecesHonour
	&&  noPawns && m_rules == Makruk && md.countingLimit <= 0)
	{
		md.plyCount = 0;
		md.countingLimit = 2 * 64;
	}
}

void MakrukBoard::vUndoMove(const Move& move)
{
	ShatranjBoard::vUndoMove(move);
	m_history.pop_back();
}

/*!
 * When using Makruk counting rules (no Western counting)
 *
 * - This method sets the castling rights field to "-" for Makruk.
 * - The ep field is replaced by the counting limit (in plies).
 * - Makruk ply counting replaces the reversible move count.
 * - This method uses an own total ply counter to keep track of the
 *   full move count.
 *
 *   Examples:
 *   Default FEN: rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - 0 0 1
 *   KSSMvK black mates: 8/8/2m5/s7/8/1k6/1s6/1K6 w - 44 38 182
 */
QString MakrukBoard::vFenString(Board::FenNotation notation) const
{
	if (m_useWesternCounting)
		return WesternBoard::vFenString(notation);

	const MoveData md = m_history.last();
	QString fen = "- " + QString::number(md.countingLimit);
	fen.append(' ').append(QString::number(md.plyCount));
	fen.append(' ').append(QString::number(1 + md.totalPlies / 2));
	return fen;
}

/*!
 * This method reads FEN with full Makruk/Ouk counting support (A),
 * standard FEN (B), and short FEN-like notation without castling
 * rights and en passant fields (C).
 * The latter two use Western counting and ignore Makruk counting.
 *
 * A: rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - 0 0 1
 *
 * B: rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w - - 0 1
 * C: rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w 0 1
 *
 * FEN with Ouk initial move rights instead of castling rights are
 * supported by OukBoard. Format C does not carry this information.
 * Also see \a OukBoard.
 *
 */
bool MakrukBoard::vSetFenString(const QStringList& inputFen)
{
	m_useWesternCounting = false;

	if (!WesternBoard::vSetFenString(inputFen))
		return false;

	// Remove empty parts from QStringList
	const QStringList fen = inputFen.filter(QRegExp("^\\S+"));
	int fensize = fen.size();

	// Does not match any expected format
	if (fensize <= 1 || fensize == 3)
		return false;

	// Count all pieces
	initHistory();
	setAllPieceCounters();

	// Short FEN format or normal ep field "-" turn off Makruk counting
	bool ok = true;
	int tmp = (fen.count() > 1) ? fen.at(1).toInt(&ok) : 0;
	if (!ok || fensize == 2)
	{
		m_useWesternCounting = true;
		return true;
	}

	// Makruk counting limit (in plies) instead of en passant field
	MoveData &md = m_history.last();
	md.countingLimit = tmp;

	// Makruk counting plies instead of irreversible half move count
	tmp = (fen.count() > 2) ? fen.at(2).toInt(&ok) : 0;
	if (!ok)
		return false;
	md.plyCount = tmp;

	// This implementation uses a total ply counter of its own
	tmp = (fen.count() > 3) ? fen.at(3).toInt(&ok) : 1;
	if (!ok)
		return false;

	md.totalPlies = 2 * (tmp - 1);
	if (sideToMove() == Side::Black)
		md.totalPlies++;

	// Reconstruction of md.piecesHonour, see vMakeMove
	bool noPawns = (0 == pieceCount(Side::NoSide, Bia));
	if ((pieceCount(Side::White) < 2 || pieceCount(Side::Black) < 2)
	&&  (m_rules == BareKing || noPawns))
	{
		md.piecesHonour = true;
	}

	return true;
}

void MakrukBoard::setAllPieceCounters()
{
	MoveData& md = m_history.last();
	int maxindex = arraySize();
	for (int i = 0;  i < maxindex; ++i)
	{
		Piece piece = pieceAt(i);
		if (!piece.isValid())
			continue;

		Side side = piece.side();
		md.pieceCount[piece.type()][side]++;
		md.pieceCount[Piece::NoPiece][side]++;
	}
}

int MakrukBoard::material() const
{
	// Insufficient mating material?
	int material = 0;
	bool mets[] = { false, false };
	for (int i = 0; i < arraySize(); i++)
	{
		const Piece& piece = pieceAt(i);
		if (!piece.isValid())
			continue;

		switch (piece.type())
		{
		case Met:  // Advisor (Ferz), Queen
		{
			material+= 1;
			auto color = chessSquare(i).color();
			if (color != Square::NoColor && !mets[color])
			{
				material += 2;
				mets[color] = true;
			}
			break;
		}
		case Ma:  // Horse, Knight
			material += 4;
			break;
		default:
			material += 9;
			break;
		}
	}
	return material;
}

bool MakrukBoard::insufficientMaterial() const
{
	return material() < 25;
}

int MakrukBoard::pieceCount(Side side, int pieceType) const
{
	const MoveData & md = m_history.last();
	if (side == Side::NoSide)
		return md.pieceCount[pieceType][Side::White]
		     + md.pieceCount[pieceType][Side::Black];
	else
		return md.pieceCount[pieceType][side];
}

int MakrukBoard::countingLimit() const
{
	Side side = sideToMove();
	int rooks = pieceCount(side, Rua);
	if (rooks > 1)
		return 8;
	if (rooks == 1)
		return 16;
	int khons = pieceCount(side, Khon);
	if (khons > 1)
		return 22;
	if (pieceCount(side, Ma) > 1)
		return 32;
	if (khons == 1)
		return 44;

	return 64;
}

Result MakrukBoard::resultFromCounting() const
{
	const MoveData md = m_history.last();

	if (md.countingLimit > 0
	&&  md.plyCount >= md.countingLimit)
	{
		QString str = md.piecesHonour ? tr("Draw by counting rules.")
					      : tr("Draw by sixty-four move rule.");
		return Result(Result::Draw, Side::NoSide, str );
	}
	return Result();
}

Result MakrukBoard::result()
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
			str = tr("Draw by stalemate");
			return Result(Result::Draw, Side::NoSide, str);
		}
	}

	// Insufficent material: KvK, KMvK. KMMvK, KNvK
	if (insufficientMaterial())
	{
		str = tr("Draw by insufficient mating material");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// Main path: result from Makruk counting
	if (!m_useWesternCounting)
		return resultFromCounting();

	// Fallback: Western counting - 50 move rule
	if (reversibleMoveCount() >= 100)
	{
		str = tr("Draw by fifty move rule");
		return Result(Result::Draw, Side::NoSide, str);
	}

	return Result();
}

} // namespace Chess

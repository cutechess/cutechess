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

#include "shogiboard.h"
#include <QRegularExpression>
#include "zobrist.h"
#include "boardtransition.h"

namespace Chess {

ShogiBoard::ShogiBoard()
	: Board(new Zobrist()),
	m_kingSquare{0,0},
	m_promotionRank(0),
	m_minIndex(0),
	m_maxIndex(0),
	m_plyOffset(0),
	m_multiDigitNotation(false),
	m_hasImpassePointRule(false),
	m_checks{0,0},
	m_history()
{
	setPieceType(Pawn, tr("pawn"), "P");
	setPieceType(Lance, tr("lance"), "L", LanceMovement, "M");
	setPieceType(Knight, tr("knight"), "N", KnightMovement);
	setPieceType(SilverGeneral, tr("silver"), "S", SilverMovement, "E");
	setPieceType(GoldGeneral, tr("gold"), "G", GoldMovement, "W");
	setPieceType(Bishop, tr("bishop"), "B", BishopMovement);
	setPieceType(Rook, tr("rook"), "R", RookMovement);
	setPieceType(King, tr("king"), "K");

	setPieceType(PromotedPawn, tr("tokin"), "+P", GoldMovement, "W");
	setPieceType(PromotedLance, tr("promoted lance"), "+L", GoldMovement, "W");
	setPieceType(PromotedKnight, tr("promoted knight"), "+N", GoldMovement, "N~");
	setPieceType(PromotedSilver, tr("promoted silver"), "+S", GoldMovement, "W");
	setPieceType(PromotedBishop, tr("horse"), "+B", BishopMovement | WazirMovement, "B~");
	setPieceType(PromotedRook, tr("dragon"), "+R", RookMovement | FerzMovement, "R~");
}

Board* ShogiBoard::copy() const
{
	return new ShogiBoard(*this);
}

QList<Piece> ShogiBoard::reservePieceTypes() const
{
	QList<Piece> list;

	for (int i = 0; i < 2; i++)
	{
		for (int type = Pawn; type < King; type++)
			list << Piece(Side::Type(i), type);
	}

	return list;
}

QString ShogiBoard::variant() const
{
	return "shogi";
}

bool ShogiBoard::variantHasDrops() const
{
	return true;
}

bool ShogiBoard::variantHasOptionalPromotions() const
{
	return true;
}

QString ShogiBoard::defaultFenString() const
{
	return "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL[-] w - 1";
}

int ShogiBoard::width() const
{
	return 9;
}

Board::CoordinateSystem ShogiBoard::coordinateSystem() const
{
	return Chess::Board::NormalCoordinates;
}

int ShogiBoard::height() const
{
	return 9;
}

int ShogiBoard::reserveType(int pieceType) const
{
	return normalPieceType(pieceType);
}

int ShogiBoard::normalPieceType(int type) const
{
	switch (type)
	{
	case PromotedPawn:
		return Pawn;
	case PromotedLance:
		return Lance;
	case PromotedKnight:
		return Knight;
	case PromotedSilver:
		return SilverGeneral;
	case PromotedBishop:
		return Bishop;
	case PromotedRook:
		return Rook;

	default:
		return type;
	}
}

int ShogiBoard::promotedPieceType(int type) const
{
	switch (type)
	{
	case Pawn:
		return PromotedPawn;
	case Lance:
		return PromotedLance;
	case Knight:
		return PromotedKnight;
	case SilverGeneral:
		return PromotedSilver;
	case Bishop:
		return PromotedBishop;
	case Rook:
		return PromotedRook;

	default:
		return type;
	}
}

int ShogiBoard::promotionRank() const
{
	// Promotion zone is opponent's third.
	return height() - height() / 3;
}

bool ShogiBoard::hasImpassePointRule() const
{
	return true;
}

inline void rotateAndStoreOffsets(QVarLengthArray<int> a[2])
{
	a[Side::Black].resize(a[Side::White].size());
	for (int i = 0; i < a[Side::White].count(); i++)
		a[Side::Black][i] = -a[Side::White][i];
}

void ShogiBoard::vInitialize()
{
	// White as first player: Sente 先手.
	m_kingSquare[Side::White] = 0;
	// Black as second player: Gote 後手.
	m_kingSquare[Side::Black] = 0;

	m_promotionRank = promotionRank();

	// First index on board ("a9", "9a", "91", "9一", "九一")
	m_minIndex = 2 * (width() + 2) + 1;

	// Upper limit of board indices (plus one)
	m_maxIndex = arraySize() - m_minIndex;

	/*
	 * The Lance, Knight, Silver General and Gold General do not
	 * have dyad (C_2, 180 degrees) rotation symmetric movement,
	 * so offsets are different for White and Black.
	 */
	int arwidth = width() + 2;

	m_bishopOffsets.resize(4);
	m_bishopOffsets[0] = -arwidth - 1;
	m_bishopOffsets[1] = -arwidth + 1;
	m_bishopOffsets[2] = arwidth - 1;
	m_bishopOffsets[3] = arwidth + 1;

	m_rookOffsets.resize(4);
	m_rookOffsets[0] = -arwidth;
	m_rookOffsets[1] = -1;
	m_rookOffsets[2] = 1;
	m_rookOffsets[3] = arwidth;

	m_lanceOffsets[Side::White].resize(1);
	m_lanceOffsets[Side::White][0] = -arwidth;
	m_lanceOffsets[Side::Black].resize(1);
	m_lanceOffsets[Side::Black][0] = arwidth;

	m_knightOffsets[Side::White].resize(2);
	m_knightOffsets[Side::White][0] = -2 * arwidth - 1;
	m_knightOffsets[Side::White][1] = -2 * arwidth + 1;

	rotateAndStoreOffsets(m_knightOffsets);

	m_silverGeneralOffsets[Side::White].resize(5);
	m_silverGeneralOffsets[Side::White][0] = -arwidth - 1;
	m_silverGeneralOffsets[Side::White][1] = -arwidth + 1;
	m_silverGeneralOffsets[Side::White][2] = arwidth - 1;
	m_silverGeneralOffsets[Side::White][3] = arwidth + 1;
	m_silverGeneralOffsets[Side::White][4] = -arwidth;

	rotateAndStoreOffsets(m_silverGeneralOffsets);

	m_goldGeneralOffsets[Side::White].resize(6);
	m_goldGeneralOffsets[Side::White][0] = -arwidth - 1;
	m_goldGeneralOffsets[Side::White][1] = -arwidth + 1;
	m_goldGeneralOffsets[Side::White][2] = -arwidth;
	m_goldGeneralOffsets[Side::White][3] = 1;
	m_goldGeneralOffsets[Side::White][4] = -1;
	m_goldGeneralOffsets[Side::White][5] = arwidth;

	rotateAndStoreOffsets(m_goldGeneralOffsets);

	m_multiDigitNotation = (qMax(width(),height()) > 9);
	m_hasImpassePointRule = hasImpassePointRule();
}

QString ShogiBoard::vFenString(Board::FenNotation) const
{
	QString fen;

	//
	fen += '-';

	// Full move number
	fen += ' ';
	fen += QString::number((m_history.size() + m_plyOffset) / 2 + 1);

	return fen;
}

bool ShogiBoard::vSetFenString(const QStringList& fen)
{
	if (fen.size() < 2)
		return false;

	m_history.clear();
	m_kingSquare[Side::White] = 0;
	m_kingSquare[Side::Black] = 0;

	QStringList::const_iterator token = fen.begin();

	// Ignore field
	if (token != fen.end())
	{
		++token;
	}

	// Read the full move number and calculate m_plyOffset
	if (token != fen.end())
	{
		bool ok;
		int tmp = token->toInt(&ok);
		if (!ok || tmp < 1)
			return false;
		m_plyOffset = 2 * (tmp - 1);
	}
	else
		m_plyOffset = 0;

	if (startingSide() == Side::Black)
		m_plyOffset++;

	int kingCount[2] = {0, 0};
	for (int sq = m_minIndex; sq < m_maxIndex; ++sq)
	{
		const Piece piece = pieceAt(sq);
		if (piece.type() == King)
		{
			m_kingSquare[piece.side()] = sq;
			kingCount[piece.side()]++;
		}
	}
	if (kingCount[Side::White] != 1
	||  kingCount[Side::Black] != 1)
		return false;

	return true;
}

QString ShogiBoard::lanMoveString(const Move& move)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();
	int prom = move.promotion();

	QString str;
	// Piece drop
	if (source == 0)
	{
		Q_ASSERT(prom != Piece::NoPiece);
		QChar drop = true ? '@' : '*';
		str += pieceSymbol(prom).toUpper() + drop;
		str += squareString(target);
		return str;
	}

	str = squareString(source);
	str += squareString(target);
	if (prom != Piece::NoPiece)
		str += "+";
	return str;
}

QString ShogiBoard::sanMoveString(const Move& move)
{
	QString str;
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece = pieceAt(source);
	Piece capture = pieceAt(target);
	Square square = chessSquare(source);

	if (source == target)
		capture = Piece::NoPiece;

	// Drop move
	if (source == 0 && move.promotion() != Piece::NoPiece)
	{
		str = lanMoveString(move);
		return str;
	}

	bool needRank = false;
	bool needFile = false;

	str += pieceSymbol(piece).toUpper();
	QVarLengthArray<Move> moves;
	generateMoves(moves, piece.type());

	for (int i = 0; i < moves.size(); i++)
	{
		const Move& move2 = moves.at(i);
		if (move2.sourceSquare() == 0
		||  move2.sourceSquare() == source
		||  move2.targetSquare() != target)
			continue;

		if (!vIsLegalMove(move2))
			continue;

		Square square2(chessSquare(move2.sourceSquare()));
		if (square2.file() != square.file())
			needFile = true;
		else if (square2.rank() != square.rank())
			needRank = true;
	}

	if (needFile)
		str += QChar('a' + square.file());
	if (needRank)
		str += QString::number(1 + square.rank());

	if (capture.isValid())
		str += 'x';

	str += squareString(target);

	if (move.promotion() != Piece::NoPiece)
		str += "+";

	return str;
}

Move ShogiBoard::moveFromLanString(const QString& istr)
{
	QString str(istr);
	// Use uniform drop move indicator "@"
	str.replace(QRegularExpression("[*']"),"@");

	Move move = Chess::Board::moveFromLanString(str);
	bool isPromotion = (str.endsWith("+"));
	if (!isPromotion)
		return move;

	int source = move.sourceSquare();
	Piece piece = pieceAt(source);
	int type = piece.type();
	int promotion =  promotedPieceType(type);
	return Move(move.sourceSquare(), move.targetSquare(), promotion);
}

Move ShogiBoard::moveFromSanString(const QString& str)
{
	if (str.length() < 2)
		return Move();

	QString mstr(str);
	Side side = sideToMove();

	// Ignore mate/skip promotion/strong move/blunder notation
	while (mstr.endsWith('#') || mstr.endsWith('=')
	||     mstr.endsWith('!') || mstr.endsWith('?'))
	{
		mstr.chop(1);
	}
	// Remove promotion notation from the end of the string
	bool promotionMove = mstr.endsWith('+');
	if (promotionMove)
		mstr.chop(1);

	if (mstr.length() < 2)
		return Move();

	// Number of digits in notation of squares
	int digits = 1;

	// Find maximum number of sequential digits
	int count = 0;
	if (m_multiDigitNotation)
	{
		foreach (const QChar& ch, mstr)
		{
			if (ch.isDigit())
			{
				count++;
				if (count > digits)
					digits = count;
			}
			else
				count = 0;
		}
	}
	Square sourceSq;
	Square targetSq;
	QString::const_iterator it = mstr.cbegin();

	// A pawn move need not specify the piece type
	if (pieceFromSymbol(*it) == Pawn)
		it++; // ignore character
	// Piece type
	Piece piece = (*it != '+') ? pieceFromSymbol(*it)
				   : pieceFromSymbol('+' + *++it);
	if (piece.side() != Side::White)
		piece = Piece::NoPiece;
	else
		piece.setSide(side);

	if (piece.isEmpty())
	{
		piece = Piece(side, Pawn);
		targetSq = chessSquare(mstr.mid(0, 1 + digits));
		if (isValidSquare(targetSq))
			it += 1 + digits;
	}
	else
	{
		++it;

		// Drop moves
		if (*it == '@' || *it == '*' || *it =='\'')
		{
			targetSq = chessSquare(mstr.right(1 + digits));
			if (!isValidSquare(targetSq))
				return Move();

			Move move(0, squareIndex(targetSq), piece.type());
			if (isLegalMove(move))
				return move;
			return Move();
		}
	}

	int promotion = promotionMove ? promotedPieceType(piece.type())
				      : Piece::NoPiece;

	if (!isValidSquare(targetSq))
	{
		// Source square's file
		sourceSq.setFile(it->toLatin1() - 'a');
		if (sourceSq.file() < 0 || sourceSq.file() >= width())
			sourceSq.setFile(-1);
		else if (++it == mstr.cend())
			return Move();

		// Source square's rank
		if (it->isDigit())
		{
			const QString tmp(mstr.mid(it - mstr.constBegin(),
						   digits));
			sourceSq.setRank(-1 + tmp.toInt());
			if (sourceSq.rank() < 0 || sourceSq.rank() >= height())
				return Move();
			it += digits;
		}

		if (it == mstr.cend())
		{
			// What we thought was the source square, was
			// actually the target square.
			if (isValidSquare(sourceSq))
			{
				targetSq = sourceSq;
				sourceSq.setRank(-1);
				sourceSq.setFile(-1);
			}
			else
				return Move();
		}
		// Capture
		else if (*it == 'x')
		{
			if(++it == mstr.cend())
				return Move();
		}

		// Target square
		if (!isValidSquare(targetSq))
		{
			if (it + 1 >= mstr.cend())
				return Move();
			QString tmp(mstr.mid(it - mstr.cbegin(), 1 + digits));
			targetSq = chessSquare(tmp);
			it += tmp.size();
		}
	}
	if (!isValidSquare(targetSq))
		return Move();
	int target = squareIndex(targetSq);

	// Promotion
	if (it != mstr.cend())
	{
		if ((*it == '=' || *it == '(') && ++it == mstr.cend())
			return Move();

		promotion = pieceFromSymbol(*it).type();
		if (promotion == Piece::NoPiece)
			return Move();
	}

	QVarLengthArray<Move> moves;
	generateMoves(moves, piece.type());
	const Move* match = nullptr;

	// Loop through all legal moves to find a move that matches
	// the data we got from the move string.
	for (int i = 0; i < moves.size(); i++)
	{
		const Move& move = moves[i];
		if (move.sourceSquare() == 0 || move.targetSquare() != target)
			continue;
		Square sourceSq2 = chessSquare(move.sourceSquare());
		if (sourceSq.rank() != -1 && sourceSq2.rank() != sourceSq.rank())
			continue;
		if (sourceSq.file() != -1 && sourceSq2.file() != sourceSq.file())
			continue;
		if (move.promotion() != promotion)
			continue;

		if (!vIsLegalMove(move))
			continue;

		// Return an empty move if there are multiple moves that
		// match the move string.
		if (match != nullptr)
			return Move();
		match = &move;
	}

	if (match != nullptr)
		return *match;

	return Move();
}

void ShogiBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	Side side = sideToMove();
	int source = move.sourceSquare();
	int target = move.targetSquare();
	int prom = move.promotion();

	MoveData md {pieceAt(target)};
	m_history.append(md);

	// Add captured pieces to reserve and remove dropped pieces
	int ctype = captureType(move);
	if (ctype != Piece::NoPiece)
	{
		Piece reservePiece(side, reserveType(ctype));
		addToReserve(reservePiece);
		if (transition != nullptr)
			transition->addReservePiece(reservePiece);
	}
	else if (source == 0)
		removeFromReserve(Piece(side, prom));

	// Transitions for normal moves and drop moves
	if (transition != nullptr)
	{
		if (source != 0)
			transition->addMove(chessSquare(source),
					    chessSquare(target));
		else
			transition->addDrop(Piece(side, prom),
					    chessSquare(target));
	}

	// Set pieces onto target square: promotions and normal moves
	if (prom != Piece::NoPiece)
		setSquare(target, Piece(side, prom));
	else
		setSquare(target, pieceAt(source));

	// Clear source square for normal moves
	if (source != 0)
		setSquare(source, Piece::NoPiece);

	// Keep track of the King
	if (source == m_kingSquare[side])
		m_kingSquare[side] = target;

	m_checks[side] = m_checks[side] << 1 | inCheck(side.opposite());
}

void ShogiBoard::vUndoMove(const Move& move)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();
	int prom = move.promotion();

	Side side = sideToMove();

	// Restore piece on source square for promotions and normal moves
	if (prom != Piece::NoPiece)
	{
		int normalType = normalPieceType(prom);
		if (source != 0)
			setSquare(source, Piece(side, normalType));
	}
	else
		setSquare(source, pieceAt(target));

	// Restore piece on target square from history
	const MoveData& md = m_history.last();
	setSquare(target, md.capture);

	// Undo effects on reserve of captures and drops
	int ctype = captureType(move);
	if (ctype != Piece::NoPiece)
		removeFromReserve(Piece(side, reserveType(ctype)));
	else if (source == 0)
		addToReserve(Piece(side, prom));

	// Keep track of the King
	if (target == m_kingSquare[side])
		m_kingSquare[side] = source;

	m_checks[side] >>= 1;
	m_history.pop_back();
}

/*
 * A normal move or a drop move is allowed if the piece is not a Pawn or
 * a Lance on the highest rank or a Knight on the highest two ranks.
*/
bool ShogiBoard::rankIsAllowed(int pieceType, int square) const
{
	if (pieceType != Pawn && pieceType != Lance && pieceType != Knight)
		return true;

	int rank = chessSquare(square).rank();
	int rrank = (sideToMove() == Side::White) ? rank : height() - 1 - rank;
	int distance = height() - rrank;

	return distance > 2 || (distance > 1 && pieceType != Knight);
}

/*
 * Rule of Two Pawns (二歩, nifu):
 * A Pawn must not be dropped onto a file where an own Pawn is already present
 */
bool ShogiBoard::fileIsAllowed(int pieceType, int square) const
{
	if (pieceType != Pawn)
		return true;

	Side side = sideToMove();
	int arwidth = width() + 2;
	int start = m_minIndex - 1 + square % arwidth;

	for (int i = start; i < m_maxIndex; i += arwidth)
	{
		if (pieceAt(i) == Piece(side, Pawn))
			return false;
	}
	return true;
}

bool ShogiBoard::inPromotionZone(int square) const
{
	Square sq = chessSquare(square);
	int rank = sq.rank();
	int rrank = (sideToMove() == Side::White) ? rank : height() - 1 - rank;

	return rrank >= m_promotionRank;
}

void ShogiBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					    int pieceType,
					    int square) const
{
	// Generate drops
	if (square == 0)
	{
		for (int i = m_minIndex; i < m_maxIndex; i++)
		{
			if (!pieceAt(i).isEmpty())
				continue;

			if (rankIsAllowed(pieceType, i)
			&&  fileIsAllowed(pieceType, i))
				moves.append(Move(0, i, pieceType));
		}
		return;
	}

	Side side = sideToMove();

	if (pieceType == King)
	{
		generateHoppingMoves(square, m_bishopOffsets, moves);
		generateHoppingMoves(square, m_rookOffsets, moves);
		return;
	}
	QVarLengthArray<Move>  moves1;
	if (pieceType == Pawn)
		generateHoppingMoves(square, m_lanceOffsets[side], moves1);
	if (pieceHasMovement(pieceType, square, BishopMovement))
		generateSlidingMoves(square, m_bishopOffsets, moves1);
	if (pieceHasMovement(pieceType, square, RookMovement))
		generateSlidingMoves(square, m_rookOffsets, moves1);
	if (pieceHasMovement(pieceType, square, FerzMovement))
		generateHoppingMoves(square, m_bishopOffsets, moves1);
	if (pieceHasMovement(pieceType, square, WazirMovement))
		generateHoppingMoves(square, m_rookOffsets, moves1);
	if (pieceHasMovement(pieceType, square, LanceMovement))
		generateSlidingMoves(square, m_lanceOffsets[side], moves1);
	if (pieceHasMovement(pieceType, square, KnightMovement))
		generateHoppingMoves(square, m_knightOffsets[side], moves1);
	if (pieceHasMovement(pieceType, square, SilverMovement))
		generateHoppingMoves(square, m_silverGeneralOffsets[side], moves1);
	if (pieceHasMovement(pieceType, square, GoldMovement))
		generateHoppingMoves(square, m_goldGeneralOffsets[side], moves1);

	// Generate promotions and append valid moves to the list
	int fromPromotionZone = inPromotionZone(square);
	int promotionType = promotedPieceType(pieceType);
	for (const Move m: moves1)
	{
		// No promotions for King, Gold and promoted piece types
		if (pieceType == promotionType)
		{
			moves.append(m);
			continue;
		}

		/*
		 * A normal move is allowed if the piece is not a Pawn or a Lance
		 * on the highest rank or a Knight on the highest two ranks.
		 */
		int target = m.targetSquare();

		if (rankIsAllowed(pieceType, target))
			moves.append(m);

		// Promotion is allowed in opponent's third of the board.
		if (fromPromotionZone || inPromotionZone(target))
			moves.append(Move(square, target, promotionType));
	}
}

bool ShogiBoard::ranksAreAllowed() const
{
	// No Pawns, Lances, and Knights allowed on the highest rank
	for (int i = m_minIndex; i < m_minIndex + width(); i++)
	{
		Piece piece = pieceAt(i);
		int type = piece.type();
		if ((type == Pawn || type == Lance || type == Knight)
		&&  piece.side() == Side::White)
			return false;

		piece = pieceAt(arraySize() - 1 - i);
		type = piece.type();
		if ((type == Pawn || type == Lance || type == Knight)
		&&  piece.side() == Side::Black)
			return false;
	}
	// No Knights allowed on the 2nd highest rank
	int arwidth = width() + 2;
	for (int i = m_minIndex; i < m_minIndex + width(); i++)
	{
		Piece piece = pieceAt(i + arwidth);
		if (piece == Piece(Side::White, Knight))
			return false;

		piece = pieceAt(arraySize() - 1 - i - arwidth);
		if (piece == Piece(Side::Black, Knight))
			return false;
	}

	return true;
}

bool ShogiBoard::isLegalPosition()
{
	Side opp = sideToMove().opposite();
	if (inCheck(opp))
		return false;

	return ranksAreAllowed();
}

bool ShogiBoard::vIsLegalMove(const Move& move)
{
	Q_ASSERT(!move.isNull());

	makeMove(move);
	bool isLegal = isLegalPosition();
	bool isIncheck = inCheck(sideToMove());

	// Illegal perpetual
	Side opp = sideToMove().opposite();
	if (isIncheck && repeatCount() > 2
	&&  (m_checks[opp] & 0b1111111) == 0b1111111)
		isLegal = false;

	// Illegal mate by Pawn drop?  (打ち歩詰め, uchi-fu-zume)
	if (move.promotion() == Pawn && move.sourceSquare() == 0)
	{
		if (isIncheck && !canMove())
			isLegal = false;
	}
	undoMove();

	return isLegal;
}

bool ShogiBoard::inCheck(Side side, int square) const
{
	if (square == 0)
		square = m_kingSquare[side];

	QVarLengthArray <Move> moves;
	if (sideToMove() == side)
		// needs C2-symmetry of piece movement between sides
		for (int type = Pawn; type <= PromotedRook; type++)
		{
			generateMovesForPiece(moves, type, square);
			for (const auto m: moves)
			{
				if (captureType(m) == type)
					return true;
			}
			moves.clear();
		}
	else
	{
		generateMoves(moves);
		for (const auto m: moves)
		{
			if (m.targetSquare() == square)
				return true;
		}
	}
	return false;
}


Result ShogiBoard::impassePointRule(int points, int pieces) const
{
	// 27-Point rule
	Side side = sideToMove();
	if (side == Side::Black)
		points++;

	if (points > 27 && pieces >= 10)
	{
		QString winStr = side == Side::White ? tr("Sente") : tr("Gote");
		QString str = tr("Impasse: %1 wins by 27-point rule").arg(winStr);
		return Result(Result::Win, side, str);
	}
	return Result();
}

Result ShogiBoard::resultFromImpassePointRule() const
{
	Side side = sideToMove();
	if (!inPromotionZone(m_kingSquare[side]) || inCheck(side))
		return Result();
	if (!inPromotionZone(m_kingSquare[side.opposite()]))
		return Result();

	// Subtract one count for the King
	int pieceValue = -1;
	int pieceCount = -1;

	// Count pieces in promotion zone and count piece values
	for (int i = m_minIndex; i < m_maxIndex; i++)
	{
		Piece piece = pieceAt(i);
		if (piece.side() != side)
			continue;

		if (inPromotionZone(i))
			pieceCount++;

		int type = piece.type();
		if (type == Bishop || type == Rook
		||  type == PromotedBishop || type == PromotedRook)
			pieceValue += 5;
		else
			pieceValue ++;
	}
	// Count pieces in hand
	for (int type = Pawn; type < King; type++)
	{
		if (type == Bishop || type == Rook)
			pieceValue += 5 * reserveCount(Piece(side, type));
		else
			pieceValue += reserveCount(Piece(side, type));
	}
	return impassePointRule(pieceValue, pieceCount);
}

Result ShogiBoard::result()
{
	Side side = sideToMove();
	Side opp = side.opposite();
	QString str;

	// Checkmate/Stalemate
	if (!canMove())
	{
		QString winStr = opp == Side::White ? tr("Sente") : tr("Gote");
		if (inCheck(side))
			str = tr("%1 mates").arg(winStr);
		else
			str = tr("%1 wins by stalemate").arg(winStr);

		return Result(Result::Win, opp, str);
	}

	// 4-fold repetition
	if (repeatCount() >= 3)
	{
		if (inCheck(side)
		&& (m_checks[opp] & 0b1111111) == 0b1111111)
		{
			str = tr("Illegal perpetual");
			return Result(Result::Win, side, str);
		}
		// Sennichite 千日手
		str = tr("Draw by fourfold repetition");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// Entering Kings (相入玉, ai-nyū gyoku) and Impasse (持将棋, jishōgi)
	if (m_hasImpassePointRule)
		return resultFromImpassePointRule();

	return Result();
}

} // namespace Chess

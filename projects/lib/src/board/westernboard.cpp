/*
    This file is part of Cute Chess.

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

#include "westernboard.h"
#include <QStringList>
#include "westernzobrist.h"
#include "boardtransition.h"


namespace Chess {

WesternBoard::WesternBoard(WesternZobrist* zobrist)
	: Board(zobrist),
	  m_arwidth(0),
	  m_sign(1),
	  m_enpassantSquare(0),
	  m_enpassantTarget(0),
	  m_plyOffset(0),
	  m_reversibleMoveCount(0),
	  m_kingCanCapture(true),
	  m_hasCastling(true),
	  m_pawnHasDoubleStep(true),
	  m_hasEnPassantCaptures(true),
	  m_pawnAmbiguous(false),
	  m_multiDigitNotation(false),
	  m_zobrist(zobrist)
{
	setPieceType(Pawn, tr("pawn"), "P");
	setPieceType(Knight, tr("knight"), "N", KnightMovement);
	setPieceType(Bishop, tr("bishop"), "B", BishopMovement);
	setPieceType(Rook, tr("rook"), "R", RookMovement);
	setPieceType(Queen, tr("queen"), "Q", BishopMovement | RookMovement);
	setPieceType(King, tr("king"), "K");

	m_pawnSteps += {CaptureStep, -1};
	m_pawnSteps += {FreeStep, 0};
	m_pawnSteps += {CaptureStep, 1};
}

int WesternBoard::width() const
{
	return 8;
}

int WesternBoard::height() const
{
	return 8;
}

bool WesternBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings == 1 && blackKings == 1;
}

bool WesternBoard::kingCanCapture() const
{
	return true;
}

bool WesternBoard::hasCastling() const
{
	return true;
}

bool WesternBoard::pawnHasDoubleStep() const
{
	return true;
}

bool WesternBoard::hasEnPassantCaptures() const
{
	return pawnHasDoubleStep();
}

bool WesternBoard::variantHasChanneling(Side, int) const
{
	return false;
}

void WesternBoard::vInitialize()
{
	m_kingCanCapture = kingCanCapture();
	m_hasCastling = hasCastling();
	m_pawnHasDoubleStep = pawnHasDoubleStep();
	m_hasEnPassantCaptures = hasEnPassantCaptures();

	m_arwidth = width() + 2;

	m_castlingRights.rookSquare[Side::White][QueenSide] = 0;
	m_castlingRights.rookSquare[Side::White][KingSide] = 0;
	m_castlingRights.rookSquare[Side::Black][QueenSide] = 0;
	m_castlingRights.rookSquare[Side::Black][KingSide] = 0;

	m_kingSquare[Side::White] = 0;
	m_kingSquare[Side::Black] = 0;

	m_castleTarget[Side::White][QueenSide] = (height() + 1) * m_arwidth + 1 + castlingFile(QueenSide);
	m_castleTarget[Side::White][KingSide] = (height() + 1) * m_arwidth + 1 + castlingFile(KingSide);
	m_castleTarget[Side::Black][QueenSide] = 2 * m_arwidth + 1 + castlingFile(QueenSide);
	m_castleTarget[Side::Black][KingSide] = 2 * m_arwidth + 1 + castlingFile(KingSide);

	m_knightOffsets.resize(8);
	m_knightOffsets[0] = -2 * m_arwidth - 1;
	m_knightOffsets[1] = -2 * m_arwidth + 1;
	m_knightOffsets[2] = -m_arwidth - 2;
	m_knightOffsets[3] = -m_arwidth + 2;
	m_knightOffsets[4] = m_arwidth - 2;
	m_knightOffsets[5] = m_arwidth + 2;
	m_knightOffsets[6] = 2 * m_arwidth - 1;
	m_knightOffsets[7] = 2 * m_arwidth + 1;

	m_bishopOffsets.resize(4);
	m_bishopOffsets[0] = -m_arwidth - 1;
	m_bishopOffsets[1] = -m_arwidth + 1;
	m_bishopOffsets[2] = m_arwidth - 1;
	m_bishopOffsets[3] = m_arwidth + 1;

	m_rookOffsets.resize(4);
	m_rookOffsets[0] = -m_arwidth;
	m_rookOffsets[1] = -1;
	m_rookOffsets[2] = 1;
	m_rookOffsets[3] = m_arwidth;

	m_pawnAmbiguous = (pawnAmbiguity(FreeStep) > 1);
	m_multiDigitNotation =  (height() > 9 && coordinateSystem() == NormalCoordinates)
			     || (width() > 9 && coordinateSystem() == InvertedCoordinates);
}

inline int WesternBoard::pawnPushOffset(const PawnStep& ps, int sign) const
{
	return sign * ps.file - sign * m_arwidth * 1;
}

int WesternBoard::captureType(const Move& move) const
{
	if (pieceAt(move.sourceSquare()).type() == Pawn
	&&  move.targetSquare() == m_enpassantSquare)
		return Pawn;

	return Board::captureType(move);
}

WesternBoard::CastlingSide WesternBoard::castlingSide(const Move& move) const
{
	int target = move.targetSquare();
	const int* rookSq = m_castlingRights.rookSquare[sideToMove()];
	if (target == rookSq[QueenSide])
		return QueenSide;
	if (target == rookSq[KingSide])
		return KingSide;
	return NoCastlingSide;
}

QString WesternBoard::lanMoveString(const Move& move)
{
	CastlingSide cside = castlingSide(move);
	if (cside != NoCastlingSide && !isRandomVariant())
	{
		Move tmp(move.sourceSquare(),
			 m_castleTarget[sideToMove()][cside]);
		return Board::lanMoveString(tmp);
	}

	return Board::lanMoveString(move);
}

QString WesternBoard::sanMoveString(const Move& move)
{
	QString str;
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece = pieceAt(source);
	Piece capture = pieceAt(target);
	Square square = chessSquare(source);

	if (source == target)
		capture = Piece::NoPiece;

	char checkOrMate = 0;
	makeMove(move);
	if (inCheck(sideToMove()))
	{
		if (canMove())
			checkOrMate = '+';
		else
			checkOrMate = '#';
	}
	undoMove();

	// drop move
	if (source == 0 && move.promotion() != Piece::NoPiece)
	{
		str = lanMoveString(move);
		if (checkOrMate != 0)
			str += checkOrMate;
		return str;
	}

	bool needRank = false;
	bool needFile = false;
	Side side = sideToMove();

	if (piece.type() == Pawn)
	{
		if (m_pawnAmbiguous)
		{
			needFile = true;
			needRank = true; // for Xboard-compatibility
		}
		if (target == m_enpassantSquare)
			capture = Piece(side.opposite(), Pawn);
		if (capture.isValid())
			needFile = true;
	}
	else if (piece.type() == King)
	{
		CastlingSide cside = castlingSide(move);
		if (cside != NoCastlingSide)
		{
			if (cside == QueenSide)
				str = "O-O-O";
			else
				str = "O-O";
			if (checkOrMate != 0)
				str += checkOrMate;
			return str;
		}
	}
	if (piece.type() != Pawn)	// not pawn
	{
		str += pieceSymbol(piece).toUpper();
		QVarLengthArray<Move> moves;
		generateMoves(moves, piece.type());

		for (int i = 0; i < moves.size(); i++)
		{
			const Move& move2 = moves[i];
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
	}
	if (needFile)
		str += QChar('a' + square.file());
	if (needRank)
		str += QString::number(1 + square.rank());

	if (capture.isValid())
		str += 'x';

	str += squareString(target);

	if (move.promotion() != Piece::NoPiece)
		str += "=" + pieceSymbol(move.promotion()).toUpper();

	if (checkOrMate != 0)
		str += checkOrMate;

	return str;
}

Move WesternBoard::moveFromLanString(const QString& str)
{
	Move move(Board::moveFromLanString(str));

	Side side = sideToMove();
	int source = move.sourceSquare();
	int target = move.targetSquare();

	if (source == m_kingSquare[side]
	&&  qAbs(source - target) != 1)
	{
		const int* rookSq = m_castlingRights.rookSquare[side];
		if (target == m_castleTarget[side][QueenSide])
			target = rookSq[QueenSide];
		else if (target == m_castleTarget[side][KingSide])
			target = rookSq[KingSide];

		if (target != 0)
			return Move(source, target);
	}

	return move;
}

Move WesternBoard::moveFromSanString(const QString& str)
{
	if (str.length() < 2)
		return Move();

	QString mstr = str;
	Side side = sideToMove();

	// Ignore check/mate/strong move/blunder notation
	while (mstr.endsWith('+') || mstr.endsWith('#')
	||     mstr.endsWith('!') || mstr.endsWith('?'))
	{
		mstr.chop(1);
	}

	if (mstr.length() < 2)
		return Move();

	// Castling
	if (mstr.startsWith("O-O"))
	{
		CastlingSide cside;
		if (mstr == "O-O")
			cside = KingSide;
		else if (mstr == "O-O-O")
			cside = QueenSide;
		else
			return Move();

		int source = m_kingSquare[side];
		int target = m_castlingRights.rookSquare[side][cside];

		Move move(source, target);
		if (isLegalMove(move))
			return move;
		else
			return Move();
	}

	// number of digits in notation of squares
	int digits = 1;

	// only for tall boards: find maximum number of sequential digits
	if (m_multiDigitNotation)
	{
		int count = 0;
		for (const QChar& ch : qAsConst(mstr))
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

	// A SAN move can't start with the capture mark, and
	if (*it == 'x')
		return Move();
	// a pawn move should not specify the piece type
	if (pieceFromSymbol(*it) == Pawn)
		it++; // ignore character
	// Piece type
	Piece piece = pieceFromSymbol(*it);
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
		if (*it == '@')
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

	bool stringIsCapture = false;

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
			stringIsCapture = true;
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

	// Make sure that the move string is right about whether
	// or not the move is a capture.
	bool isCapture = false;
	if (pieceAt(target).side() == side.opposite()
	||  (target == m_enpassantSquare && piece.type() == Pawn))
		isCapture = true;
	if (isCapture != stringIsCapture)
		return Move();

	// Promotion
	int promotion = Piece::NoPiece;
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
		// Castling moves were handled earlier
		if (pieceAt(target) == Piece(side, Rook))
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

QString WesternBoard::castlingRightsString(FenNotation notation) const
{
	QString str;

	for (int side = Side::White; side <= Side::Black; side++)
	{
		for (int cside = KingSide; cside >= QueenSide; cside--)
		{
			int rs = m_castlingRights.rookSquare[side][cside];
			if (rs == 0)
				continue;

			int offset = (cside == QueenSide) ? -1: 1;
			Piece piece;
			int i = rs + offset;
			bool ambiguous = false;

			// If the castling rook is not the outernmost rook,
			// the castling square is ambiguous
			while (!(piece = pieceAt(i)).isWall())
			{
				if (piece == Piece(Side::Type(side), Rook))
				{
					ambiguous = true;
					break;
				}
				i += offset;
			}

			QChar c;
			// If the castling square is ambiguous, then we can't
			// use 'K' or 'Q'. Instead we'll use the square's file.
			if (ambiguous || notation == ShredderFen)
				c = QChar('a' + chessSquare(rs).file());
			else
			{
				if (cside == 0)
					c = 'q';
				else
					c = 'k';
			}
			if (side == upperCaseSide())
				c = c.toUpper();
			str += c;
		}
	}

	if (str.length() == 0)
		str = "-";
	return str;
}

int WesternBoard::pawnAmbiguity(StepType t) const
{
	int count = 0;
	for (const auto a: m_pawnSteps)
	{
		if (a.type == t)
			++count;
	}
	return count;
}

QString WesternBoard::vFenIncludeString(FenNotation notation) const
{
	Q_UNUSED(notation);
	return "";
}

QString WesternBoard::vFenString(FenNotation notation) const
{
	// Castling rights
	QString fen = castlingRightsString(notation) + ' ';

	// En-passant square
	if (m_enpassantSquare != 0)
	{
		fen += squareString(m_enpassantSquare);
		if (m_pawnAmbiguous)
			fen += squareString(m_enpassantTarget);
	}
	else
		fen += '-';

	fen +=vFenIncludeString(notation);

	// Reversible halfmove count
	fen += ' ';
	fen += QString::number(m_reversibleMoveCount);

	// Full move number
	fen += ' ';
	fen += QString::number((m_history.size() + m_plyOffset) / 2 + 1);

	return fen;
}

bool WesternBoard::parseCastlingRights(QChar c)
{
	if (!m_hasCastling)
		return false;

	int offset = 0;
	CastlingSide cside = NoCastlingSide;
	Side side = (c.isUpper()) ? upperCaseSide() : upperCaseSide().opposite();
	c = c.toLower();

	if (c == 'q')
	{
		cside = QueenSide;
		offset = -1;
	}
	else if (c == 'k')
	{
		cside = KingSide;
		offset = 1;
	}

	int kingSq = m_kingSquare[side];

	if (offset != 0)
	{
		Piece piece;
		int i = kingSq + offset;
		int rookSq = 0;

		// Locate the outernmost rook on the castling side
		while (!(piece = pieceAt(i)).isWall())
		{
			if (piece == Piece(side, Rook))
				rookSq = i;
			i += offset;
		}
		if (rookSq != 0)
		{
			setCastlingSquare(side, cside, rookSq);
			return true;
		}
	}
	else	// Shredder FEN or X-FEN
	{
		int file = c.toLatin1() - 'a';
		if (file < 0 || file >= width())
			return false;

		// Get the rook's source square
		int rookSq;
		if (side == Side::White)
			rookSq = (height() + 1) * m_arwidth + 1 + file;
		else
			rookSq = 2 * m_arwidth + 1 + file;

		// Make sure the king and the rook are on the same rank
		if (abs(kingSq - rookSq) >= width())
			return false;

		// Update castling rights in the FenData object
		if (pieceAt(rookSq) == Piece(side, Rook))
		{
			if (rookSq > kingSq)
				cside = KingSide;
			else
				cside = QueenSide;
			setCastlingSquare(side, cside, rookSq);
			return true;
		}
	}

	return false;
}

bool WesternBoard::vSetFenString(const QStringList& fen)
{
	if (fen.size() < 2)
		return false;
	QStringList::const_iterator token = fen.begin();

	// Find the king squares
	int kingCount[2] = {0, 0};
	for (int sq = 0; sq < arraySize(); sq++)
	{
		Piece tmp = pieceAt(sq);
		if (tmp.type() == King)
		{
			m_kingSquare[tmp.side()] = sq;
			kingCount[tmp.side()]++;
		}
	}
	if (!kingsCountAssertion(kingCount[Side::White],
				 kingCount[Side::Black]))
		return false;

	// short non-standard format without castling and ep fields?
	bool isShortFormat = false;
	if (fen.size() < 3)
		token->toInt(&isShortFormat);

	// allowed only for variants without castling and en passant captures
	if (isShortFormat && (m_hasCastling || m_hasEnPassantCaptures))
		return false;

	// Castling rights
	m_castlingRights.rookSquare[Side::White][QueenSide] = 0;
	m_castlingRights.rookSquare[Side::White][KingSide] = 0;
	m_castlingRights.rookSquare[Side::Black][QueenSide] = 0;
	m_castlingRights.rookSquare[Side::Black][KingSide] = 0;

	if (!isShortFormat)
	{
		if (*token != "-")
		{
			QString::const_iterator c;
			for (c = token->begin(); c != token->end(); ++c)
			{
				if (!parseCastlingRights(*c))
					return false;
			}
		}
		++token;
	}
	// En-passant square
	m_enpassantSquare = 0;
	Side side(sideToMove());
	m_sign = (side == Side::White) ? 1 : -1;

	if (m_hasEnPassantCaptures && *token != "-")
	{
		int epSq = squareIndex(*token);
		int fenEpTgt = 0;
		// ambiguous ep variants: read ep square [and target]
		if (m_pawnAmbiguous)
		{
			for (int i = 2; i <= token->length(); i++)
			{
				epSq = squareIndex(token->left(i));
				fenEpTgt = squareIndex(token->mid(i));
				if (epSq != 0 && fenEpTgt != 0)
					break;
			}
		}
		setEnpassantSquare(epSq);
		if (m_enpassantSquare == 0)
			return false;

		Piece ownPawn(side, Pawn);
		Piece opPawn(side.opposite(), Pawn);
		int matchesOwn = 0;
		int epTgt = 0;

		for (const PawnStep& pStep: m_pawnSteps)
		{
			int sq = epSq - pawnPushOffset(pStep, m_sign);
			Piece piece = pieceAt(sq);
			if (pStep.type == CaptureStep && piece == ownPawn)
				matchesOwn++;
			else if (pStep.type == FreeStep && piece == opPawn
			&&      (fenEpTgt == 0 || fenEpTgt == sq))
				epTgt = sq;
		}
		// Ignore the en-passant square if an en-passant
		// capture isn't possible.
		if (!matchesOwn)
			setEnpassantSquare(0);
		else
			// set ep square and target
			setEnpassantSquare(epSq, epTgt);
	}
	if (!isShortFormat)
		++token;

	// Reversible halfmove count
	if (token != fen.end())
	{
		bool ok;
		int tmp = token->toInt(&ok);
		if (!ok || tmp < 0)
			return false;
		m_reversibleMoveCount = tmp;
		++token;
	}
	else
		m_reversibleMoveCount = 0;

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

	if (m_sign != 1)
		m_plyOffset++;

	m_history.clear();
	return true;
}

void WesternBoard::setEnpassantSquare(int square, int target)
{

	m_enpassantTarget = square ? target : 0;
	if (square == m_enpassantSquare)
		return;

	if (m_enpassantSquare != 0)
		xorKey(m_zobrist->enpassant(m_enpassantSquare));
	if (square != 0)
		xorKey(m_zobrist->enpassant(square));

	m_enpassantSquare = square;
}

void WesternBoard::setCastlingSquare(Side side,
				     CastlingSide cside,
				     int square)
{
	int& rs = m_castlingRights.rookSquare[side][cside];
	if (rs == square)
		return;

	if (rs != 0)
		xorKey(m_zobrist->castling(side, rs));
	if (square != 0)
		xorKey(m_zobrist->castling(side, square));
	rs = square;
}

void WesternBoard::removeCastlingRights(int square)
{
	Piece piece = pieceAt(square);
	if (piece.type() != Rook)
		return;

	Side side(piece.side());
	const int* cr = m_castlingRights.rookSquare[side];

	if (square == cr[QueenSide])
		setCastlingSquare(side, QueenSide, 0);
	else if (square == cr[KingSide])
		setCastlingSquare(side, KingSide, 0);
}

void WesternBoard::removeCastlingRights(Side side)
{
	setCastlingSquare(side, QueenSide, 0);
	setCastlingSquare(side, KingSide, 0);
}

int WesternBoard::castlingFile(CastlingSide castlingSide) const
{
	Q_ASSERT(castlingSide != NoCastlingSide);
	return castlingSide == QueenSide ? 2 : width() - 2; // usually C and G
}

void WesternBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	Side side = sideToMove();
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece capture = pieceAt(target);
	int promotionType = move.promotion();
	int pieceType = pieceAt(source).type();
	int epSq = m_enpassantSquare;
	int epTgt = m_enpassantTarget;
	int* rookSq = m_castlingRights.rookSquare[side];
	bool clearSource = true;
	bool isReversible = true;

	Q_ASSERT(target != 0);

	MoveData md = { capture, epSq, epTgt, m_castlingRights,
			NoCastlingSide, m_reversibleMoveCount };

	if (source == 0)
	{
		Q_ASSERT(promotionType != Piece::NoPiece);

		pieceType = promotionType;
		promotionType = Piece::NoPiece;
		clearSource = false;
		isReversible = false;
		epSq = 0;
	}

	if (source == target)
		clearSource = 0;

	setEnpassantSquare(0);

	if (pieceType == King)
	{
		// In case of a castling move, make the rook's move
		CastlingSide cside = castlingSide(move);
		if (cside != NoCastlingSide)
		{
			md.castlingSide = cside;
			int rookSource = target;
			target = m_castleTarget[side][cside];
			int rookTarget = (cside == QueenSide) ? target + 1 : target -1;
			if (rookTarget == source || target == source)
				clearSource = false;

			Piece rook = Piece(side, Rook);
			setSquare(rookSource, Piece::NoPiece);
			setSquare(rookTarget, rook);
			// FIDE rules 5.2, 9.3, PGN/FEN spec. 16.1.3.5:
			// 50-moves counting goes on when castling.

			if (transition != nullptr)
				transition->addMove(chessSquare(rookSource),
						    chessSquare(rookTarget));
		}
		m_kingSquare[side] = target;
		// Any king move removes all castling rights
		setCastlingSquare(side, QueenSide, 0);
		setCastlingSquare(side, KingSide, 0);
	}
	else if (pieceType == Pawn)
	{
		isReversible = false;

		// Make an en-passant capture
		if (target == epSq)
		{
			int epTarget = epTgt;
			setSquare(epTarget, Piece::NoPiece);

			if (transition != nullptr)
				transition->addSquare(chessSquare(epTarget));
		}
		// Push a pawn two squares ahead, creating an en-passant
		// opportunity for the opponent.
		else if ((source / m_arwidth - target / m_arwidth) * m_sign == 2)
		{
			epSq = (source + target) / 2;
			const Piece opPawn(side.opposite(), Pawn);
			for (const PawnStep& pstep: m_pawnSteps)
			{
				if (pstep.type == CaptureStep 
				 && opPawn == pieceAt(epSq + pawnPushOffset(pstep, m_sign)))
				{
					setEnpassantSquare(epSq, target);
				}
			}
		}
		else if (promotionType != Piece::NoPiece)
			pieceType = promotionType;
	}
	else if (pieceType == Rook)
	{
		// Remove castling rights from the rook's square
		for (int i = QueenSide; i <= KingSide; i++)
		{
			if (source == rookSq[i])
			{
				setCastlingSquare(side, CastlingSide(i), 0);
				// FIDE rules 5.2, 9.3, PGN/FEN spec. 16.1.3.5:
				// 50-moves counting goes on when castling.
				break;
			}
		}
	}

	if (captureType(move) != Piece::NoPiece)
	{
		removeCastlingRights(target);
		isReversible = false;
	}

	if (promotionType != Piece::NoPiece)
		isReversible = false;

	if (transition != nullptr)
	{
		if (source != 0)
			transition->addMove(chessSquare(source),
					    chessSquare(target));
		else
			transition->addDrop(Piece(side, pieceType),
					    chessSquare(target));
	}

	setSquare(target, Piece(side, pieceType));
	if (clearSource)
		setSquare(source, Piece::NoPiece);

	if (isReversible)
		m_reversibleMoveCount++;
	else
		m_reversibleMoveCount = 0;

	m_history.append(md);
	m_sign *= -1;
}

void WesternBoard::vUndoMove(const Move& move)
{
	const MoveData& md = m_history.last();
	int source = move.sourceSquare();
	int target = move.targetSquare();

	m_sign *= -1;
	Side side = sideToMove();

	setEnpassantSquare(md.enpassantSquare, md.enpassantTarget);
	m_reversibleMoveCount = md.reversibleMoveCount;
	m_castlingRights = md.castlingRights;

	CastlingSide cside = md.castlingSide;
	if (cside != NoCastlingSide)
	{
		m_kingSquare[side] = source;
		// Move the rook back after castling
		int tmp = m_castleTarget[side][cside];
		setSquare(tmp, Piece::NoPiece);
		tmp = (cside == QueenSide) ? tmp + 1 : tmp - 1;
		setSquare(tmp, Piece::NoPiece);

		setSquare(target, Piece(side, Rook));
		setSquare(source, Piece(side, King));
		m_history.pop_back();
		return;
	}
	else if (target == m_kingSquare[side])
	{
		m_kingSquare[side] = source;
	}
	else if (target == m_enpassantSquare)
	{
		// Restore the pawn captured by the en-passant move
		int epTarget = m_enpassantTarget;
		setSquare(epTarget, Piece(side.opposite(), Pawn));
	}

	if (move.promotion() != Piece::NoPiece)
	{
		if (source != 0)
		{
			if (variantHasChanneling(side, source))
				setSquare(source, pieceAt(target));
			else
				setSquare(source, Piece(side, Pawn));
		}
	}
	else
		setSquare(source, pieceAt(target));

	setSquare(target, md.capture);
	m_history.pop_back();
}

void WesternBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					 int pieceType,
					 int square) const
{
	if (pieceType == Pawn)
	{
		generatePawnMoves(square, moves);
		return;
	}
	if (pieceType == King)
	{
		generateHoppingMoves(square, m_bishopOffsets, moves);
		generateHoppingMoves(square, m_rookOffsets, moves);
		generateCastlingMoves(moves);
		return;
	}

	if (pieceHasMovement(pieceType, square, KnightMovement))
		generateHoppingMoves(square, m_knightOffsets, moves);
	if (pieceHasMovement(pieceType, square, BishopMovement))
		generateSlidingMoves(square, m_bishopOffsets, moves);
	if (pieceHasMovement(pieceType, square, RookMovement))
		generateSlidingMoves(square, m_rookOffsets, moves);
}

bool WesternBoard::defendedByKnight(Side side, int square) const
{
	for (int i = 0; i < m_knightOffsets.size(); i++)
	{
		int targetSquare = square + m_knightOffsets[i];
		Piece piece = pieceAt(targetSquare);
		if (piece.side() == side
		&&  pieceHasCaptureMovement(piece, targetSquare, KnightMovement))
			return true;
	}
    return false;
}

bool WesternBoard::inCheck(Side side, int square) const
{
	Side opSide = side.opposite();
	if (square == 0)
	{
		square = m_kingSquare[side];
		// In the "horde" variant the horde side has no king
		if (square == 0)
			return false;
	}

	// Pawn attacks
	int sign = (side == Side::White) ? 1 : -1;

	for (const PawnStep& pStep: m_pawnSteps)
	{
		if (pStep.type == CaptureStep)
		{
			int fromSquare = square - pawnPushOffset(pStep, -sign);
			if (pieceAt(fromSquare) == Piece(opSide, Pawn))
				return true;
		}
	}

	Piece opKing(opSide, King);
	Piece piece;
	
	// Knight, archbishop, chancellor attacks
	for (int i = 0; i < m_knightOffsets.size(); i++)
	{
		int targetSquare = square + m_knightOffsets[i];
		piece = pieceAt(targetSquare);
		if (piece.side() == opSide
		&&  pieceHasCaptureMovement(piece, targetSquare, KnightMovement))
			return true;
	}
	
	// Bishop, queen, archbishop, king attacks
	for (int i = 0; i < m_bishopOffsets.size(); i++)
	{
		int offset = m_bishopOffsets[i];
		int targetSquare = square + offset;
		if (m_kingCanCapture
		&&  pieceAt(targetSquare) == opKing)
			return true;
		while ((piece = pieceAt(targetSquare)).isEmpty()
		||     piece.side() == opSide)
		{
			if (!piece.isEmpty())
			{
				if (pieceHasCaptureMovement(piece, targetSquare, BishopMovement))
					return true;
				break;
			}
			targetSquare += offset;
		}
	}
	
	// Rook, queen, chancellor, king attacks
	for (int i = 0; i < m_rookOffsets.size(); i++)
	{
		int offset = m_rookOffsets[i];
		int targetSquare = square + offset;
		if (m_kingCanCapture
		&&  pieceAt(targetSquare) == opKing)
			return true;
		while ((piece = pieceAt(targetSquare)).isEmpty()
		||     piece.side() == opSide)
		{
			if (!piece.isEmpty())
			{
				if (pieceHasCaptureMovement(piece, targetSquare, RookMovement))
					return true;
				break;
			}
			targetSquare += offset;
		}
	}
	
	return false;
}

bool WesternBoard::isLegalPosition()
{
	Side side = sideToMove().opposite();
	if (inCheck(side))
		return false;

	if (m_history.isEmpty())
		return true;

	const Move& move = lastMove();

	// Make sure that no square between the king's initial and final
	// squares (including the initial and final squares) are under
	// attack (in check) by the opponent.
	CastlingSide cside = m_history.last().castlingSide;
	if (cside != NoCastlingSide)
	{
		int source = move.sourceSquare();
		int target = m_castleTarget[side][cside];
		int offset = (source <= target) ? 1 : -1;

		for (int i = source; i != target; i += offset)
		{
			if (inCheck(side, i))
				return false;
		}
	}

	return true;
}

bool WesternBoard::vIsLegalMove(const Move& move)
{
	Q_ASSERT(!move.isNull());

	Side side(sideToMove());

	if (move.sourceSquare() == m_kingSquare[side])
	{
		if (!m_kingCanCapture
		&&  captureType(move) != Piece::NoPiece)
			return false;

		// No castling when in check
		Piece piece = pieceAt(move.targetSquare());
		if (m_hasCastling
		&&  piece.type() == Rook
		&&  piece.side() == side
		&&  inCheck(side))
			return false;
	}

	return Board::vIsLegalMove(move);
}

void WesternBoard::addPromotions(int sourceSquare,
				 int targetSquare,
				 QVarLengthArray<Move>& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, Knight));
	moves.append(Move(sourceSquare, targetSquare, Bishop));
	moves.append(Move(sourceSquare, targetSquare, Rook));
	moves.append(Move(sourceSquare, targetSquare, Queen));
}

void WesternBoard::generatePawnMoves(int sourceSquare,
				     QVarLengthArray<Move>& moves) const
{
	int targetSquare;
	Piece capture;
	int step = m_sign * m_arwidth;
	bool isPromotion = pieceAt(sourceSquare - step * 2).isWall();

	// Normal moves, Captures, including en-passant moves
	Side opSide(sideToMove().opposite());

	for (const PawnStep& pStep: m_pawnSteps)
	{
		targetSquare = sourceSquare + pawnPushOffset(pStep, m_sign);
		capture = pieceAt(targetSquare);
		bool isCapture = capture.side() == opSide
				||  targetSquare == enpassantSquare();
		bool isNormalStep = capture.isEmpty();

		if ((isNormalStep && pStep.type == FreeStep)
		||  (isCapture && pStep.type == CaptureStep))
		{
			if (isPromotion)
				addPromotions(sourceSquare, targetSquare, moves);
			else
				moves.append(Move(sourceSquare, targetSquare));

			// Double step
			if (isNormalStep
			&&  m_pawnHasDoubleStep
			&&  pieceAt(sourceSquare + step * 2).isWall())
			{
				targetSquare += pawnPushOffset(pStep, m_sign);
				capture = pieceAt(targetSquare);
				if (capture.isEmpty())
					moves.append(Move(sourceSquare, targetSquare));
			}
		}
	}
}

bool WesternBoard::canCastle(CastlingSide castlingSide) const
{
	Side side = sideToMove();
	int rookSq = m_castlingRights.rookSquare[side][castlingSide];
	if (rookSq == 0)
		return false;

	int kingSq = m_kingSquare[side];
	int target = m_castleTarget[side][castlingSide];
	int left;
	int right;
	int rtarget;

	// Find all the squares involved in the castling
	if (castlingSide == QueenSide)
	{
		rtarget = target + 1;

		if (target < rookSq)
			left = target;
		else
			left = rookSq;

		if (rtarget > kingSq)
			right = rtarget;
		else
			right = kingSq;
	}
	else	// Kingside
	{
		rtarget = target - 1;

		if (rtarget < kingSq)
			left = rtarget;
		else
			left = kingSq;

		if (target > rookSq)
			right = target;
		else
			right = rookSq;
	}

	// Make sure that the smallest back rank interval containing the king,
	// the castling rook, and their destination squares contains no pieces
	// other than the king and the castling rook.
	for (int i = left; i <= right; i++)
	{
		if (i != kingSq && i != rookSq && !pieceAt(i).isEmpty())
			return false;
	}

	return true;
}

void WesternBoard::generateCastlingMoves(QVarLengthArray<Move>& moves) const
{
	Side side = sideToMove();
	int source = m_kingSquare[side];
	for (int i = QueenSide; i <= KingSide; i++)
	{
		if (canCastle(CastlingSide(i)))
		{
			int target = m_castlingRights.rookSquare[side][i];
			moves.append(Move(source, target));
		}
	}
}

int WesternBoard::kingSquare(Side side) const
{
	Q_ASSERT(!side.isNull());
	return m_kingSquare[side];
}

int WesternBoard::enpassantSquare() const
{
	return m_enpassantSquare;
}

bool WesternBoard::hasCastlingRight(Side side, CastlingSide castlingSide) const
{
	return m_castlingRights.rookSquare[side][castlingSide] != 0;
}

int WesternBoard::reversibleMoveCount() const
{
	return m_reversibleMoveCount;
}

Result WesternBoard::result()
{
	QString str;

	// Checkmate/Stalemate
	if (!canMove())
	{
		if (inCheck(sideToMove()))
		{
			Side winner = sideToMove().opposite();
			str = tr("%1 mates").arg(winner.toString());

			return Result(Result::Win, winner, str);
		}
		else
		{
			str = tr("Draw by stalemate");
			return Result(Result::Draw, Side::NoSide, str);
		}
	}

	// Insufficient mating material
	int material = 0;
	bool bishops[] = { false, false };
	for (int i = 0; i < arraySize(); i++)
	{
		const Piece& piece = pieceAt(i);
		if (!piece.isValid())
			continue;

		switch (piece.type())
		{
		case King:
			break;
		case Bishop:
		{
			auto color = chessSquare(i).color();
			if (color != Square::NoColor && !bishops[color])
			{
				material++;
				bishops[color] = true;
			}
			break;
		}
		case Knight:
			material++;
			break;
		default:
			material += 2;
			break;
		}
	}
	if (material <= 1)
	{
		str = tr("Draw by insufficient mating material");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 50 move rule
	if (m_reversibleMoveCount >= 100)
	{
		str = tr("Draw by fifty moves rule");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		str = tr("Draw by 3-fold repetition");
		return Result(Result::Draw, Side::NoSide, str);
	}

	return Result();
}

bool Chess::WesternBoard::winPossible(Chess::Side side) const
{
	// Find any piece besides the King
	int minIndex = 2 * m_arwidth + 1;
	for (int i = minIndex; i < arraySize() - minIndex - 1; i++)
	{
		Piece piece = pieceAt(i);
		if (piece.side() == side && i != kingSquare(side))
			return true;
	}
	if (variantHasDrops())
	{
		const QList<Piece>& reserveTypes = reservePieceTypes();
		for (const Piece& ptype: reserveTypes)
		{
			if (reserveCount(ptype) > 0 && ptype.side() == side)
				return true;
		}
	}
	return false;
}

} // namespace Chess

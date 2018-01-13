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

#include "seirawanboard.h"
#include "westernzobrist.h"
#include "boardtransition.h"

namespace Chess {

SeirawanBoard::SeirawanBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Hawk, tr("hawk"), "H", KnightMovement | BishopMovement, "A");
	setPieceType(Elephant, tr("elephant"), "E", KnightMovement | RookMovement, "C");

	// following "virtual" piece types are only used for castling/channeling disambiguation
	setPieceType(rookSquareChanneling(Hawk, forward), tr("auxhawk"), "X", 0, "A");
	setPieceType(rookSquareChanneling(Elephant, forward), tr("auxelephant"), "Y", 0, "C");
}

Board* SeirawanBoard::copy() const
{
	return new SeirawanBoard(*this);
}

QString SeirawanBoard::variant() const
{
	return "seirawan";
}

QString SeirawanBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[EHeh] w BCDFGbcdfgKQkq - 0 1";
}

int SeirawanBoard::rookSquareChanneling(int promotion, direction d = forward)
{
	return d == forward ? promotion + 8 : promotion - 8;
}

bool SeirawanBoard::variantHasDrops() const
{
	return true;
}

bool SeirawanBoard::variantHasChanneling(Side side, int square) const
{
	int rank = chessSquare(square).rank();
	int baserank = (side == Side::White) ? 0 : height() -1 ;
	return  rank == baserank && !side.isNull();
}

bool SeirawanBoard::variantHasOptionalPromotions() const
{
	return true;
}

QList<Piece> SeirawanBoard::reservePieceTypes() const
{
	QList<Piece> list;

	for (int i = 0; i < 2; i++)
	{
		list << Piece(Side::Type(i), Hawk);
		list << Piece(Side::Type(i), Elephant);
	}

	return list;
}

void SeirawanBoard::addPromotions(int sourceSquare,
				  int targetSquare,
				  QVarLengthArray<Move>& moves) const
{
	WesternBoard::addPromotions(sourceSquare, targetSquare, moves);

	moves.append(Move(sourceSquare, targetSquare, Hawk));
	moves.append(Move(sourceSquare, targetSquare, Elephant));
}

bool SeirawanBoard::vSetFenString(const QStringList& fen)
{
	m_squareMap.clear();
	return WesternBoard::vSetFenString(fen);
}

void SeirawanBoard::insertIntoSquareMap(int square, int count)
{
	m_squareMap.insert(square, count);
}

void SeirawanBoard::updateSquareMap(const Move& move, int increment)
{
	int source = move.sourceSquare();
	if (m_squareMap.contains(source))
		m_squareMap[source] += increment;

	int target = move.targetSquare();
	if (m_squareMap.contains(target))
		m_squareMap[target] += increment;
}

bool SeirawanBoard::parseCastlingRights(QChar c)
{
	Side side = (c.isUpper()) ? upperCaseSide() : upperCaseSide().opposite();
	QChar ch = c.toLower();
	QString rank = side == Side::White ? "1" : QString::number(height());

	if (ch >= 'a' && ch <= 'h')
	{
		insertIntoSquareMap(squareIndex(ch + rank));
		return true;
	}
	if (ch == 'k')
	{
		insertIntoSquareMap(kingSquare(side));
		insertIntoSquareMap(squareIndex("h" + rank));
	}
	if (ch == 'q')
	{
		insertIntoSquareMap(kingSquare(side));
		insertIntoSquareMap(squareIndex("a" + rank));
	}
	return WesternBoard::parseCastlingRights(c);
}

QString SeirawanBoard::vFenString(Board::FenNotation notation) const
{
	QString vfs = Chess::WesternBoard::vFenString(notation);
	int reservePieceCount[2] = {0, 0};

	const QList<Piece>& reserveTypes = reservePieceTypes();
	for (const Piece& pc: reserveTypes)
		reservePieceCount[pc.side()] += reserveCount(pc);

	// prepend castling field with list of files usable for piece channeling
	QString s;

	for (int i: m_squareMap.keys())
	{
		if (m_squareMap[i] > 0)
			continue;

		Piece piece = pieceAt(i);
		Side side = piece.side();
		if (!variantHasChanneling(side, i))
		{
			qWarning() << "channeling map mismatch for square "
				   << i << ": " << squareString(i);
			continue;
		}
		// do not list channeling files if there are no reserve pieces
		if (reservePieceCount[side] <= 0)
			continue;

		if (side == upperCaseSide())
			s.append(squareString(i).mid(0,1).toUpper());
		else
			s.append(squareString(i).mid(0,1).toLower());
	}

	QStringList vfen = vfs.split(' ', QString::SkipEmptyParts);

	if ((!vfen.isEmpty() && vfen.at(0)!= "-") || s.isEmpty())
		s.append(vfen.at(0));

	// remove redundant files which are also included in castling rights
	struct castlingfiles {QChar castling; QChar kingfile; QChar rookfile;};
	const castlingfiles files[4]{{'K','E','H'},{'Q','E','A'},{'k','e','h'},{'q','e','a'}};

	for (const auto& token: files)
		if (s.contains(token.castling))
		{
			s.remove(token.kingfile);
			s.remove(token.rookfile);
		}

	// append the rest of the FEN string
	for (int i = 1; i < vfen.count(); i++)
		s.append(" ").append(vfen.at(i));

	return s;
}

/*
 * This method uses algebraic notation (LAN) of standard chess. In addition
 * channeling moves are respresented as moves with promotions, e.g. b1c3h
 * inserts a Hawk at the vacated square b1. Castling with channeling onto King
 * square is coded likewise, e.g. e1g1e castles short and inserts an Elephant
 * at square e1. However, castling with piece insertion at the rook square is
 * represented as "Rook captures own King with promotion to reserve piece",
 * e.g. h1e1h inserts a Hawk on h1 after castling short.
 */
QString SeirawanBoard::lanMoveString(const Move& move)
{
	int promotion = move.promotion();
	QString str(Chess::WesternBoard::lanMoveString(move));

	// normal move or channeling /wo castling
	if (Board::lanMoveString(move) == str || !promotion)
		return str;

	// castling with channeling onto king square
	if (promotion < rookSquareChanneling(Hawk))
		return str + pieceSymbol(promotion).toLower();

	// castling with channeling onto rook square
	Move move1(move.targetSquare(),
		   move.sourceSquare(),
		   rookSquareChanneling(move.promotion(), backward));
	return Board::lanMoveString(move1);
}

/*
 * This method returns the same short algebraic notation (SAN) of a given move
 * as standard chess. However, channeling moves are denoted by adding a slash
 * and a piece symbol to the standard move notation. E.g. Nc3/H moves a Knight
 * to c3 and inserts a Hawk at the vacated square. Castling with channeling has
 * a similar notation, but needs disambigation: a piece from the reserve can be
 * inserted either at the vacated king square or the rook square. E.g. O-O/Ee1
 * castles short and drops an Elephant piece on square e1, while O-O/Eh1 castles
 * short and drops the piece on the vacated rook square h1.
 */
QString SeirawanBoard::sanMoveString(const Move& move)
{
	QString str(WesternBoard::sanMoveString(move));
	int promotion = move.promotion();
	int pieceType = promotion;
	Side side = sideToMove();

	int source = move.sourceSquare();
	Square srcSq = chessSquare(source);
	int target = move.targetSquare();
	int baserank = (side == Side::White) ? 0 : height() - 1;

	// promotion on own baserank is channeling move
	if (srcSq.rank() == baserank && promotion != Piece::NoPiece)
	{
		// use notation symbol "/" (slash) for channeling move
		str.replace('=','/');
		// castling moves
		if (!str.contains('/'))
		{
			// internal representation of rook square channeling
			if (promotion >= rookSquareChanneling(Hawk))
				pieceType = rookSquareChanneling(promotion, backward);

			Piece piece(side, pieceType);
			str.append('/' + pieceSymbol(piece).toUpper());
			if (promotion == pieceType)
				str.append(squareString(source));
			else
				str.append(squareString(target));
		}
	}
	return str;
}

/*
 * This method generates a board move from SAN (see above). It accepts
 * castling/channeling notation formats like O-O-O/Hh1 and O-O-O/Hh.
 * Internally a channeling move is represented as move with promotion.
 * Castling moves with channeling onto rook squares use auxiliary promotion
 * piece types to differ from drops onto king squares.
 */
Move SeirawanBoard::moveFromSanString(const QString& str)
{
	// leave orthodox moves to WesternBoard
	int index = str.indexOf('/');
	if (index < 0)
		return WesternBoard::moveFromSanString(str);

	// channeling moves
	QString s = str.left(index);
	Move move = WesternBoard::moveFromSanString(s);
	int promotion = pieceFromSymbol(str.mid(index + 1, 1)).type();

	if (move.isNull() || promotion == 0)
		return Move();

	Piece piece = Piece(sideToMove(), promotion);
	if (!reservePieceTypes().contains(piece) ||  reserveCount(piece) < 1)
		return Move();

	int target = move.targetSquare();

	// castling with channeling onto rook square
	if (move.sourceSquare() == kingSquare(sideToMove())
	&&  str.mid(index + 2, 1) == squareString(target).left(1))
		promotion = rookSquareChanneling(promotion);

	// channeling moves
	return Move(move.sourceSquare(), target, promotion );
}

Move SeirawanBoard::moveFromLanString(const QString& str)
{
	Move move0(Board::moveFromLanString(str));
	Side side = sideToMove();
	int source = move0.sourceSquare();
	int target = move0.targetSquare();
	int promotion = move0.promotion();

	// castling move with channeling onto rook square
	// coded as castling with promotion to auxiliary type
	if (promotion != Piece::NoPiece
	&&  pieceAt(source) == Piece(side, Rook)
	&&  target == kingSquare(side))
	{
		return Move(target, source, rookSquareChanneling(promotion));
	}

	Move move(WesternBoard::moveFromLanString(str));
	return Move(move.sourceSquare(), move.targetSquare(), promotion);
}

void SeirawanBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	int source = move.sourceSquare();
	int prom = move.promotion();
	Side side = sideToMove();

	WesternBoard::vMakeMove(move, transition);

	if (m_squareMap.contains(source)
	&&  m_squareMap[source] == 0
	&&  prom != Piece::NoPiece)
	{
		if (prom >= rookSquareChanneling(Hawk))
		{
			prom = rookSquareChanneling(prom, backward);
			source = move.targetSquare();
		}
		Piece piece(side, prom);
		if (reserveCount(piece) > 0)
		{
			Square csq = chessSquare(source);

			if (csq.isValid())
			{
				removeFromReserve(piece);
				setSquare(source, piece); // drop-in
				if (transition != nullptr)
					transition->addDrop(piece, csq);
			}
		}
	}
	updateSquareMap(move, +1);
}

void SeirawanBoard::vUndoMove(const Move& move)
{
	Side side = sideToMove();
	int index = move.sourceSquare();

	if (move.promotion() >= rookSquareChanneling(Hawk))
		  index = move.targetSquare();

	Piece piece = pieceAt(index);

	updateSquareMap(move, -1);

	// channeling move?
	if ( piece.side() == side
	&&   chessSquare(index).isValid()
	&&   reservePieceTypes().contains(piece))
	{
		// undo channeling move
		addToReserve(piece);
		// skipped: setSquare(index, Piece());
	}
	WesternBoard::vUndoMove(move);
}

void SeirawanBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	if (!isValidSquare(chessSquare(square)))
		return;

	QVarLengthArray< Move > moves1, moves2;
	WesternBoard::generateMovesForPiece(moves1, pieceType, square);

	// add normal moves
	for (Move m: moves1)
		moves.append(m);

	// return if no channeling is allowed on square
	if (!m_squareMap.contains(square) ||  m_squareMap[square] > 0)
		return;

	// generate channeling moves as promotions on base rank
	Side side = sideToMove();
	for (Move m: moves1)
	{
		const QList<Piece>& reserveTypes = reservePieceTypes();
		for (const Piece& pc: reserveTypes)
		{
			if (pc.side() == side &&  reserveCount(pc) > 0)
				moves2.append(Move(m.sourceSquare(),
						   m.targetSquare(),
						   pc.type()));
		}
	}

	// add channeling moves
	for (Move m: moves2)
	{
		moves.append(m);
		// add castling move with channeling onto rook square
		Piece piece1 = pieceAt(m.sourceSquare());
		Piece piece2 = pieceAt(m.targetSquare());

		if (piece1.type() == King
		&&  piece2.type() == Rook
		&&  piece2.side() == side)
			moves.append(Move(m.sourceSquare(),
					  m.targetSquare(),
					  rookSquareChanneling(m.promotion())));
	}
}

} // namespace Chess

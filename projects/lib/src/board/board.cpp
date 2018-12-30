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

#include "board.h"
#include <QStringList>
#include "zobrist.h"


namespace Chess {

QDebug operator<<(QDebug dbg, const Board* board)
{
	QString str = "FEN: " + board->fenString() + '\n';
	str += Board::tr("Zobrist key") + ": 0x" +
	       QString::number(board->m_key, 16).toUpper() + '\n';

	int i = (board->m_width + 2) * 2;
	for (int y = 0; y < board->m_height; y++)
	{
		i++;
		for (int x = 0; x < board->m_width; x++)
		{
			Piece pc = board->m_squares[i];
			if (pc.isValid())
				str += board->pieceSymbol(pc);
			else
				str += ".";
			str += ' ';
			i++;
		}
		i++;
		str += '\n';
	}

	dbg.nospace() << str;
	return dbg.space();
}


Board::Board(Zobrist* zobrist)
	: m_initialized(false),
	  m_width(0),
	  m_height(0),
	  m_side(Side::White),
	  m_startingSide(Side::White),
	  m_maxPieceSymbolLength(1),
	  m_key(0),
	  m_zobrist(zobrist),
	  m_sharedZobrist(zobrist)
{
	Q_ASSERT(zobrist != nullptr);

	setPieceType(Piece::NoPiece, QString(), QString());
}

Board::~Board()
{
}

bool Board::isRandomVariant() const
{
	return false;
}

bool Board::variantHasDrops() const
{
	return false;
}

bool Board::variantHasOptionalPromotions() const
{
	return false;
}

bool Board::variantHasWallSquares() const
{
	return false;
}

QList<Piece> Board::reservePieceTypes() const
{
	return QList<Piece>();
}

Board::CoordinateSystem Board::coordinateSystem() const
{
	return NormalCoordinates;
}

Side Board::upperCaseSide() const
{
	return Side::White;
}

Piece Board::pieceAt(const Square& square) const
{
	if (!isValidSquare(square))
		return Piece::WallPiece;
	return pieceAt(squareIndex(square));
}

void Board::initialize()
{
	if (m_initialized)
		return;

	m_initialized = true;
	m_width = width();
	m_height = height();
	for (int i = 0; i < (m_width + 2) * (m_height + 4); i++)
		m_squares.append(Piece::WallPiece);
	vInitialize();

	m_maxPieceSymbolLength = 1;
	for (const PieceData& pd: m_pieceData)
		if (pd.symbol.length() > m_maxPieceSymbolLength)
			m_maxPieceSymbolLength = pd.symbol.length();

	m_zobrist->initialize((m_width + 2) * (m_height + 4), m_pieceData.size());
}

int Board::maxPieceSymbolLength() const
{
	return m_maxPieceSymbolLength;
}

void Board::setPieceType(int type,
			 const QString& name,
			 const QString& symbol,
			 unsigned movement,
			 const QString& gsymbol)
{
	if (type >= m_pieceData.size())
		m_pieceData.resize(type + 1);

	const QString& graphicalSymbol = gsymbol.isEmpty() ? symbol : gsymbol;

	PieceData data =
		{ name, symbol.toUpper(), movement, graphicalSymbol.toUpper() };
	m_pieceData[type] = data;
}

QString Board::pieceSymbol(Piece piece) const
{
	int type = piece.type();
	if (type <= 0 || type >= m_pieceData.size())
		return QString();

	if (piece.side() == upperCaseSide())
		return m_pieceData[type].symbol;
	return m_pieceData[type].symbol.toLower();
}

Piece Board::pieceFromSymbol(const QString& pieceSymbol) const
{
	if (pieceSymbol.isEmpty())
		return Piece::NoPiece;

	int code = Piece::NoPiece;
	QString symbol = pieceSymbol.toUpper();

	for (int i = 1; i < m_pieceData.size(); i++)
	{
		if (symbol == m_pieceData[i].symbol)
		{
			code = i;
			break;
		}
	}
	if (code == Piece::NoPiece)
		return code;

	Side side(upperCaseSide());
	if (pieceSymbol == symbol)
		return Piece(side, code);
	return Piece(side.opposite(), code);
}

QString Board::pieceString(int pieceType) const
{
	if (pieceType <= 0 || pieceType >= m_pieceData.size())
		return QString();
	return m_pieceData[pieceType].name;
}

QString Board::representation(Piece piece) const
{
	int type = piece.type();
	if (type <= 0 || type >= m_pieceData.size())
		return QString();

	if (piece.side() == upperCaseSide())
		return m_pieceData[type].representation;
	return m_pieceData[type].representation.toLower();
}

int Board::reserveType(int pieceType) const
{
	return pieceType;
}

int Board::reserveCount(Piece piece) const
{
	if (!piece.isValid()
	||  piece.type() >= m_reserve[piece.side()].size())
		return 0;
	return m_reserve[piece.side()].at(piece.type());
}

void Board::addToReserve(const Piece& piece, int count)
{
	Q_ASSERT(piece.isValid());
	Q_ASSERT(count > 0);

	Side side(piece.side());
	int type(piece.type());
	if (type >= m_reserve[side].size())
		m_reserve[side].resize(type + 1);

	int& oldCount = m_reserve[side][type];
	for (int i = 1; i <= count; i++)
		xorKey(m_zobrist->reservePiece(piece, oldCount++));
}

void Board::removeFromReserve(const Piece& piece)
{
	Q_ASSERT(piece.isValid());
	Q_ASSERT(piece.type() < m_reserve[piece.side()].size());

	int& count = m_reserve[piece.side()][piece.type()];
	xorKey(m_zobrist->reservePiece(piece, --count));
}

Square Board::chessSquare(int index) const
{
	int arwidth = m_width + 2;
	int file = (index % arwidth) - 1;
	int rank = (m_height - 1) - ((index / arwidth) - 2);
	return Square(file, rank);
}

int Board::squareIndex(const Square& square) const
{
	if (!isValidSquare(square))
		return 0;

	int rank = (m_height - 1) - square.rank();
	return (rank + 2) * (m_width + 2) + 1 + square.file();
}

bool Board::isValidSquare(const Chess::Square& square) const
{
	if (!square.isValid()
	||  square.file() >= m_width || square.rank() >= m_height)
		return false;
	return true;
}

QString Board::squareString(int index) const
{
	return squareString(chessSquare(index));
}

QString Board::squareString(const Square& square) const
{
	if (!square.isValid())
		return QString();

	QString str;

	if (coordinateSystem() == NormalCoordinates)
	{
		str += QChar('a' + square.file());
		str += QString::number(square.rank() + 1);
	}
	else
	{
		str += QString::number(m_width - square.file());
		str += QChar('a' + (m_height - square.rank()) - 1);
	}

	return str;
}

Square Board::chessSquare(const QString& str) const
{
	if (str.length() < 2)
		return Square();

	bool ok = false;
	int file = 0;
	int rank = 0;

	if (coordinateSystem() == NormalCoordinates)
	{
		file = str.at(0).toLatin1() - 'a';
		rank = str.midRef(1).toInt(&ok) - 1;
	}
	else
	{
		int tmp = str.length() - 1;
		file = m_width - str.leftRef(tmp).toInt(&ok);
		rank = m_height - (str.at(tmp).toLatin1() - 'a') - 1;
	}

	if (!ok)
		return Square();
	return Square(file, rank);
}

int Board::squareIndex(const QString& str) const
{
	return squareIndex(chessSquare(str));
}

QString Board::lanMoveString(const Move& move)
{
	QString str;

	// Piece drop
	if (move.sourceSquare() == 0)
	{
		Q_ASSERT(move.promotion() != Piece::NoPiece);
		str += pieceSymbol(move.promotion()).toUpper() + '@';
		str += squareString(move.targetSquare());
		return str;
	}

	str += squareString(move.sourceSquare());
	str += squareString(move.targetSquare());
	if (move.promotion() != Piece::NoPiece)
		str += pieceSymbol(move.promotion()).toLower();

	return str;
}

QString Board::moveString(const Move& move, MoveNotation notation)
{
	if (notation == StandardAlgebraic)
		return sanMoveString(move);
	return lanMoveString(move);
}

Move Board::moveFromLanString(const QString& istr)
{
	QString str(istr);
	str.remove(QRegExp("[x=+#!?]"));
	int len = str.length();
	if (len < 4)
		return Move();

	Piece promotion;
	int drop = str.indexOf('@');
	if (drop > 0)
	{
		promotion = pieceFromSymbol(str.left(drop));
		if (!promotion.isValid())
			return Move();
		
		Square trg(chessSquare(str.mid(drop + 1)));
		if (!isValidSquare(trg))
			return Move();
		
		return Move(0, squareIndex(trg), promotion.type());
	}

	if (len > 4)
		promotion = pieceFromSymbol(str.mid(len - 1));

	if (promotion.isValid())
		len = len - 1;

	for (int i = 2; i < len - 1; i++)
	{
		Square sourceSq(chessSquare(str.mid(0, i)));
		Square targetSq(chessSquare(str.mid(i, len - i)));
		if (!isValidSquare(sourceSq) || !isValidSquare(targetSq))
			continue;
		int source = squareIndex(sourceSq);
		int target = squareIndex(targetSq);

		return Move(source, target, promotion.type());
	}
	return Move();

}

Move Board::moveFromString(const QString& str)
{
	Move move = moveFromSanString(str);
	if (move.isNull())
	{
		move = moveFromLanString(str);
		if (!isLegalMove(move))
			return Move();
	}
	return move;
}

Move Board::moveFromGenericMove(const GenericMove& move) const
{
	int source = squareIndex(move.sourceSquare());
	int target = squareIndex(move.targetSquare());

	return Move(source, target, move.promotion());
}

GenericMove Board::genericMove(const Move& move) const
{
	int source = move.sourceSquare();
	int target = move.targetSquare();

	return GenericMove(chessSquare(source),
			   chessSquare(target),
			   move.promotion());
}

QStringList Board::pieceList(Side side) const
{
	QStringList list;
	for (int file = 0; file < height(); file++)
		for (int rank = 0; rank < width(); rank++)
		{
			Square sq = Chess::Square(file, rank);
			const Piece piece = pieceAt(sq);
			if (piece.side() != side)
				continue;

			QString s = pieceSymbol(piece).toUpper();
			s.append(squareString(sq));
			list.append(s);
		}
	return list;
}

QString Board::fenString(FenNotation notation) const
{
	QString fen;

	// Squares
	int i = (m_width + 2) * 2;
	for (int y = 0; y < m_height; y++)
	{
		int nempty = 0;
		i++;
		if (y > 0)
			fen += '/';
		for (int x = 0; x < m_width; x++)
		{
			Piece pc = m_squares[i];

			if (pc.isEmpty())
				nempty++;

			// Add the number of empty successive squares
			// to the FEN string.
			if (nempty > 0
			&&  (!pc.isEmpty() || x == m_width - 1))
			{
				fen += QString::number(nempty);
				nempty = 0;
			}

			if (pc.isValid())
				fen += pieceSymbol(pc);
			else if (pc.isWall())
				fen += "*";

			i++;
		}
		i++;
	}

	// Hand pieces
	if (variantHasDrops())
	{
		QString str;
		for (i = Side::White; i <= Side::Black; i++)
		{
			Side side = Side::Type(i);
			for (int j = m_reserve[i].size() - 1; j >= 1; j--)
			{
				int count = m_reserve[i].at(j);
				for (int k = 0; k < count; k++)
					str += pieceSymbol(Piece(side, j));
			}
		}
		if (str.isEmpty())
			str = "-";
		fen += QString("[%1]").arg(str);
	}

	// Side to move
	fen += QString(" %1 ").arg(m_side.symbol());

	return fen + vFenString(notation);
}

bool Board::setFenString(const QString& fen)
{
	QStringList strList = fen.split(' ');
	if (strList.isEmpty())
		return false;

	QStringList::iterator token = strList.begin();
	if (token->length() < m_height * 2)
		return false;

	initialize();

	int square = 0;
	int rankEndSquare = 0;	// last square of the previous rank
	int boardSize = m_width * m_height;
	int k = (m_width + 2) * 2 + 1;

	for (int i = 0; i < m_squares.size(); i++)
		m_squares[i] = Piece::WallPiece;
	m_key = 0;

	// Get the board contents (squares)
	int handPieceIndex = -1;
	int maxsymlen = maxPieceSymbolLength();
	QString pieceStr;
	for (int i = 0; i < token->length(); i++)
	{
		QChar c = token->at(i);

		// Move to the next rank
		if (c == '/')
		{
			if (!pieceStr.isEmpty())
				return false;

			// Reject the FEN string if the rank didn't
			// have exactly 'm_width' squares.
			if (square - rankEndSquare != m_width)
				return false;
			rankEndSquare = square;
			k += 2;
			continue;
		}
		// Start of hand pieces
		if (c == '[')
		{
			if (!variantHasDrops())
				return false;
			handPieceIndex = i + 1;
			break;
		}
		// Wall square
		if (c == '*' && variantHasWallSquares())
		{
			if (!pieceStr.isEmpty())
				return false;
			square++;
			k++;
			continue;
		}
		// Add empty squares
		if (c.isDigit())
		{
			if (!pieceStr.isEmpty())
				return false;

			int j;
			int nempty;
			if (i < (token->length() - 1) && token->at(i + 1).isDigit())
			{
				nempty = token->midRef(i, 2).toInt();
				i++;
			}
			else
				nempty = c.digitValue();

			if (nempty > m_width || square + nempty > boardSize)
				return false;
			for (j = 0; j < nempty; j++)
			{
				square++;
				setSquare(k++, Piece::NoPiece);
			}
			continue;
		}

		if (square >= boardSize)
			return false;

		// read ahead for multi-character symbols
		for (int l = qMin(maxsymlen, token->length() - i); l > 0; l--)
		{
			pieceStr = token->mid(i, l);
			Piece piece = pieceFromSymbol(pieceStr);
			if (piece.isValid())
			{
				setSquare(k++, piece);
				i += l - 1;
				pieceStr.clear();
				square++;
				break;
			}
		}
		// left over: unknown symbols
		if (!pieceStr.isEmpty())
			return false;
	}

	// The board must have exactly 'boardSize' squares and each rank
	// must have exactly 'm_width' squares.
	if (square != boardSize || square - rankEndSquare != m_width)
		return false;

	// Hand pieces
	m_reserve[Side::White].clear();
	m_reserve[Side::Black].clear();
	if (handPieceIndex != -1)
	{
		for (int i = handPieceIndex; i < token->length(); i++)
		{
			QChar c = token->at(i);
			if (c == ']')
				break;
			if (c == '-' && i == handPieceIndex)
				continue;

			int count = 1;
			if (c.isDigit())
			{
				count = c.digitValue();
				if (count <= 0)
					return false;
				++i;
				if (i >= token->length() - 1)
					return false;
				c = token->at(i);
			}
			Piece tmp = pieceFromSymbol(c);
			if (!tmp.isValid())
				return false;
			addToReserve(tmp, count);
		}
	}

	// Side to move
	if (++token == strList.end())
		return false;
	m_side = Side(*token);
	m_startingSide = m_side;
	if (m_side.isNull())
		return false;

	m_moveHistory.clear();
	m_startingFen = fen;

	// Let subclasses handle the rest of the FEN string
	if (token != strList.end())
		++token;
	strList.erase(strList.begin(), token);
	if (!vSetFenString(strList))
		return false;

	if (m_side == Side::White)
		xorKey(m_zobrist->side());

	if (!isLegalPosition())
		return false;

	return true;
}

void Board::reset()
{
	setFenString(defaultFenString());
}

void Board::makeMove(const Move& move, BoardTransition* transition)
{
	Q_ASSERT(!m_side.isNull());
	Q_ASSERT(!move.isNull());

	MoveData md = { move, m_key };

	vMakeMove(move, transition);

	xorKey(m_zobrist->side());
	m_side = m_side.opposite();
	m_moveHistory << md;
}

void Board::undoMove()
{
	Q_ASSERT(!m_moveHistory.isEmpty());
	Q_ASSERT(!m_side.isNull());

	m_side = m_side.opposite();
	vUndoMove(m_moveHistory.last().move);

	m_key = m_moveHistory.last().key;
	m_moveHistory.pop_back();
}

void Board::generateMoves(QVarLengthArray<Move>& moves, int pieceType) const
{
	Q_ASSERT(!m_side.isNull());

	// Cut the wall squares (the ones with a value of WallPiece) off
	// from the squares to iterate over. It bumps the speed up a bit.
	unsigned begin = (m_width + 2) * 2;
	unsigned end = m_squares.size() - begin;

	moves.clear();
	for (unsigned sq = begin; sq < end; sq++)
	{
		Piece tmp = m_squares[sq];
		if (tmp.side() == m_side
		&&  (pieceType == Piece::NoPiece || tmp.type() == pieceType))
			generateMovesForPiece(moves, tmp.type(), sq);
	}

	generateDropMoves(moves, pieceType);
}

void Board::generateDropMoves(QVarLengthArray<Move>& moves, int pieceType) const
{
	const QVector<int>& pieces(m_reserve[m_side]);
	if (pieces.isEmpty())
		return;

	if (pieceType == Piece::NoPiece)
	{
		for (int i = 1; i < pieces.size(); i++)
		{
			Q_ASSERT(pieces.at(i) >= 0);
			if (pieces.at(i) > 0)
				generateMovesForPiece(moves, i, 0);
		}
	}
	else if (pieceType < pieces.size() && pieces.at(pieceType) > 0)
		generateMovesForPiece(moves, pieceType, 0);
}

void Board::generateHoppingMoves(int sourceSquare,
				 const QVarLengthArray<int>& offsets,
				 QVarLengthArray<Move>& moves) const
{
	Side opSide = sideToMove().opposite();
	for (int i = 0; i < offsets.size(); i++)
	{
		int targetSquare = sourceSquare + offsets[i];
		if (!isValidSquare(chessSquare(targetSquare)))
			continue;
		Piece capture = pieceAt(targetSquare);
		if (capture.isEmpty() || capture.side() == opSide)
			moves.append(Move(sourceSquare, targetSquare));
	}
}

void Board::generateSlidingMoves(int sourceSquare,
				 const QVarLengthArray<int>& offsets,
				 QVarLengthArray<Move>& moves) const
{
	Side side = sideToMove();
	for (int i = 0; i < offsets.size(); i++)
	{
		int offset = offsets[i];
		int targetSquare = sourceSquare + offset;
		Piece capture;
		while (!(capture = pieceAt(targetSquare)).isWall()
		&&      capture.side() != side)
		{
			moves.append(Move(sourceSquare, targetSquare));
			if (!capture.isEmpty())
				break;
			targetSquare += offset;
		}
	}
}

bool Board::moveExists(const Move& move) const
{
	Q_ASSERT(!move.isNull());

	int source = move.sourceSquare();
	QVarLengthArray<Move> moves;

	if (source == 0)
		generateDropMoves(moves, move.promotion());
	else
	{
		Piece piece = m_squares[source];
		if (piece.side() != m_side)
			return false;
		generateMovesForPiece(moves, piece.type(), source);
	}

	for (int i = 0; i < moves.size(); i++)
	{
		if (moves[i] == move)
			return true;
	}
	return false;
}

int Board::captureType(const Move& move) const
{
	Q_ASSERT(!move.isNull());

	Piece piece(m_squares[move.targetSquare()]);
	if (piece.side() == m_side.opposite())
		return piece.type();
	return Piece::NoPiece;
}

bool Board::vIsLegalMove(const Move& move)
{
	Q_ASSERT(!move.isNull());

	makeMove(move);
	bool isLegal = isLegalPosition();
	undoMove();

	return isLegal;
}

bool Board::isLegalMove(const Move& move)
{
	return !move.isNull() && moveExists(move) && vIsLegalMove(move);
}

int Board::repeatCount() const
{
	if (plyCount() < 4)
		return 0;

	int repeatCount = 0;
	for (int i = plyCount() - 1; i >= 0; i--)
	{
		if (m_moveHistory.at(i).key == m_key)
			repeatCount++;
	}

	return repeatCount;
}

int Board::reversibleMoveCount() const
{
	return -1;
}

bool Board::isRepetition(const Chess::Move& move)
{
	Q_ASSERT(!move.isNull());

	makeMove(move);
	bool isRepeat = (repeatCount() > 0);
	undoMove();

	return isRepeat;
}

bool Board::canMove()
{
	QVarLengthArray<Move> moves;
	generateMoves(moves);

	for (int i = 0; i < moves.size(); i++)
	{
		if (vIsLegalMove(moves[i]))
			return true;
	}

	return false;
}

QVector<Move> Board::legalMoves()
{
	QVarLengthArray<Move> moves;
	QVector<Move> legalMoves;

	generateMoves(moves);
	legalMoves.reserve(moves.size());

	for (int i = moves.size() - 1; i >= 0; i--)
	{
		if (vIsLegalMove(moves[i]))
			legalMoves << moves[i];
	}

	return legalMoves;
}

Result Board::tablebaseResult(unsigned int* dtm) const
{
	Q_UNUSED(dtm);
	return Result();
}

} // namespace Chess

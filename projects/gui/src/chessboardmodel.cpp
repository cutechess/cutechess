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

#include "chessboardmodel.h"
#include <board/board.h>
#include "squareinfo.h"


ChessboardModel::ChessboardModel(QObject* parent)
	: QAbstractTableModel(parent),
	  m_board(0),
	  m_width(8),
	  m_height(8),
	  m_widthOffset(0)
{
}

Qt::ItemFlags ChessboardModel::flags(const QModelIndex& index) const
{
	if (m_selectable.contains(index))
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::NoItemFlags;
}

void ChessboardModel::setBoard(Chess::Board* board)
{
	Q_ASSERT(board != 0);

	if (m_board != 0)
	{
		disconnect(m_board);
		m_board->disconnect(this);
	}

	m_board = board;
	m_move = Chess::GenericMove();
	m_width = m_board->width();
	m_height = m_board->height();
	m_widthOffset = 0;

	// If the variant has piece drops, add additional
	// columns for piece holdings
	if (m_board->variantHasDrops())
		m_widthOffset = 1;
	m_width += m_widthOffset * 2;

	connect(m_board, SIGNAL(boardReset()), this, SLOT(boardReset()));
	connect(m_board, SIGNAL(squareChanged(const Chess::Square&)),
		this, SLOT(onSquareChanged(const Chess::Square&)));
	connect(m_board, SIGNAL(handPieceChanged(Chess::Piece)),
		this, SLOT(onHandPieceChanged(Chess::Piece)));
	connect(m_board, SIGNAL(moveMade(const Chess::GenericMove&)),
		this, SLOT(onMoveMade(const Chess::GenericMove&)));

	reset();
}

int ChessboardModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_height;
}

int ChessboardModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_width;
}

QVariant ChessboardModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || m_board == 0)
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		SquareInfo squareInfo;
		Chess::Square sq(indexToSquare(index));
		Chess::Piece piece;

		if (m_board->isValidSquare(sq))
		{
			if ((sq.file() % 2) == (sq.rank() % 2))
				squareInfo.setColor(SquareInfo::DarkColor);
			else
				squareInfo.setColor(SquareInfo::LightColor);

			if ((piece = m_board->pieceAt(sq)).isValid())
				squareInfo.setPieceCount(1);

			if (m_move.sourceSquare() == sq)
				squareInfo.setType(SquareInfo::SourceSquare);
			else if (m_move.targetSquare() == sq)
				squareInfo.setType(SquareInfo::TargetSquare);
		}
		else if ((piece = indexToHandPiece(index)).isValid())
		{
			squareInfo.setPieceCount(m_board->handPieceCount(piece));
			squareInfo.setColor(SquareInfo::HoldingsColor);
		}

		squareInfo.setPieceSymbol(m_board->pieceSymbol(piece));
		return QVariant::fromValue(squareInfo);
	}

	return QVariant();
}

QVariant ChessboardModel::headerData(int section,
                                Qt::Orientation orientation,
                                int role) const
{
	if (!m_board || role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal && m_widthOffset != 0)
	{
		if (section < m_widthOffset
		||  section >= m_width - m_widthOffset)
			return QVariant();
		section -= m_widthOffset;
	}

	if (m_board->coordinateSystem() == Chess::Board::NormalCoordinates)
	{
		if (orientation == Qt::Vertical)
			return QString::number(m_height - section);
		return QChar('a' + section);
	}
	if (orientation == Qt::Vertical)
		return QChar('a' + section);
	return QString::number(m_width - section);
}

QModelIndex ChessboardModel::squareToIndex(const Chess::Square& square) const
{
	if (!square.isValid())
		return QModelIndex();

	int row = (m_height - 1) - square.rank();
	int column = square.file() + m_widthOffset;
	return createIndex(row, column);
}

Chess::Square ChessboardModel::indexToSquare(const QModelIndex& index) const
{
	if (!index.isValid())
		return Chess::Square();

	Chess::Square square;
	square.setFile(index.column() - m_widthOffset);
	square.setRank(m_height - index.row() - 1);

	return square;
}

QModelIndex ChessboardModel::handPieceToIndex(Chess::Piece piece) const
{
	Q_ASSERT(m_board != 0);

	if (!piece.isValid() || m_widthOffset == 0)
		return QModelIndex();

	int col = 0;
	int row = piece.type() - 1;
	if (piece.side() == m_board->upperCaseSide())
	{
		col = m_width - 1;
		row = m_height - piece.type();
	}

	return createIndex(row, col);
}

Chess::Piece ChessboardModel::indexToHandPiece(const QModelIndex& index) const
{
	Q_ASSERT(index.isValid());
	Q_ASSERT(m_board != 0);

	if (m_widthOffset == 0 || index.column() >= m_width)
		return Chess::Piece::NoPiece;

	Chess::Side side;
	int col = index.column();
	int pieceType = index.row() + 1;

	if (col == 0)
		side = m_board->upperCaseSide().opposite();
	else if (col == m_width - 1)
	{
		side = m_board->upperCaseSide();
		pieceType = m_height - index.row();
	}
	else
		return Chess::Piece::NoPiece;

	return Chess::Piece(side, pieceType);
}

void ChessboardModel::updateSelectable()
{
	Q_ASSERT(m_board != 0);

	m_selectable.clear();
	Chess::Side side(m_board->sideToMove());

	QVector<Chess::Move> moves(m_board->legalMoves());
	foreach (const Chess::Move& move, moves)
	{
		QModelIndex index;
		
		if (move.sourceSquare() == 0) // piece drop
		{
			Chess::Piece piece(side, move.promotion());
			index = handPieceToIndex(piece);
		}
		else // normal move
		{
			Chess::GenericMove tmp(m_board->genericMove(move));
			index = squareToIndex(tmp.sourceSquare());
		}

		if (!m_selectable.contains(index))
		{
			m_selectable.append(index);
			emit dataChanged(index, index);
		}
	}
}

void ChessboardModel::onHumanMove(const QModelIndex& source, const QModelIndex& target)
{
	Chess::Side side(m_board->sideToMove());
	Chess::GenericMove move;
	move.setTargetSquare(indexToSquare(target));

	// Piece drop
	Chess::Piece piece(indexToHandPiece(source));
	if (piece.isValid())
	{
		move.setPromotion(piece.type());
		emit humanMove(m_board->moveFromGenericMove(move), side);
		return;
	}

	move.setSourceSquare(indexToSquare(source));
	Chess::Move boardMove = m_board->moveFromGenericMove(move);

	QVector<Chess::Move> moves(m_board->legalMoves());
	QList<int> promotions;
	foreach (const Chess::Move& tmp, moves)
	{
		if (tmp.sourceSquare() == boardMove.sourceSquare()
		&&  tmp.targetSquare() == boardMove.targetSquare())
		{
			int prom = tmp.promotion();

			// Make sure "No promotion" is at the top
			if (prom == Chess::Piece::NoPiece)
				promotions.prepend(prom);
			else
				promotions.append(prom);
		}
	}

	// Emit the move if there's only one match
	if (promotions.size() == 1)
	{
		boardMove = Chess::Move(boardMove.sourceSquare(),
					boardMove.targetSquare(),
					promotions.first());
		emit humanMove(boardMove, side);
	}
	// If there are multiple promotion possibilities, the user
	// has to select a promotion piece.
	else if (promotions.size() > 1)
		emit promotionNeeded(m_board, boardMove, promotions);
}

void ChessboardModel::onMoveMade(const Chess::GenericMove& move)
{
	QModelIndex index(squareToIndex(m_move.sourceSquare()));
	if (index.isValid())
		emit dataChanged(index, index);
	index = squareToIndex(m_move.targetSquare());
	if (index.isValid())
		emit dataChanged(index, index);

	m_move = move;
	updateSelectable();
}

void ChessboardModel::onSquareChanged(const Chess::Square& square)
{
	QModelIndex index(squareToIndex(square));
	emit dataChanged(index, index);
}

void ChessboardModel::onHandPieceChanged(Chess::Piece piece)
{
	QModelIndex index(handPieceToIndex(piece));
	emit dataChanged(index, index);
}

void ChessboardModel::boardReset()
{
	m_move = Chess::GenericMove();
	updateSelectable();
	reset();
}

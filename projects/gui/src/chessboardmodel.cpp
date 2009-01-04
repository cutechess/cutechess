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
#include <chessboard/chessboard.h>

ChessboardModel::ChessboardModel(QObject* parent)
	: QAbstractTableModel(parent),
	  m_board(0)
{
}

void ChessboardModel::setBoard(Chess::Board* board)
{
	Q_ASSERT(board != 0);

	if (m_board != 0)
		disconnect(m_board);

	m_board = board;
	connect(m_board, SIGNAL(boardReset()), this, SLOT(boardReset()));
	connect(m_board, SIGNAL(squareChanged(const Chess::Square&)),
	        this, SLOT(squareChanged(const Chess::Square&)));
}

int ChessboardModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (m_board == 0)
		return 8;

	return m_board->height();
}

int ChessboardModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (m_board == 0)
		return 8;

	return m_board->width();
}

QVariant ChessboardModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (m_board == 0)
		return QString();

	if (role == Qt::DisplayRole)
	{
		int file = index.column();
		int rank = (rowCount(QModelIndex()) - 1) - index.row();
		Chess::Square sq = { file, rank };
		int piece = m_board->pieceAt(sq);

		if (piece == Chess::NoPiece)
			return QString();

		if (piece == Chess::InvalidPiece)
			return QVariant();

		return piece;
	}

	return QVariant();
}

QVariant ChessboardModel::headerData(int section,
                                Qt::Orientation orientation,
                                int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Vertical)
		{
			return QString::number(rowCount(QModelIndex()) - section);
		}
		else
		{
			QString files = "abcdefghij";
			return files.at(section);
		}
	}

	return QVariant();
}

void ChessboardModel::squareChanged(const Chess::Square& square)
{
	int row = (rowCount(QModelIndex()) - 1) - square.rank;
	int column = square.file;
	QModelIndex index = createIndex(row, column);

	emit dataChanged(index, index);
}

void ChessboardModel::boardReset()
{
	reset();
}


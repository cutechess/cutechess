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

#ifndef CHESSBOARD_MODEL_H
#define CHESSBOARD_MODEL_H

#include <QAbstractTableModel>
#include <board/piece.h>

namespace Chess { class Board; class Square; class GenericMove; }

class ChessboardModel : public QAbstractTableModel
{
	Q_OBJECT

	public:

		ChessboardModel(QObject* parent = 0);
		void setBoard(Chess::Board* board);

		// Inherited from QAbstractTableModel
		Qt::ItemFlags flags(const QModelIndex& index) const;
		int rowCount(const QModelIndex& parent) const;
		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& parent, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;

	signals:
		void moveMade(const QModelIndex& source, const QModelIndex& target);
		void humanMove(const Chess::GenericMove& move);

	public slots:
		void onSquareChanged(const Chess::Square& square);
		void onHandPieceChanged(Chess::Piece piece);
		void boardReset();
		void onHumanMove(const QModelIndex& source, const QModelIndex& target);

	private slots:
		void onMoveMade(const Chess::GenericMove& move);

	private:
		QModelIndex squareToIndex(const Chess::Square& square) const;
		Chess::Square indexToSquare(const QModelIndex& index) const;
		QModelIndex handPieceToIndex(Chess::Piece piece) const;
		Chess::Piece indexToHandPiece(const QModelIndex& index) const;
		void updateSelectable();

		Chess::Board* m_board;
		int m_width;
		int m_height;
		int m_widthOffset;
		QModelIndexList m_selectable;
		static const QString m_files;
};

#endif // CHESSBOARD_MODEL_H

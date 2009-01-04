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

namespace Chess { class Board; class Square; }

class ChessboardModel : public QAbstractTableModel
{
	Q_OBJECT

	public:

		ChessboardModel(QObject* parent = 0);
		void setBoard(Chess::Board* board);
		int rowCount(const QModelIndex& parent) const;
		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& parent, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;

	public slots:
		void squareChanged(const Chess::Square& square);
		void boardReset();

	private:
		Chess::Board* m_board;
};

#endif // CHESSBOARD_MODEL_H


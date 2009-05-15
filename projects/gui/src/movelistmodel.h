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

#ifndef MOVE_LIST_MODEL_H
#define MOVE_LIST_MODEL_H

#include <QAbstractItemModel>

class ChessGame;

class MoveListModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		MoveListModel(QObject* parent = 0);
		void setGame(ChessGame* game);

		// Inherited from QAbstractItemModel
		QModelIndex index(int row, int column,
		                  const QModelIndex& parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex& index) const;
		bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;
	private slots:
		void boardReset();
		void onMoveMade();

	private:
		static const QStringList m_headers;
		ChessGame* m_game;

};

#endif // MOVE_LIST_MODEL_H


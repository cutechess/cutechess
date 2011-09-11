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
#include <QStringList>
#include <QPair>

class ChessGame;
namespace Chess { class GenericMove; }

/*!
 * \brief Supplies chess move information to views.
 */
class MoveListModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		/*! Constructs a move list model with the given \a parent. */
		MoveListModel(QObject* parent = 0);
		/*! Associates \a game with this model. */
		void setGame(ChessGame* game);

		// Inherited from QAbstractItemModel
		virtual QModelIndex index(int row, int column,
					  const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex parent(const QModelIndex& index) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation,
					    int role = Qt::DisplayRole) const;
	private slots:
		void onMoveMade(const Chess::GenericMove& move,
				const QString& sanString,
				const QString& comment);

	private:
		static const QStringList m_headers;
		ChessGame* m_game;
		QList< QPair<QString, QString> > m_moveList;
};

#endif // MOVE_LIST_MODEL_H

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

#ifndef EVALWIDGET_H
#define EVALWIDGET_H

#include <QWidget>
#include <QPointer>
#include <moveevaluation.h>

class QTableWidget;
class ChessPlayer;

/*!
 * \brief A widget that shows the engine's thinking in realtime.
 */
class EvalWidget : public QWidget
{
	Q_OBJECT

	public:
		/*! Creates a new EvalWidget object. */
		explicit EvalWidget(QWidget* parent = nullptr);

		/*!
		 * Connects the widget to \a player and disconnects from
		 * the previous player (if any).
		 */
		void setPlayer(ChessPlayer* player);

	private slots:
		void clear();
		void onEval(const MoveEvaluation& eval);

	private:
		enum StatHeaders
		{
			NpsHeader,
			HashHeader,
			PonderMoveHeader,
			PonderHitHeader,
			TbHeader
		};

		QPointer<ChessPlayer> m_player;
		QTableWidget* m_statsTable;
		QTableWidget* m_pvTable;
		int m_depth;
		QString m_pv;
};

#endif // EVALWIDGET_H

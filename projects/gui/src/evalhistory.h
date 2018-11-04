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

#ifndef EVALHISTORY_H
#define EVALHISTORY_H

#include <QWidget>
#include <QPointer>

class QCustomPlot;
class ChessGame;
class PgnGame;

/*!
 * \brief A widget that shows engines' move evaluation history.
 *
 * The fullmove number is on the X axis and score (from white's
 * perspective) is on the Y axis.
 */
class EvalHistory : public QWidget
{
	Q_OBJECT

	public:
		/*! Creates a new EvalHistory widget. */
		explicit EvalHistory(QWidget* parent = nullptr);
		/*!
		 * Connects the widget to \a game and disconnects
		 * from the previous game (if any).
		 */
		void setGame(ChessGame* game);
		/*! Sets evaluation history from PGN game (pointer) \a pgn */
		void setPgnGame(PgnGame *pgn);

	private slots:
		void onScore(int ply, int score);

	private:
		void addData(int ply, int score);
		void replot(int maxPly);
		void setScores(const QMap<int, int> &scores);

		QCustomPlot* m_plot;
		QPointer<ChessGame> m_game;
};

#endif // EVALHISTORY_H

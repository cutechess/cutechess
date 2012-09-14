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

#ifndef GAMEVIEWER_H
#define GAMEVIEWER_H

#include <QWidget>
#include <QVector>
#include <QPointer>
#include <board/genericmove.h>
class QAction;
class QToolBar;
class QSlider;
class ChessGame;
class PgnGame;
class BoardScene;
class BoardView;

class GameViewer : public QWidget
{
	Q_OBJECT

	public:
		explicit GameViewer(Qt::Orientation orientation = Qt::Horizontal,
				    QWidget* parent = 0);

		void setGame(ChessGame* game);
		void setGame(const PgnGame* pgn);
		void disconnectGame();

	public slots:
		void viewMove(int index);

	private slots:
		void viewFirstMove();
		void viewPreviousMove();
		void viewNextMove();
		void viewLastMove();
		void viewPosition(int index);

		void onFenChanged(const QString& fen);
		void onMoveMade(const Chess::GenericMove& move);

	private:
		BoardScene* m_boardScene;
		BoardView* m_boardView;
		QToolBar* m_toolBar;
		QSlider* m_moveNumberSlider;

		QAction* m_viewFirstMoveAction;
		QAction* m_viewPreviousMoveAction;
		QAction* m_viewNextMoveAction;
		QAction* m_viewLastMoveAction;

		QPointer<ChessGame> m_game;
		QVector<Chess::GenericMove> m_moves;
		int m_moveIndex;
};

#endif // GAMEVIEWER_H

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

#ifndef MOVE_LIST_H
#define MOVE_LIST_H

#include <QTextEdit>

class ChessGame;
namespace Chess { class GenericMove; }
class QMouseEvent;
class QSyntaxHighlighter;

class MoveList : public QTextEdit
{
	Q_OBJECT

	public:
		/*! Constructs a move list with the given \a parent. */
		MoveList(QWidget* parent = 0);

		/*! Associates \a game with this document. */
		void setGame(ChessGame* game);

	signals:
		void moveClicked(int side, int move);
		void commentClicked(int side, int move);
	
	protected:
		void mouseReleaseEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);

	private slots:
		void onMoveMade(const Chess::GenericMove& genericMove,
		                const QString& sanString,
		                const QString& comment);

	private:
		int findKey(int pos, const QList<int>& list);
		QSyntaxHighlighter* m_syntax;
		ChessGame* m_game;
};

#endif // MOVE_LIST_H

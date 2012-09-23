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

#include <QWidget>
#include <QPointer>
#include <QUrl>

class QTextBrowser;
class PgnGame;
class ChessGame;
namespace Chess { class GenericMove; }
class QMouseEvent;
class QSyntaxHighlighter;

class MoveList : public QWidget
{
	Q_OBJECT

	public:
		/*! Constructs a move list with the given \a parent. */
		MoveList(QWidget* parent = 0);

		/*!
		 * Associates \a game and \a pgn with this document.
		 *
		 * Either \a game or \a pgn must not be NULL.
		 * If \a pgn is NULL, then the PGN data is retrieved from \a game.
		 */
		void setGame(ChessGame* game, PgnGame* pgn = 0);

	signals:
		/*! Emitted when the user clicks move \a num. */
		void moveClicked(int num);
		/*! Emitted when the user clicks comment \a num. */
		void commentClicked(int num);
	
	private slots:
		void onMoveMade(const Chess::GenericMove& genericMove,
		                const QString& sanString,
		                const QString& comment);
		void onMoveOrCommentClicked(const QUrl& url);

	private:
		void insertHtmlMove(const QString& move);
		QTextBrowser* m_moveList;
		QPointer<ChessGame> m_game;
		int m_moveCount;
		int m_startingSide;
};

#endif // MOVE_LIST_H

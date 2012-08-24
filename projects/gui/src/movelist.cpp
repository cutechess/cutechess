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

#include "movelist.h"
#include <QMouseEvent>
#include <chessgame.h>
#include "movehighlighter.h"

MoveList::MoveList(QWidget* parent)
	: QTextEdit(parent),
	  m_syntax(new MoveHighlighter(document())),
	  m_game(0)
{
	setReadOnly(true);
	setMouseTracking(true);
}

void MoveList::setGame(ChessGame* game)
{
	Q_ASSERT(game != 0);

	if (m_game != 0)
		m_game->disconnect(this);
	m_game = game;

	clear();

	QString moves;
	moves.reserve(512);

	for (int i = 0; i < m_game->pgn()->moves().size(); i++)
	{
		const PgnGame::MoveData& md = m_game->pgn()->moves().at(i);
		QString move = QString("%1 ").arg(md.moveString);

		if ((i + 1) % 2 != 0)
			move.prepend(QString("%1. ").arg((i + 1) / 2 + 1));
		if (!md.comment.isEmpty())
			move.append(QString("{%1} ").arg(md.comment));

		moves.append(move);
	}

	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::End);
	setTextCursor(cursor);
	insertPlainText(moves);

	connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		this, SLOT(onMoveMade(Chess::GenericMove, QString, QString)));
}

void MoveList::onMoveMade(const Chess::GenericMove& genericMove,
                                 const QString& sanString,
                                 const QString& comment)
{
	Q_UNUSED(genericMove);

	int moveCount = m_game->pgn()->moves().size();
	QString move = QString("%1 ").arg(sanString);

	if (moveCount % 2 != 0)
		move.prepend(QString("%1. ").arg(moveCount / 2 + 1));

	if (!comment.isEmpty())
		move.append(QString("{%1} ").arg(comment));

	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::End);
	setTextCursor(cursor);
	insertPlainText(move);
}

void MoveList::mouseReleaseEvent(QMouseEvent *e)
{
	int pos = textCursor().position() - textCursor().block().position();
	const MoveHighlighter::MoveData* md =
		dynamic_cast<MoveHighlighter::MoveData *>(textCursor().block().userData());

	if (md && e->button() == Qt::LeftButton && !textCursor().hasSelection() &&
		pos < textCursor().block().length() - 1)
	{
		int key = findKey(pos, md->tokens.keys());

		if (key != -1)
		{
			MoveHighlighter::MoveToken token = md->tokens.value(key);

			if (token.type == MoveHighlighter::Move)
				emit(moveClicked(token.side, token.move));
			else if (token.type == MoveHighlighter::Comment)
				emit(commentClicked(token.side, token.move));
		}
	}
	QTextEdit::mouseReleaseEvent(e);
}

void MoveList::mouseMoveEvent(QMouseEvent *e)
{
	QCursor newCursor = Qt::IBeamCursor;
	const QTextCursor cursor = cursorForPosition(e->pos());
	int pos = cursor.position() - cursor.block().position();
	const MoveHighlighter::MoveData* md =
		dynamic_cast<MoveHighlighter::MoveData *>(textCursor().block().userData());

	if (md && !textCursor().hasSelection() &&
		pos < textCursor().block().length() - 1)
	{
		int key = findKey(pos, md->tokens.keys());

		if (key != -1)
		{
			MoveHighlighter::MoveToken token = md->tokens.value(key);

			if (token.type == MoveHighlighter::Move ||
			    token.type == MoveHighlighter::Comment)
				newCursor = Qt::PointingHandCursor;
		}
	}
	viewport()->setCursor(newCursor);
	QTextEdit::mouseMoveEvent(e);
}

int MoveList::findKey(int pos, const QList<int>& list)
{
	if (list.isEmpty())
		return -1;

	int i;
	bool found = false;

	for (i = 0; i < list.size(); i++)
	{
		if (pos >= list.at(i))
			found = true;

		if (found && pos < list.at(i))
		{
			i--;
			break;
		}
	}
	if (!found)
		return -1;

	if (i == list.size())
		i--;

	return list.at(i);
}

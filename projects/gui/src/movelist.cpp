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
	  m_game(0),
	  m_moveCount(0),
	  m_startingSide(0)
{
	setReadOnly(true);
	setMouseTracking(true);
}

static void appendMove(QString& s,
		       int moveNum,
		       int startingSide,
		       const QString& moveString,
		       const QString& comment)
{
	QString move = QString("%1 ").arg(moveString);

	#ifndef Q_OS_WIN32
	move.replace('-', QString::fromUtf8("\xE2\x81\xA0-\xE2\x81\xA0"));
	#endif

	if (moveNum == 0 && startingSide == Chess::Side::Black)
		move.prepend("1... ");
	else
	{
		moveNum += startingSide;
		if (moveNum % 2 == 0)
			move.prepend(QString("%1. ").arg(moveNum / 2 + 1));
	}

	if (!comment.isEmpty())
		move.append(QString("{%1} ").arg(comment));

	s.append(move);
}

void MoveList::setGame(ChessGame* game, PgnGame* pgn)
{
	if (m_game != 0)
		m_game->disconnect(this);
	m_game = game;

	if (pgn == 0)
	{
		Q_ASSERT(game != 0);
		pgn = m_game->pgn();
	}

	clear();

	QString moves;
	moves.reserve(512);

	m_startingSide = pgn->startingSide();
	for (m_moveCount = 0; m_moveCount < pgn->moves().size(); m_moveCount++)
	{
		const PgnGame::MoveData& md = pgn->moves().at(m_moveCount);
		appendMove(moves, m_moveCount, m_startingSide, md.moveString, md.comment);
	}

	insertPlainTextMove(moves);

	if (m_game != 0)
		connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
			this, SLOT(onMoveMade(Chess::GenericMove, QString, QString)));
}

void MoveList::onMoveMade(const Chess::GenericMove& genericMove,
                                 const QString& sanString,
                                 const QString& comment)
{
	Q_UNUSED(genericMove);

	QString move;
	appendMove(move, m_moveCount++, m_startingSide, sanString, comment);
	insertPlainTextMove(move);
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
			int index = (token.side - m_startingSide) + (token.move - 1) * 2;

			if (token.type == MoveHighlighter::Move)
				emit moveClicked(index, token.side, token.move);
			else if (token.type == MoveHighlighter::Comment)
				emit commentClicked(index, token.side, token.move);
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
		dynamic_cast<MoveHighlighter::MoveData *>(cursor.block().userData());

	if (md && !cursor.hasSelection() &&
		pos < cursor.block().length() - 1)
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

void MoveList::insertPlainTextMove(const QString& move)
{
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::End);
	setTextCursor(cursor);
	insertPlainText(move);
}

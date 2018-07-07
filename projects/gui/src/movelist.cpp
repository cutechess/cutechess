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

#include "movelist.h"
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QKeyEvent>
#include <chessgame.h>


MoveList::MoveList(QWidget* parent)
	: QWidget(parent),
	  m_game(nullptr),
	  m_moveCount(0),
	  m_startingSide(0),
	  m_selectedMove(-1),
	  m_moveToBeSelected(-1),
	  m_selectionTimer(new QTimer(this))
{
	m_moveList = new QTextBrowser(this);
	m_moveList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_moveList->setOpenLinks(false);
	m_moveList->setUndoRedoEnabled(false);
	m_moveList->document()->setDefaultStyleSheet(
		"a:link { text-decoration: none; } "
		".move { color: black; font-weight: bold; } "
		".comment { color: green; }");

	#ifdef Q_OS_WIN32
	QFont font(m_moveList->document()->defaultFont());
	font.setPointSize(10);
	m_moveList->document()->setDefaultFont(font);
	#endif

	connect(m_moveList, SIGNAL(anchorClicked(const QUrl&)), this,
	    SLOT(onLinkClicked(const QUrl&)));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_moveList);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);

	m_selectionTimer->setSingleShot(true);
	m_selectionTimer->setInterval(50);
	connect(m_selectionTimer, SIGNAL(timeout()),
		this, SLOT(selectChosenMove()));

	m_moveList->document()->setIndentWidth(18);

	QTextCharFormat format(m_moveList->currentCharFormat());
	m_defaultTextFormat.setForeground(format.foreground());
	m_defaultTextFormat.setBackground(format.background());

	m_moveList->installEventFilter(this);
}

void MoveList::insertMove(int ply,
			  const QString& san,
			  const QString& comment,
			  QTextCursor cursor)
{
	Move move = {
		MoveNumberToken(ply, m_startingSide),
		MoveToken(ply, san),
		MoveCommentToken(ply, comment)
	};

	bool editAsBlock = cursor.isNull();
	if (editAsBlock)
	{
		cursor = m_moveList->textCursor();
		cursor.beginEditBlock();
		cursor.movePosition(QTextCursor::End);
	}

	move.number.insert(cursor);
	move.move.insert(cursor);
	move.comment.insert(cursor);

	m_moves.append(move);

	if (editAsBlock)
		cursor.endEditBlock();
}

void MoveList::setGame(ChessGame* game, PgnGame* pgn)
{
	if (m_game != nullptr)
		m_game->disconnect(this);
	m_game = game;

	if (pgn == nullptr)
	{
		Q_ASSERT(game != nullptr);
		pgn = m_game->pgn();
	}

	m_moveList->clear();
	m_moves.clear();
	m_selectedMove = -1;
	m_moveToBeSelected = -1;
	m_selectionTimer->stop();

	QTextCursor cursor(m_moveList->textCursor());
	cursor.beginEditBlock();
	cursor.movePosition(QTextCursor::End);

	m_startingSide = pgn->startingSide();
	m_moveCount = 0;
	for (const PgnGame::MoveData& md : pgn->moves())
	{
		insertMove(m_moveCount++, md.moveString, md.comment, cursor);
	}
	cursor.endEditBlock();

	if (m_game != nullptr)
	{
		connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
			this, SLOT(onMoveMade(Chess::GenericMove, QString, QString)));
		connect(m_game, SIGNAL(moveChanged(int, Chess::GenericMove, QString, QString)),
			this, SLOT(setMove(int, Chess::GenericMove, QString, QString)));
	}

	QScrollBar* sb = m_moveList->verticalScrollBar();
	sb->setValue(sb->maximum());

	selectMove(m_moveCount - 1);
}

bool MoveList::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == m_moveList)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			int index = m_moveToBeSelected;
			bool keyLeft = false;
			if (index == -1)
				index = m_selectedMove;
			if (keyEvent->key() == Qt::Key_Left)
			{
				keyLeft = true;
				index--;
			}
			else if (keyEvent->key() == Qt::Key_Right)
				index++;
			else
				return true;

			if (index != -1 && selectMove(index))
				emit moveClicked(index, keyLeft);
		}
		return false;
	}

	return QWidget::eventFilter(obj, event);
}

void MoveList::onMoveMade(const Chess::GenericMove& move,
			  const QString& sanString,
			  const QString& comment)
{
	Q_UNUSED(move);

	QScrollBar* sb = m_moveList->verticalScrollBar();
	bool atEnd = sb->value() == sb->maximum();

	insertMove(m_moveCount++, sanString, comment);

	bool atLastMove = false;
	if (m_selectedMove == -1 || m_moveToBeSelected == m_moveCount - 2)
		atLastMove = true;
	if (m_moveToBeSelected == -1 && m_selectedMove == m_moveCount - 2)
		atLastMove = true;

	if (atLastMove)
		selectMove(m_moveCount - 1);

	if (atEnd && atLastMove)
		sb->setValue(sb->maximum());
}

// TODO: Handle changes to actual moves (eg. undo), not just comments
void MoveList::setMove(int ply,
		       const Chess::GenericMove& move,
		       const QString& sanString,
		       const QString& comment)
{
	Q_UNUSED(move);
	Q_UNUSED(sanString);
	Q_ASSERT(ply < m_moves.size());

	QTextCursor c(m_moveList->textCursor());

	MoveCommentToken& commentToken(m_moves[ply].comment);
	int oldLength = commentToken.length();
	commentToken.setValue(comment);
	commentToken.select(c);
	commentToken.insert(c);

	int newLength = commentToken.length();
	int diff = newLength - oldLength;
	if (diff == 0)
		return;

	for (int i = ply + 1; i < m_moves.size(); i++)
	{
		m_moves[i].number.move(diff);
		if (i == ply + 1)
		{
			MoveNumberToken& nextNumber(m_moves[i].number);
			oldLength = nextNumber.length();
			nextNumber.select(c);
			nextNumber.insert(c);
			newLength = nextNumber.length();
			diff += (newLength - oldLength);
		}
		m_moves[i].move.move(diff);
		m_moves[i].comment.move(diff);
	}
}

void MoveList::selectChosenMove()
{
	int moveNum = m_moveToBeSelected;
	m_moveToBeSelected = -1;
	Q_ASSERT(moveNum >= 0 && moveNum < m_moveCount);

	QTextCursor c(m_moveList->textCursor());
	c.beginEditBlock();

	if (m_selectedMove >= 0)
	{
		m_moves[m_selectedMove].move.mergeCharFormat(c, m_defaultTextFormat);
	}

	m_selectedMove = moveNum;

	QTextCharFormat format;

	format.setForeground(Qt::white);
	format.setBackground(Qt::black);
	m_moves[moveNum].move.mergeCharFormat(c, format);

	c.endEditBlock();
	m_moveList->setTextCursor(c);
}

bool MoveList::selectMove(int moveNum)
{
	if (moveNum == -1)
		moveNum = 0;
	if (moveNum >= m_moveCount || m_moveCount <= 0)
		return false;
	if (moveNum == m_selectedMove)
		return false;

	m_moveToBeSelected = moveNum;
	m_selectionTimer->start();
	return true;
}

void MoveList::onLinkClicked(const QUrl& url)
{
	bool ok;
	int ply = url.userName().toInt(&ok);

	if (!ok)
	{
		qWarning("MoveList: invalid move number: %s",
		    qUtf8Printable(url.userName()));

		return;
	}

	const Move& move(m_moves.at(ply));
	if (url.scheme() == "move")
	{
		emit moveClicked(ply, false);
	}
	else if (url.scheme() == "comment")
	{
		emit commentClicked(ply, move.comment.toString());
		return;
	}
	else
		qWarning("MoveList: unknown scheme: %s",
			 qUtf8Printable(url.scheme()));

	selectMove(ply);
}

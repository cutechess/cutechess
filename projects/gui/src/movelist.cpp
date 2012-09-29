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
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QScrollBar>
#include <chessgame.h>

MoveList::MoveList(QWidget* parent)
	: QWidget(parent),
	  m_game(0),
	  m_moveCount(0),
	  m_startingSide(0)
{
	m_moveList = new QTextBrowser(this);
	m_moveList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_moveList->setOpenLinks(false);
	m_moveList->document()->setDefaultStyleSheet(
		"a:link { text-decoration: none; } "
		".comment { color: grey; }");
	connect(m_moveList, SIGNAL(anchorClicked(const QUrl&)), this,
	    SLOT(onMoveOrCommentClicked(const QUrl&)));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_moveList);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
}

static void appendMove(QString& s,
		       int moveNum,
		       int startingSide,
		       const QString& moveString,
		       const QString& comment)
{
	QString move = QString("<a href=\"move://%1\">%2</a> ").arg(moveNum).arg(moveString);

	#ifndef Q_OS_WIN32
	move.replace('-', "&#8288;-&#8288;");
	#endif

	if (moveNum == 0 && startingSide == Chess::Side::Black)
		move.prepend("<strong>1...</strong> ");
	else
	{
		moveNum += startingSide;
		if (moveNum % 2 == 0)
			move.prepend(QString("<strong>%1.</strong> ").arg(moveNum / 2 + 1));
	}

	if (!comment.isEmpty())
		move.append(QString("<a class=\"comment\" href=\"comment://%1\">%2</a> ")
			    .arg(moveNum).arg(comment));

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

	m_moveList->clear();

	QString moves;
	moves.reserve(512);

	m_startingSide = pgn->startingSide();
	for (m_moveCount = 0; m_moveCount < pgn->moves().size(); m_moveCount++)
	{
		const PgnGame::MoveData& md = pgn->moves().at(m_moveCount);
		appendMove(moves, m_moveCount, m_startingSide, md.moveString, md.comment);
	}

	insertHtmlMove(moves);

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
	insertHtmlMove(move);
}

void MoveList::insertHtmlMove(const QString& move)
{
	QScrollBar* sb = m_moveList->verticalScrollBar();
	bool atEnd = sb->value() == sb->maximum();

	QTextCursor cursor = m_moveList->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertHtml(move);

	if (atEnd)
		sb->setValue(sb->maximum());
}

void MoveList::onMoveOrCommentClicked(const QUrl& url)
{
	bool ok;
	int moveNum = url.authority().toInt(&ok);

	if (!ok)
	{
		qWarning("MoveList: invalid move number: %s",
		    qPrintable(url.authority()));

		return;
	}

	if (url.scheme() == "move")
		emit moveClicked(moveNum);
	else if (url.scheme() == "comment")
		emit commentClicked(moveNum);
	else
		qWarning("MoveList: unknown scheme: %s",
		    qPrintable(url.scheme()));
}

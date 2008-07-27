/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "chessgame.h"
#include "chessboard/chessboard.h"
#include "chessplayer.h"

ChessGame::ChessGame(QObject *parent)
: QObject(parent)
{
	m_whitePlayer = 0;
	m_blackPlayer = 0;
	m_sideToMove = 0;
	m_chessboard = new Chessboard();
}

ChessGame::~ChessGame()
{
	delete m_chessboard;
}

Chessboard* ChessGame::chessboard() const
{
	return m_chessboard;
}

void ChessGame::moveMade(const ChessMove& move)
{
	ChessPlayer* sender = dynamic_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(sender != 0);

	if (sender != m_sideToMove) {
		qDebug("%s tried to make a move on the opponent's turn", qPrintable(sender->name()));
		return;
	}

	if (!m_chessboard->isLegalMove(move)) {
		qDebug("%s sent an illegal move", qPrintable(sender->name()));
		return;
	}

	if (m_sideToMove == m_whitePlayer)
		m_sideToMove = m_blackPlayer;
	else
		m_sideToMove = m_whitePlayer;

	m_sideToMove->sendOpponentsMove(move);
	m_chessboard->makeMove(move);
}

void ChessGame::newGame(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer)
{
	m_chessboard->setFenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	m_whitePlayer = whitePlayer;
	m_blackPlayer = blackPlayer;

	connect(m_whitePlayer, SIGNAL(debugMessage(const QString&)),
	        this, SIGNAL(debugMessage(const QString&)));
	connect(m_blackPlayer, SIGNAL(debugMessage(const QString&)),
	        this, SIGNAL(debugMessage(const QString&)));

	connect(m_whitePlayer, SIGNAL(moveMade(const ChessMove&)),
	        this, SLOT(moveMade(const ChessMove&)));
	connect(m_blackPlayer, SIGNAL(moveMade(const ChessMove&)),
	        this, SLOT(moveMade(const ChessMove&)));

	m_whitePlayer->setSide(Chessboard::White);
	m_blackPlayer->setSide(Chessboard::Black);
	m_sideToMove = m_whitePlayer;
	m_sideToMove->go();
}

ChessPlayer* ChessGame::whitePlayer() const
{
	return m_whitePlayer;
}

ChessPlayer* ChessGame::blackPlayer() const
{
	return m_blackPlayer;
}


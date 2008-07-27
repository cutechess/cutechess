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

ChessGame::ChessGame()
{
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

void ChessGame::newGame(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer)
{
	m_whitePlayer = whitePlayer;
	m_blackPlayer = blackPlayer;
}

ChessPlayer* ChessGame::whitePlayer() const
{
	return m_whitePlayer;
}

ChessPlayer* ChessGame::blackPlayer() const
{
	return m_blackPlayer;
}


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

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "manager.h"

class Chessboard;
class ChessPlayer;

class GameManager : public ManagerBase<GameManager>
{
	friend class ManagerBase<GameManager>;

	public:
		/**
		 * Returns the internal chessboard.
		 * @return Internal chessboard.
		*/
		Chessboard* chessboard() const;
		/**
		 * Starts a new game.
		 * @param whitePlayer The player that plays the white colored pieces.
		 * @param blackPlayer The player that plays the black colored pieces.
		*/
		void newGame(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer);
	
	protected:
		GameManager();
		~GameManager();
	
	private:
		Chessboard* m_chessboard;
		ChessPlayer* m_whitePlayer;
		ChessPlayer* m_blackPlayer;
	
};

#endif // GAMEMANAGER_H


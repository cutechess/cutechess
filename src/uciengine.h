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

#ifndef UCIENGINE_H
#define UCIENGINE_H

#include <QStringList>
#include "chessengine.h"

/**
 * The UciEngine class represents a chess engine which uses the UCI
 * chess interface.
 *
 * UCI's specifications: http://wbec-ridderkerk.nl/html/UCIProtocol.html
 * @see ChessEngine
 */
class UciEngine : public ChessEngine
{
	Q_OBJECT

	public:
		/**
		 * Creates a new UciEngine object.
		 * @param ioDevice An open chess engine process or socket.
		 * @param chessboard A chessboard object for converting between the
		 * various move formats.
		 * @param timeControl Time control for the player.
		 * @param parent The parent object.
		 */
		UciEngine(QIODevice* ioDevice,
		          Chessboard* chessboard,
		          const TimeControl& timeControl,
		          QObject* parent = 0);

		~UciEngine();

		void newGame(Chessboard::ChessSide side, ChessPlayer* opponent);
		void go();
		void makeMove(const ChessMove& move);
		ChessProtocol protocol() const;
		void ping();

	protected:
		void parseLine(const QString& line);
		
	private:
		QStringList m_moves;
};

#endif // UCIENGINE_H


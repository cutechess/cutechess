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

#ifndef XBOARDENGINE_H
#define XBOARDENGINE_H

#include "chessengine.h"

/**
 * The XboardEngine class represents a chess engine which uses the Xboard
 * chess engine communication protocol.
 *
 * Xboard's specifications: http://www.tim-mann.org/xboard/engine-intf.html
 * @see ChessEngine
 */
class XboardEngine : public ChessEngine
{
	Q_OBJECT

	public:
		/**
		 * Creates a new XboardEngine object.
		 * @param ioDevice An open chess engine process or socket.
		 * @param chessboard A chessboard object for converting between the
		 * various move formats.
		 * @param timeControl Time control for the player.
		 * @param parent The parent object.
		 */
		XboardEngine(QIODevice* ioDevice,
		             Chessboard* chessboard,
		             const TimeControl& timeControl,
		             QObject* parent = 0);

		~XboardEngine();

		void newGame(Chessboard::ChessSide side, ChessPlayer* opponent);
		void go();
		void sendOpponentsMove(const ChessMove& move);
		ChessProtocol protocol() const;
		void ping();

	protected:
		void parseLine(const QString& line);

	private:
		bool m_forceMode;
		int m_lastPing;
		
		void sendTimeLeft();
};

#endif // XBOARDENGINE_H


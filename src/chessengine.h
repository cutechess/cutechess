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

#ifndef CHESSENGINE_H
#define CHESSENGINE_H

#include "chessplayer.h"

class QString;
class QIODevice;

/**
 * The ChessEngine class represents an artificial intelligence chess player,
 * which is a separate program using either the Xboard or Uci chess protocol.
 * Communication between the GUI and the chess engines happens via a QIODevice.
 * @see ChessPlayer
 */
class ChessEngine : public ChessPlayer
{
	Q_OBJECT

	public:
		/**
		 * The chess protocol that is used to communicate with the engine.
		 */
		enum ChessProtocol
		{
			Xboard, /**< The Xboard/Winboard chess protocol. */
			Uci /**< The Universal Chess Interface (UCI). */
		};
		
		/**
		 * Chess move notation for move strings.
		 */
		enum MoveNotation
		{
			LongNotation, /**< Long Algebraic Notation, or Coordinate Notation. */
			StandardNotation /**< Standard Algebraic Notation, or SAN. */
		};

		/**
		 * Creates a new ChessEngine object.
		 * @param ioDevice An open chess engine process or socket.
		 * @param chessboard A chessboard object for converting between the
		 * various move formats.
		 * @param timeControl Time control for the player.
		 * @param parent The parent object.
		 */
		ChessEngine(QIODevice* ioDevice,
		            Chessboard* chessboard,
		            TimeControl* timeControl,
		            QObject* parent = 0);

		virtual ~ChessEngine();

		virtual void newGame(Chessboard::ChessSide side) = 0;
		virtual void go() = 0;
		virtual bool isHuman() const;
		virtual void sendOpponentsMove(const ChessMove& move) = 0;
		
		/**
		 * Is the player ready to play?
		 */
		bool isReady() const;
		
		/**
		 * Gets the chess protocol which the engine uses.
		 * @return The chess protocol.
		 */
		virtual ChessProtocol protocol() const = 0;

		/**
		 * Writes data to the chess engine.
		 * @param data The data that will be written to the engine's device.
		 */
		void write(const QString& data) const;

	protected:
		/**
		 * Parses a line of input from the engine.
		 * @param line A line of text.
		 */
		virtual void parseLine(const QString& line) = 0;


		/** The chessboard which the player plays on. */
		Chessboard *m_chessboard;

		/**
		 * The chess move notation the engine wants to use.
		 * All moves which are sent to and received from the engine must
		 * be in this format.
		 */
		MoveNotation m_notation;

		/** Is the player ready to play? */
		bool m_isReady;
		
		/** The ID number of the chess engine. */
		int m_id;
		
		/** The number of active chess engines. */
		static int m_count;

	protected slots:
		/**
		 * Reads input from the engine.
		 */
		void on_readyRead();

	private:
		QIODevice *m_ioDevice;

};

#endif // CHESSENGINE_H


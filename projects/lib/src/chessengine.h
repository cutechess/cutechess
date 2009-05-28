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

#ifndef CHESSENGINE_H
#define CHESSENGINE_H

#include <QStringList>
#include "chessplayer.h"

class QString;
class QIODevice;
class EngineSettings;

/*!
 * \brief An artificial intelligence chess player.
 *
 * ChessEngine is a separate process (run locally or over a network) using
 * either the Xboard or Uci chess protocol. Communication between the GUI
 * and the chess engines happens via a QIODevice.
 *
 * \sa XboardEngine
 * \sa UciEngine
 */
class LIB_EXPORT ChessEngine : public ChessPlayer
{
	Q_OBJECT

	public:
		/*! The chess communication protocol. */
		enum Protocol
		{
			Xboard,	//!< The Xboard/Winboard chess protocol.
			Uci	//!< The Universal Chess Interface (UCI).
		};
		
		/*! Ping or synchronization type. */
		enum PingType
		{
			//! Wait for initialization.
			PingInit,
			//! Wait until the engine is ready to think of a move.
			PingMove,
			//! Ping the engine for any reason.
			PingUnknown
		};
		
		/*! Creates and initializes a new ChessEngine. */
		ChessEngine(QIODevice* ioDevice, QObject* parent = 0);
		virtual ~ChessEngine();

		// Inherited from ChessPlayer
		virtual void endGame(Chess::Result result);
		bool isHuman() const;
		bool supportsVariant(Chess::Variant variant) const;

		/*!
		 * Starts the engine.
		 * The engine process must already be started.
		 */
		virtual void start() = 0;

		/*! Applies \a settings on the engine. */
		virtual void applySettings(const EngineSettings& settings);
		
		/*! Returns the engine's chess protocol. */
		virtual Protocol protocol() const = 0;

		/*! Writes text data to the chess engine. */
		void write(const QString& data);
		
		/*!
		 * Tells the engine to use a maximum of \a limit
		 * threads or processes for searching.
		 */
		virtual void setConcurrency(int limit) = 0;
		
		/*! Sets the path to endgame bitbases (eg. Scorpio's). */
		virtual void setEgbbPath(const QString& path) = 0;
		
		/*! Sets the path to Nalimov endgame table bases. */
		virtual void setEgtbPath(const QString& path) = 0;
		
		/*! Tells the engine to terminate. */
		void quit();
		
	protected:
		// Inherited from ChessPlayer
		virtual void startGame() = 0;
		void closeConnection();

		/*! Parses a line of input from the engine. */
		virtual void parseLine(const QString& line) = 0;

		/*!
		 * Checks if the engine is still responding, and synchronizes
		 * it with the GUI.
		 */
		virtual void ping(PingType type = PingUnknown);

		/*! Tells the engine to stop thinking and move now. */
		virtual void stopThinking() = 0;

		/*! Is the engine initialized? */
		bool m_initialized;

		/*! Is the engine finishing a game? */
		bool m_finishingGame;

		/*! Are evaluation scores from white's point of view? */
		bool m_whiteEvalPov;

		/*! The last ping's type. */
		PingType m_pingType;

		/*! Timer for detecting an unresponsive engine. */
		QTimer m_pingTimer;

		/*! Supported variants. */
		QVector<Chess::Variant> m_variants;
		
	protected slots:
		// Inherited from ChessPlayer
		void onDisconnect();
		void onTimeout();

		/*! Reads input from the engine. */
		void onReadyRead();

		/*! Called when the engine becomes unresponsive. */
		void onPingTimeout();

		/*! Called when the engine responds to ping. */
		void pong();

		/*!
		 * Flushes the write buffer.
		 * If there are any commands in the buffer, they will be sent
		 * to the engine.
		 */
		void flushWriteBuffer();

	private:
		static int m_count;
		int m_id;
		QIODevice *m_ioDevice;
		QStringList m_writeBuffer;
};

#endif // CHESSENGINE_H

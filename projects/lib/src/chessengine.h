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

#include "chessplayer.h"
#include <QVariant>
#include <QStringList>
#include "engineconfiguration.h"

class QIODevice;
class EngineOption;


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
		/*!
		 * The write mode used by \a write() when the engine is
		 * being pinged. This doesn't affect the IO device's
		 * buffering.
		 */
		enum WriteMode
		{
			Buffered,	//!< Use the write buffer
			Unbuffered	//!< Bypass the write buffer
		};

		/*! Creates and initializes a new ChessEngine. */
		ChessEngine(QObject* parent = 0);
		virtual ~ChessEngine();

		/*! Returns the current device associated with the engine. */
		QIODevice* device() const;
		/*! Sets the current device to \a device. */
		void setDevice(QIODevice* device);

		// Inherited from ChessPlayer
		void closeConnection();
		void quit();
		virtual void endGame(const Chess::Result& result);
		bool isHuman() const;
		bool isReady() const;
		bool supportsVariant(const QString& variant) const;

		/*!
		 * Starts communicating with the engine.
		 * \note The engine device must already be started.
		 */
		void start();

		/*! Applies \a configuration to the engine. */
		void applyConfiguration(const EngineConfiguration& configuration);

		/*!
		 * Sends a ping message (an echo request) to the engine to
		 * check if it's still responding to input, and to synchronize
		 * it with the game operator. If the engine doesn't respond in
		 * reasonable time, it will be terminated.
		 *
		 * \note All input to the engine will be delayed until we
		 * get a response to the ping.
		 */
		void ping();
		
		/*! Returns the engine's chess protocol. */
		virtual QString protocol() const = 0;

		/*!
		 * Writes text data to the chess engine.
		 *
		 * If \a mode is \a Unbuffered, the data will be written to
		 * the device immediately even if the engine is being pinged.
		 */
		void write(const QString& data, WriteMode mode = Buffered);

		/*!
		 * Sets an option with the name \a name to \a value.
		 *
		 * \note If the engine doesn't have an option called \a name,
		 * nothing happens.
		 */
		void setOption(const QString& name, const QVariant& value);
		
	public slots:
		// Inherited from ChessPlayer
		void go();
		
	protected:
		// Inherited from ChessPlayer
		virtual void startGame() = 0;

		/*!
		 * Puts the engine in the correct mode to start communicating
		 * with it, using the chosen chess protocol.
		 */
		virtual void startProtocol() = 0;

		/*! Parses a line of input from the engine. */
		virtual void parseLine(const QString& line) = 0;

		/*!
		 * Sends a ping command to the engine.
		 * \return True if successfull
		 */
		virtual bool sendPing() = 0;

		/*! Sends the stop command to the engine. */
		virtual void sendStop() = 0;

		/*! Sends the quit command to the engine. */
		virtual void sendQuit() = 0;

		/*! Tells the engine to stop thinking and move now. */
		void stopThinking();

		EngineOption* getOption(const QString& name) const;

		virtual void sendOption(const QString& name, const QString& value) = 0;

		/*! Are evaluation scores from white's point of view? */
		bool m_whiteEvalPov;

		/*! Supported variants. */
		QStringList m_variants;

		QVector<EngineOption*> m_options;
		
	protected slots:
		// Inherited from ChessPlayer
		void onTimeout();

		/*! Reads input from the engine. */
		void onReadyRead();

		/*! Called when the engine doesn't respond to ping. */
		void onPingTimeout();

		/*! Called when the engine idles for too long. */
		void onIdleTimeout();

		/*! Called when the engine responds to ping. */
		void pong();

		/*!
		 * Called when the engine has started the chess protocol and
		 * is ready to start a game.
		 */
		void onProtocolStart();

		/*!
		 * Flushes the write buffer.
		 * If there are any commands in the buffer, they will be sent
		 * to the engine.
		 */
		void flushWriteBuffer();

	private slots:
		void onQuitTimeout();

	private:
		static int s_count;
		int m_id;
		State m_pingState;
		bool m_pinging;
		QTimer* m_pingTimer;
		QTimer* m_quitTimer;
		QTimer* m_idleTimer;
		QIODevice *m_ioDevice;
		QStringList m_writeBuffer;
		QMap<QString, QVariant> m_optionBuffer;
};

#endif // CHESSENGINE_H

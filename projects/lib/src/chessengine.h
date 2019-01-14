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
		ChessEngine(QObject* parent = nullptr);
		virtual ~ChessEngine();

		/*! Returns the current device associated with the engine. */
		QIODevice* device() const;
		/*! Sets the current device to \a device. */
		void setDevice(QIODevice* device);

		// Inherited from ChessPlayer
		virtual void endGame(const Chess::Result& result);
		virtual bool isHuman() const;
		virtual bool isReady() const;
		virtual bool supportsVariant(const QString& variant) const;

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
		 * if \a sendCommand is false, nothing is actually sent to the
		 * engine (but we'll pretend a message was sent); otherwise the
		 * ping message is sent.
		 *
		 * \note All input to the engine will be delayed until we
		 * get a response to the ping.
		 */
		void ping(bool sendCommand = true);
		
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

		/*! Returns a list of supported options and their values. */
		QList<EngineOption*> options() const;

		/*! Returns a list of supported chess variants. */
		QStringList variants() const;

	public slots:
		// Inherited from ChessPlayer
		virtual void go();
		virtual void quit();
		virtual void kill();
		
	protected:
		/*!
		 * Reads the first whitespace-delimited token from a string
		 * and returns a QStringRef reference to the token.
		 *
		 * If \a readToEnd is true, the whole string is read, except
		 * for leading and trailing whitespace. Otherwise only one
		 * word is read.
		 *
		 * If \a str doesn't contain any words, a null QStringRef
		 * object is returned.
		 */
		static QStringRef firstToken(const QString& str,
					     bool readToEnd = false);
		/*!
		 * Reads the first whitespace-delimited token after the
		 * token referenced by \a previous.
		 *
		 * If \a readToEnd is true, everything from the first word
		 * after \a previous to the end of the string is read,
		 * except for leading and trailing whitespace. Otherwise
		 * only one word is read.
		 *
		 * If \a previous is null or it's not followed by any words,
		 * a null QStringRef object is returned.
		 */
		static QStringRef nextToken(const QStringRef& previous,
					    bool readToEnd = false);

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
		 * Returns true if successful; otherwise returns false.
		 */
		virtual bool sendPing() = 0;

		/*! Sends the stop command to the engine. */
		virtual void sendStop() = 0;

		/*! Sends the quit command to the engine. */
		virtual void sendQuit() = 0;

		/*!
		 * Tells the engine to stop thinking and move now (if on move).
		 *
		 * Returns true if the stop message was actually sent; otherwise
		 * returns false.
		 */
		bool stopThinking();

		/*! Adds \a option to the engine options list. */
		void addOption(EngineOption* option);
		/*!
		 * Returns the option that matches \a name.
		 * Returns 0 if an option with that name doesn't exist.
		 */
		EngineOption* getOption(const QString& name) const;
		/*! Tells the engine to set option \a name's value to \a value. */
		virtual void sendOption(const QString& name, const QVariant& value) = 0;

		/*! Adds \a variant to the list of supported variants. */
		void addVariant(const QString& variant);
		/*! Clears the list of supported variants. */
		void clearVariants();

		/*!
		 * Returns the restart mode.
		 * The default value is \a EngineConfiguration::RestartAuto.
		 */
		EngineConfiguration::RestartMode restartMode() const;
		/*!
		 * Returns true if the engine restarts between games; otherwise
		 * returns false.
		 */
		virtual bool restartsBetweenGames() const;
		/*!
		 * Returns true if the engine is currently thinking on the
		 * opponent's move; otherwise returns false.
		 */
		virtual bool isPondering() const;

		/*! Are evaluation scores from white's point of view? */
		bool whiteEvalPov() const;
		/*!
		 * Returns true if pondering is enabled for the engine;
		 * otherwise returns false.
		 *
		 * \note Even if pondering is enabled, it's still possible that
		 * the engine does not support pondering.
		 */
		bool pondering() const;
		/*!
		 * Gives id number of the engine
		 */
		int id() const;

	protected slots:
		// Inherited from ChessPlayer
		virtual void onTimeout();

		/*! Reads input from the engine. */
		void onReadyRead();

		/*! Called when the engine doesn't respond to ping. */
		void onPingTimeout();

		/*! Called when the engine idles for too long. */
		void onIdleTimeout();

		/*!
		 * Called when the engine responds to ping.
		 *
		 * Does nothing if the engine is not being pinged.
		 * Flushes any pending output to the engine.
		 * If \a emitReady is true, the ready() signal is emitted after
		 * pending output is flushed.
		 */
		void pong(bool emitReady = true);

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

		/*! Clear the write buffer without flushing it. */
		void clearWriteBuffer();

	private slots:
		void onQuitTimeout();
		void onProtocolStartTimeout();

	private:
		static int s_count;

		int m_id;
		State m_pingState;
		bool m_pinging;
		bool m_whiteEvalPov;
		bool m_pondering;
		QTimer* m_pingTimer;
		QTimer* m_quitTimer;
		QTimer* m_idleTimer;
		QTimer* m_protocolStartTimer;
		QIODevice *m_ioDevice;
		QStringList m_writeBuffer;
		QStringList m_variants;
		QList<EngineOption*> m_options;
		QMap<QString, QVariant> m_optionBuffer;
		EngineConfiguration::RestartMode m_restartMode;
};

#endif // CHESSENGINE_H

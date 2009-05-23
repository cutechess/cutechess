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

#ifndef XBOARDENGINE_H
#define XBOARDENGINE_H

#include "chessengine.h"

/*!
 * \brief A chess engine which uses the Xboard chess engine communication protocol.
 *
 * Xboard's specifications: http://www.tim-mann.org/xboard/engine-intf.html
 */
class LIB_EXPORT XboardEngine : public ChessEngine
{
	Q_OBJECT

	public:
		/*! Creates a new XboardEngine. */
		XboardEngine(QIODevice* ioDevice, QObject* parent = 0);

		// Inherited from ChessEngine
		void start();
		void endGame(Chess::Result result);
		void go();
		void makeMove(const Chess::Move& move);
		Protocol protocol() const;
		void ping(PingType type);
		bool inObserverMode() const;
		void setObserverMode(bool enabled);
		void setConcurrency(int limit);
		void setEgbbPath(const QString& path);
		void setEgtbPath(const QString& path);

	protected:
		// Inherited from ChessEngine
		void startGame();
		void parseLine(const QString& line);
		void stopThinking();
	
	private slots:
		/*! Initializes the engine, and emits the 'ready' signal. */
		void initialize();

	private:
		struct OptionCmd
		{
			QString line;
			bool* feature;
		};

		void setFeature(const QString& name, const QString& val);
		void setOption(const QString& line, bool* feature);
		void sendTimeLeft();
		void finishGame();
		
		bool m_forceMode;
		bool m_drawOnNextMove;
		
		// Engine features
		bool m_ftEgbb;
		bool m_ftEgtb;
		bool m_ftSmp;
		bool m_ftMemory;
		bool m_ftName;
		bool m_ftPing;
		bool m_ftSetboard;
		bool m_ftTime;
		bool m_ftUsermove;
		
		bool m_gotResult;
		bool m_waitForMove;
		int m_lastPing;
		Chess::MoveNotation m_notation;
		QTimer m_initTimer;
		QVector<OptionCmd> m_optionBuffer;
};

#endif // XBOARDENGINE_H

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

#ifndef UCIENGINE_H
#define UCIENGINE_H

#include "chessengine.h"


/*!
 * \brief A chess engine which uses the UCI chess interface.
 *
 * UCI's specifications: http://wbec-ridderkerk.nl/html/UCIProtocol.html
 */
class LIB_EXPORT UciEngine : public ChessEngine
{
	Q_OBJECT

	public:
		/*! Creates a new UciEngine. */
		UciEngine(QIODevice* ioDevice, QObject* parent = 0);

		// Inherited from ChessEngine
		void endGame(const Chess::Result& result);
		void makeMove(const Chess::Move& move);
		Protocol protocol() const;

	protected:
		// Inherited from ChessEngine
		bool sendPing();
		void sendQuit();
		void startProtocol();
		void startGame();
		void startThinking();
		void parseLine(const QString& line);
		void stopThinking();
		void sendOption(const QString& name, const QString& value);
		
	private:
		void addVariants();
		void parseInfo(const QString& line);
		EngineOption* parseOption(const QString& line);
		void sendPosition();
		
		QString m_startFen;
		QString m_moveStrings;
};

#endif // UCIENGINE_H

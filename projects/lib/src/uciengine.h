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

#include <QString>
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
		/*!
		 * Creates a new UciEngine object.
		 *
		 * \param ioDevice An open chess engine process or socket.
		 * \param chessboard A chessboard object for converting between the
		 * various move formats.
		 * \param parent The parent object.
		 */
		UciEngine(QIODevice* ioDevice,
		          Chess::Board* chessboard,
		          QObject* parent = 0);

		~UciEngine();

		void newGame(Chess::Side side, ChessPlayer* opponent);
		void go();
		void makeMove(const Chess::Move& move);
		Protocol protocol() const;
		void ping();

	protected:
		void parseLine(const QString& line);
		
	private:
		void sendPosition();
		
		QString m_startFen;
		QStringList m_moves;
};

#endif // UCIENGINE_H

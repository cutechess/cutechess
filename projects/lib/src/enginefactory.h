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

#ifndef ENGINE_FACTORY_H
#define ENGINE_FACTORY_H

#include "chessengine.h"

/*!
 * \brief Factory for creating chess engines.
 *
 * \sa ChessEngine
 * \sa XboardEngine
 * \sa UciEngine
 */
class LIB_EXPORT EngineFactory
{
	public:
		/*!
		 * Creates a chess engine based on \a protocol.
		 * \note The engine process (\a ioDevice) must be already
		 * started.
		 */
		static ChessEngine* createEngine(ChessEngine::Protocol protocol,
		                                 QIODevice* ioDevice,
		                                 QObject* parent = 0);

	private:
		EngineFactory();

};

#endif // ENGINE_FACTORY_H


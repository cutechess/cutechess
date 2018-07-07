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

#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include <QStringList>
#include "classregistry.h"
#include "chessengine.h"


/*! \brief A factory for creating ChessEngine objects. */
class LIB_EXPORT EngineFactory
{
	public:
		/*! Returns the class registry for concrete ChessEngine subclasses. */
		static ClassRegistry<ChessEngine>* registry();
		/*!
		 * Creates and returns a new engine that uses protocol \a protocol.
		 * Returns 0 if no engine class is associated with \a protocol.
		 */
		static ChessEngine* create(const QString& protocol);
		/*! Returns a list of supported chess protocols. */
		static QStringList protocols();

	private:
		EngineFactory();
};

/*!
 * Registers engine class \a TYPE with protocol name \a PROTOCOL.
 *
 * This macro must be called once for every concrete ChessEngine class.
 */
#define REGISTER_ENGINE_CLASS(TYPE, PROTOCOL) \
	REGISTER_CLASS(ChessEngine, TYPE, PROTOCOL, EngineFactory::registry());

#endif // ENGINEFACTORY_H

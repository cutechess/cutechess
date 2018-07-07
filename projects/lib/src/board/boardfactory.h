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

#ifndef BOARDFACTORY_H
#define BOARDFACTORY_H

#include <QStringList>
#include <classregistry.h>
#include "board.h"


namespace Chess {

/*! \brief A factory for creating Board objects. */
class LIB_EXPORT BoardFactory
{
	public:
		/*! Returns the class registry for concrete Board subclasses. */
		static ClassRegistry<Board>* registry();
		/*!
		 * Creates and returns a new Board of variant \a variant.
		 * Returns 0 if \a variant is not supported.
		 */
		static Board* create(const QString& variant);
		/*! Returns a list of supported chess variants. */
		static QStringList variants();

	private:
		BoardFactory();
};

/*!
 * Registers board class \a TYPE with variant name \a VARIANT.
 *
 * This macro must be called once for every concrete Board class.
 */
#define REGISTER_BOARD(TYPE, VARIANT) \
	REGISTER_CLASS(Board, TYPE, VARIANT, BoardFactory::registry());

} // namespace Chess

#endif // BOARDFACTORY_H

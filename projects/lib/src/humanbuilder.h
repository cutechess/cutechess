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

#ifndef HUMANBUILDER_H
#define HUMANBUILDER_H

#include "playerbuilder.h"
#include <QString>


/*! \brief A class for constructing human players. */
class LIB_EXPORT HumanBuilder : public PlayerBuilder
{
	public:
		/*!
		 * Creates a new HumanBuilder.
		 *
		 * Any created players will have the name \a playerName,
		 * unless it's an empty string.
		 */
		HumanBuilder(const QString& playerName = QString());

		// Inherited from PlayerBuilder
		virtual ChessPlayer* create(QObject* receiver,
					    const char* method,
					    QObject* parent) const;

	private:
		QString m_playerName;
};

#endif // HUMANBUILDER_H

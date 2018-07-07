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

#ifndef PLAYERBUILDER_H
#define PLAYERBUILDER_H

#include <QString>
class QObject;
class ChessPlayer;


/*!
 * \brief A class for constructing new chess players.
 *
 * PlayerBuilder's subclasses can create all types of chess players.
 * This class enables a GameManager object to take control of
 * constructing, reusing and deleting players, and running multiple
 * games concurrently, which requires multiple instances of the
 * same players.
 *
 * \sa GameManager
 */
class LIB_EXPORT PlayerBuilder
{
	public:
		/*! Creates a new player builder with name \a name. */
		PlayerBuilder(const QString& name);
		/*! Destroys the player builder. */
		virtual ~PlayerBuilder();

		/*!
		 * Returns true if the builder is for a human player;
		 * otherwise returns false.
		 */
		virtual bool isHuman() const = 0;
		/*! Returns the player's name. */
		QString name() const;
		/*! Sets the player's name to \a name. */
		void setName(const QString& name);
		/*!
		 * Creates a new player and sets its parent to \a parent.
		 *
		 * \param receiver The receiver of the player's debugging messages.
		 * \param method The receiver's method the \a debugMessage(QString)
		 *               signal will connect to.
		 * \param parent The player's parent object.
		 * \param error If an error occurs and \a error is not 0, the error
		 *              description is written here.
		 */
		virtual ChessPlayer* create(QObject* receiver,
					    const char* method,
					    QObject* parent,
					    QString* error) const = 0;

	private:
		QString m_name;
};

#endif // PLAYERBUILDER_H

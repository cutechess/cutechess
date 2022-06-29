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

#ifndef BOARDSETTINGS_H
#define BOARDSETTINGS_H

#include <QMap>
#include <QVariant>

/*!
 * \brief A transfer object for GraphicsBoard settings
 *
 * BoardSettings are read by GraphicsBoard for set-up.
 */
class BoardSettings
{
	public:
		/*! Creates a new BoardSettings object. */
		BoardSettings();
		/*!
		 * Returns true if the instance has been
		 * initialized else false.
		 */
		bool initialized() const;

		/*! Sets the initialized flag to \a value. */
		void setInitialized(bool value);

		/*! Returns value of setting for \a key. */
		const QVariant value(const QString key) const;

		/*! Sets item for \a key to QVariant \a value */
		void set(const QString key, const QVariant value);

	protected:


	private:
		bool m_initialized;
		QMap<QString, QVariant> m_map;
};

#endif // BOARDSETTINGS_H

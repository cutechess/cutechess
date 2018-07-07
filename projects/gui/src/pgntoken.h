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

#ifndef PGNTOKEN_H
#define PGNTOKEN_H

#include <QString>
#include <QTextCursor>
#include <QTextCharFormat>

/*!
 * \brief A text or HTML token in a PGN game.
 */
class PgnToken
{
	public:
		/*! Creates a new PgnToken. */
		PgnToken();
		/*! Destroys the PgnToken. */
		virtual ~PgnToken();

		/*!
		 * Returns true if the token is empty;
		 * otherwise returns false.
		 */
		bool isEmpty() const;
		/*! Returns the token's length. */
		int length() const;
		/*! Returns the token as a string. */
		virtual QString toString() const = 0;
		/*!
		 * Inserts the token into the document.
		 * This method calls the subclass' \a vInsert() method.
		 */
		void insert(QTextCursor& cursor);
		/*! Selects the token with \a cursor. */
		void select(QTextCursor& cursor);
		/*! Moves the token's begin and end positions by \a diff. */
		void move(int diff);
		/*! Merges the token's character format with \a format. */
		void mergeCharFormat(QTextCursor& cursor,
				     const QTextCharFormat& format);

	protected:
		/*!
		 * Subclasses must implement this function to insert the
		 * token into the document using \a cursor.
		 */
		virtual void vInsert(QTextCursor& cursor) = 0;

	private:
		int m_begin;
		int m_end;
};

#endif // PGNTOKEN_H

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

#ifndef PGNSTREAM_H
#define PGNSTREAM_H

#include <QtGlobal>
#include <QString>
#include <QTextStream>
#include "chessboard/chessboard.h"
class QIODevice;


/*!
 * \brief A class for reading games in PGN format from a text stream.
 *
 * PgnStream is used for reading PGN games from a QIODevice or a string.
 * It has its own input methods, and keeps track of the current line
 * number which can be used to report errors in the games. PgnStream
 * also has its own Chess::Board object, so that the same board can be
 * easily used with all the games in the stream.
 *
 * \sa PgnGame
 * \sa OpeningBook
 */
class LIB_EXPORT PgnStream
{
	public:
		/*!
		 * Creates a new PgnStream.
		 * A device or a string must be set before the stream
		 * can be used.
		 */
		PgnStream();
		/*! Creates a PgnStream that operates on \a device. */
		explicit PgnStream(QIODevice* device);
		/*! Creates a PgnStream that operates on \a string. */
		explicit PgnStream(QString* string);

		/*!
		 * Creates a new PgnStream and opens the file in ReadOnly mode.
		 * If the opening fails, isOpen() will return false.
		 *
		 * \param filename The name of the file which holds the PGN game(s)
		 * \param variant The chess variant for every game in the file.
		 * Games of other variants are ignored, or their moves are
		 * found to be illegal.
		 * Use \a Chess::NoVariant (the default) to allow multiple variants.
		 */

		/*!
		 * Returns the Board object which is used to verify the moves
		 * and FEN strings in the file.
		 */
		Chess::Board* board();

		/*! Returns the assigned device, or 0 if no device is in use. */
		QIODevice* device() const;
		/*! Sets the current device to \ə device. */
		void setDevice(QIODevice* device);

		/*! Returns the assigned string, or 0 if no string is in use. */
		QString* string() const;
		/*! Sets the current string to \a string. */
		void setString(QString* string);

		/*! Returns true if the stream is open. */
		bool isOpen() const;

		/*! Returns the current line number. */
		qint64 lineNumber() const;

		/*! Reads one character and returns it. */
		QChar readChar();

		/*! Reads one line of text and returns it. */
		QString readLine();

		/*! Rewinds back to the start of input. */
		void rewind();
		/*!
		 * Rewinds the stream position by one character, which means that
		 * the next time readChar() is called, nothing is read and the
		 * buffer character is returned.
		 *
		 * \note Only one character is kept in the buffer, so calling
		 * this method multiple times in a row has the same effect as
		 * calling it just once.
		 */
		void rewindChar();
		/*!
		 * Reads and discards whitespace from the stream until either
		 * a non-space character is read, or EOF is reached.
		 */
		void skipWhiteSpace();

		/*! Returns the status of the stream. */
		QTextStream::Status status() const;

		/*!
		 * Returns the chess variant of the PGN game/collection.
		 * \note The value Chess::NoVariant means that no specific
		 * variant is expected, but games may still be read.
		 */
		Chess::Variant variant() const;
		/*! Sets the expected chess variant to \a variant. */
		void setVariant(Chess::Variant variant);

	private:
		QTextStream m_in;
		Chess::Variant m_variant;
		Chess::Board m_board;
		qint64 m_lineNumber;
		QChar m_lastChar;
		bool m_rewind;
};

#endif // PGNSTREAM_H

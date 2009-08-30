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
		/*! The current status of the PGN stream. */
		enum Status
		{
			Ok,         //!< The stream is operating normally.
			ReadPastEnd //!< The stream has read past the end of the data.
		};

		/*!
		 * Creates a new PgnStream.
		 * A device or a string must be set before the stream
		 * can be used.
		 */
		PgnStream();
		/*! Creates a PgnStream that operates on \a device. */
		explicit PgnStream(QIODevice* device);
		/*! Creates a PgnStream that operates on \a string. */
		explicit PgnStream(const QString* string);

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
		const QString* string() const;
		/*! Sets the current string to \a string. */
		void setString(const QString* string);

		/*! Returns true if the stream is open. */
		bool isOpen() const;

		/*! Returns the current position in the stream. */
		qint64 pos() const;

		/*! Returns the current line number. */
		qint64 lineNumber() const;

		/*! Reads one character and returns it. */
		QChar readChar();

		/*! Reads one line of text and returns it. */
		QString readLine();

		/*! Resets the stream to its default state. */
		void reset();

		/*!
		 * Rewinds back to the start of input.
		 * This is equivalent to calling \a seek(0).
		 */
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
		 * Seeks to position \a pos in the device, and sets the current
		 * line number to \a lineNumber.
		 * Returns true if successfull.
		 */
		bool seek(qint64 pos, qint64 lineNumber = 1);
		/*!
		 * Reads and discards whitespace from the stream until either
		 * a non-space character is read, or EOF is reached.
		 */
		void skipWhiteSpace();

		/*! Returns the status of the stream. */
		Status status() const;

		/*!
		 * Returns the chess variant of the PGN game/collection.
		 * \note The value Chess::NoVariant means that no specific
		 * variant is expected, but games may still be read.
		 */
		Chess::Variant variant() const;
		/*! Sets the expected chess variant to \a variant. */
		void setVariant(Chess::Variant variant);

	private:
		Chess::Variant m_variant;
		Chess::Board m_board;
		qint64 m_pos;
		qint64 m_lineNumber;
		char m_lastChar;
		QIODevice* m_device;
		const QString* m_string;
		Status m_status;
};

#endif // PGNSTREAM_H

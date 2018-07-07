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

#ifndef PGNSTREAM_H
#define PGNSTREAM_H

#include <QtGlobal>
#include <QString>
class QIODevice;
namespace Chess { class Board; }


/*!
 * \brief A class for reading games in PGN format from a text stream.
 *
 * PgnStream is used for reading PGN games from a QIODevice or a string.
 * It has its own input methods, and keeps track of the current line
 * number which can be used to report errors in the games. PgnStream
 * also has its own Chess::Board object, so that the same board can be
 * easily used with all the games in the stream. The chess variant can
 * be changed at any time, so it's possible to read PGN streams that
 * contain games of multiple variants.
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
		/*! The type of a PGN token. */
		enum TokenType
		{
			/*! Empty token (ie. nothing was read). */
			NoToken,
			/*! Move string in Standard Algebraic Notation. */
			PgnMove,
			/*! Move number before a full move. */
			PgnMoveNumber,
			/*!
			 * PGN tag.
			 * \note The token string does NOT contain the opening
			 * and closing square brackets.
			 */
			PgnTag,
			/*!
			 * PGN comment.
			 * \note The token string does NOT contain the opening
			 * and closing brackets.
			 */
			PgnComment,
			/*! One-line PGN comment. */
			PgnLineComment,
			/*! NAG code (Numeric Annotation Glyph). */
			PgnNag,
			/*! Game result. */
			PgnResult,
			/*! Unknown token. */
			Unknown
		};

		/*!
		 * Creates a new PgnStream.
		 *
		 * A device or a string must be set before the stream
		 * can be used.
		 */
		explicit PgnStream(const QString& variant = "standard");
		/*! Creates a PgnStream that operates on \a device. */
		explicit PgnStream(QIODevice* device,
				   const QString& variant = "standard");
		/*! Creates a PgnStream that operates on \a string. */
		explicit PgnStream(const QByteArray* string,
				   const QString& variant = "standard");

		/*! Destructs the PgnStream object. */
		~PgnStream();

		/*!
		 * Returns the Board object which is used to verify the moves
		 * and FEN strings in the stream.
		 */
		Chess::Board* board();

		/*! Returns the assigned device, or 0 if no device is in use. */
		QIODevice* device() const;
		/*! Sets the current device to \a device. */
		void setDevice(QIODevice* device);

		/*! Returns the assigned string, or 0 if no string is in use. */
		const QByteArray* string() const;
		/*! Sets the current string to \a string. */
		void setString(const QByteArray* string);

		/*! Returns the chess variant. */
		QString variant() const;
		/*!
		 * Sets the chess variant to \a variant.
		 * Returns true if successful; otherwise returns false.
		 */
		bool setVariant(const QString& variant);

		/*! Returns true if the stream is open. */
		bool isOpen() const;

		/*! Returns the current position in the stream. */
		qint64 pos() const;

		/*! Returns the current line number. */
		qint64 lineNumber() const;

		/*! Resets the stream to its default state. */
		void reset();

		/*! Reads one character and returns it. */
		char readChar();
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
		 * Rewinds back to the start of input.
		 * This is equivalent to calling \a seek(0).
		 */
		void rewind();
		/*!
		 * Seeks to position \a pos in the device, and sets the current
		 * line number to \a lineNumber.
		 * Returns true if successful; otherwise returns false.
		 */
		bool seek(qint64 pos, qint64 lineNumber = 1);

		/*! Returns the status of the stream. */
		Status status() const;

		/*!
		 * Seeks to the next game in the stream. Returns true if a game
		 * is available; otherwise returns false.
		 *
		 * This function must be called once for each new game, or
		 * nothing can be parsed.
		 *
		 * \sa readNext()
		 */
		bool nextGame();
		/*!
		 * Reads the next token and returns its type.
		 *
		 * Returns PgnStream::NoToken if a token is not available.
		 * \sa nextGame(), tokenType(), tokenString()
		 */
		TokenType readNext();

		/*!
		 * Returns the current token as string.
		 * \sa tokenType()
		 */
		QByteArray tokenString() const;
		/*!
		 * Returns the type of the current token.
		 * \sa tokenString()
		 */
		TokenType tokenType() const;

		/*! Returns the name of the current PGN tag. */
		QByteArray tagName() const;
		/*! Returns the value of the current PGN tag. */
		QByteArray tagValue() const;
	private:
		enum Phase
		{
			OutOfGame,
			InTags,
			InGame
		};

		void parseUntil(const char* chars);
		void parseTag();
		void parseComment(char opBracket);

		Chess::Board* m_board;
		qint64 m_pos;
		qint64 m_lineNumber;
		char m_lastChar;
		QByteArray m_tokenString;
		QByteArray m_tagName;
		QByteArray m_tagValue;
		TokenType m_tokenType;
		QIODevice* m_device;
		const QByteArray* m_string;
		Status m_status;
		Phase m_phase;
};

#endif // PGNSTREAM_H

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

#ifndef PGNFILE_H
#define PGNFILE_H

#include <QtGlobal>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "chessboard/chessboard.h"


/*!
 * \brief A file consisting of one or more chess games in PGN format
 *
 * PgnFile is used for reading games from a PGN file. It has its own
 * input methods, and keeps track of the current line number which can
 * be used to report errors in the file. PgnFile also has its own
 * Chess::Board object, so that the same board can be easily used with
 * all the games in the file.
 *
 * \sa PgnGame
 * \sa OpeningBook
 */
class LIB_EXPORT PgnFile
{
	public:
		/*! Creates a new empty PgnFile. */
		PgnFile();

		/*!
		 * Creates a new PgnFile and opens the file in ReadOnly mode.
		 * If the opening fails, isOpen() will return false.
		 *
		 * \param filename The name of the file which holds the PGN game(s)
		 * \param variant The chess variant for every game in the file.
		 * Games of other variants are ignored, or their moves are
		 * found to be illegal.
		 * Use \a Chess::NoVariant (the default) to allow multiple variants.
		 */
		explicit PgnFile(const QString& filename,
		                 Chess::Variant variant = Chess::Variant::NoVariant);
		
		/*!
		 * Returns the Board object which is used to verify the moves
		 * and FEN strings in the file.
		 */
		Chess::Board* board();
		
		/*! Returns true if the file is open. */
		bool isOpen() const;

		/*!
		 * Opens a PGN file.
		 * Returns true if successfull.
		 */
		bool open(const QString& filename);
		
		/*! Returns the current line number. */
		qint64 lineNumber() const;
		
		/*! Reads one character and returns it. */
		QChar readChar();
		
		/*! Reads one line of text and returns it. */
		QString readLine();
		
		/*! Rewinds back to the start of input. */
		void rewind();

		/*!
		 * Rewinds the file position by one character, which means that
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
		
		/*! Returns the status of the text stream. */
		QTextStream::Status status() const;
		
		/*! Returns the chess variant of the PGN game/collection. */
		Chess::Variant variant() const;

		/*! Sets the chess variant to \a variant. */
		void setVariant(Chess::Variant variant);
		
	private:
		QFile m_file;
		QTextStream m_in;
		Chess::Variant m_variant;
		Chess::Board m_board;
		qint64 m_lineNumber;
		QChar m_lastChar;
		bool m_rewind;
};

#endif // PGNFILE_H

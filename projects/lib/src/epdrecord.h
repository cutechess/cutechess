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

#ifndef EPDRECORD_H
#define EPDRECORD_H

#include <QStringList>
#include <QMap>
class QTextStream;

/*!
 * \brief An EPD (Extended Position Description) record
 *
 * EPD describes chess positions and operations that can be
 * performed at those positions. Normally EPD is used to build
 * test suites of challenging positions that a chess engine can
 * try to solve, but they can also be used as an opening suite
 * for picking different starting positions for a series of games.
 *
 * EPD at the Chess Programming Wiki:
 * http://chessprogramming.wikispaces.com/Extended+Position+Description
 *
 * At the moment the EpdRecord class can be used to parse individual
 * records from EPD files, but in the future it may also be used to
 * build new EPD test suites etc.
 *
 * \sa OpeningSuite
 */
class LIB_EXPORT EpdRecord
{
	public:
		/*! Creates a new empty EPD record. */
		EpdRecord();

		/*!
		 * Reads and parses a record from \a stream.
		 * Returns true if successful; otherwise returns false.
		 */
		bool parse(QTextStream& stream);
		/*!
		 * Returns true if the record contains an opcode that
		 * matches \a opcode; otherwise returns false.
		 */
		bool hasOpcode(const QString& opcode) const;
		/*! Returns the record's FEN string. */
		QString fen() const;
		/*!
		 * Returns the list of operands for \a opcode.
		 * An empty list is returned if the operation doesn't
		 * have any operands.
		 */
		QStringList operands(const QString& opcode) const;

	private:
		QString m_fen;
		QMap<QString, QStringList> m_operations;
};

#endif // EPDRECORD_H

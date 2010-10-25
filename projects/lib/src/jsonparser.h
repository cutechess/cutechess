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

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QVariant>

class QTextStream;


/*!
 * \brief A JSON (JavaScript Object Notation) parser.
 *
 * JsonParser parses JSON data from a text stream and
 * converts it into a QVariant.
 *
 * JSON specification: http://json.org/
 * \sa JsonSerializer
 */
class LIB_EXPORT JsonParser
{
	public:
		/*! Creates a new parser that reads data from \a stream. */
		JsonParser(QTextStream& stream);
		/*!
		 * Parses JSON data from the stream.
		 *
		 * Returns a null QVariant object if a parsing error occurs.
		 * Use isError() to check for errors.
		 */
		QVariant parse();

		/*! Returns true if a parsing error occured. */
		bool isError() const;
		/*! Returns a detailed description of the error. */
		QString errorMessage() const;
		/*! Returns the line number on which the error occured. */
		qint64 errorLineNumber() const;

	private:
		enum Token
		{
			JsonError,
			JsonGeneric,
			JsonNone,
			JsonComma,
			JsonColon,
			JsonBeginObject,
			JsonEndObject,
			JsonBeginArray,
			JsonEndArray,
			JsonTrue,
			JsonFalse,
			JsonNull,
			JsonNumber,
			JsonString
		};

		static QString tokenString(Token type,
					   const QString& str = QString());

		Token parseToken();
		QVariant parseValue(Token* tokenType = 0);
		QVariant parseObject();
		QVariant parseArray();
		QString parseString();
		void setError(const QString& message);
		void clearError();

		bool m_error;
		qint64 m_currentLine;
		qint64 m_errorLine;
		QString m_errorMessage;
		QString m_lastToken;
		QChar m_buffer;
		QTextStream& m_stream;
};

#endif // JSONPARSER_H

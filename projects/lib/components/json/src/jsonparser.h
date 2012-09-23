/*
    Copyright (c) 2010 Ilari Pihlajisto

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QVariant>
#include <QCoreApplication>

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
	Q_DECLARE_TR_FUNCTIONS(JsonParser)

	public:
		/*! Creates a new parser that reads data from \a stream. */
		JsonParser(QTextStream& stream);
		/*!
		 * Parses JSON data from the stream.
		 *
		 * Returns a null QVariant object if a parsing error occurs.
		 * Use hasError() to check for errors.
		 */
		QVariant parse();

		/*! Returns true if a parsing error occured. */
		bool hasError() const;
		/*! Returns a detailed description of the error. */
		QString errorString() const;
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
		QString m_errorString;
		QString m_lastToken;
		QChar m_buffer;
		QTextStream& m_stream;
};

#endif // JSONPARSER_H

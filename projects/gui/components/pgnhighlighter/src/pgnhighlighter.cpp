/*
    Copyright (c) 2010 Arto Jonsson

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

#include <QTextDocument>

#include "pgnhighlighter.h"

PgnHighlighter::PgnHighlighter(QTextDocument* document)
	: QSyntaxHighlighter(document)
{
	// default text styles
	QTextCharFormat commentFormat;
	commentFormat.setForeground(QColor(0, 255, 0));
	setFormatFor(Comment, commentFormat);

	QTextCharFormat tagFormat;
	tagFormat.setFontWeight(QFont::Bold);
	setFormatFor(Tag, tagFormat);

	QTextCharFormat stringFormat;
	stringFormat.setForeground(QColor(0, 0, 255));
	setFormatFor(String, stringFormat);

	QTextCharFormat moveNumberFormat;
	moveNumberFormat.setForeground(QColor(255, 0, 0));
	setFormatFor(MoveNumber, moveNumberFormat);

	QTextCharFormat resultFormat;
	resultFormat.setForeground(QColor(255, 0, 0));
	resultFormat.setFontWeight(QFont::Bold);
	setFormatFor(Result, resultFormat);
}

void PgnHighlighter::setFormatFor(Construct construct, const QTextCharFormat& format)
{
	m_formats[construct] = format;
	rehighlight();
}

QTextCharFormat PgnHighlighter::formatFor(Construct construct) const
{
	return m_formats[construct];
}

void PgnHighlighter::highlightBlock(const QString& text)
{
	int state = previousBlockState();
	int len = text.length();
	int start = 0;
	int pos = 0;

	while (pos < len)
	{
		QChar ch = text.at(pos);

		switch (state)
		{
			default:
			case NormalState:
				if (pos == 0 && ch == '%') {
					setFormat(pos, len, m_formats[Comment]);
					pos = len - 1; // end of line
				} else if (ch == ';') {
					setFormat(pos, len - pos, m_formats[Comment]);
					pos = len - 1; // end of line
				} else if (ch == '{') {
					start = pos;
					state = InComment;
				} else if (ch == '[') {
					start = pos;
					state = InTag;
				} else if (ch.isDigit()) {
					start = pos;

					if (text.mid(pos, 3) == "0-1" || text.mid(pos, 3) == "1-0") {
						pos += 3;
						setFormat(start, pos - start, m_formats[Result]);
						state = NormalState;
					} else if (text.mid(pos, 7) == "1/2-1/2") {
						pos += 7;
						setFormat(start, pos - start, m_formats[Result]);
						state = NormalState;
					} else if (ch.digitValue() >= 1) {
						state = InMoveNumber;
					}
				}
				break;

			case InComment:
				if (ch == '}') {
					setFormat(start, pos + 1 - start, m_formats[Comment]);
					state = NormalState;
				}
				break;

			case InTag:
				if (ch == ']') {
					setFormat(start, pos + 1 - start, m_formats[Tag]);
					state = NormalState;
				} else if (ch == '"') {
					setFormat(start, pos - start, m_formats[Tag]);
					start = pos;
					state = InString;
				}
				break;

			case InString:
				if (ch == '"') {
					setFormat(start, pos + 1 - start, m_formats[String]);
					start = pos + 1;
					state = InTag;
				}
				break;

			case InMoveNumber:
				if (ch == '.') {
					setFormat(start, pos + 1 - start, m_formats[MoveNumber]);
					state = NormalState;
				} else if (ch.isDigit()) {
					state = InMoveNumber;
				} else {
					state = NormalState;
				}

				break;

		}
		pos++;
	}
	if (state == InComment)
		setFormat(start, len - start, m_formats[Comment]);
	else if (state == InTag || state == InString || state == InMoveNumber)
		state = NormalState;

	setCurrentBlockState(state);
}

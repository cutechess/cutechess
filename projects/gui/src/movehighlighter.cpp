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

#include <QTextDocument>

#include "movehighlighter.h"

MoveHighlighter::MoveHighlighter(QTextDocument* document)
	: QSyntaxHighlighter(document)
{
	QTextCharFormat commentFormat;
	commentFormat.setForeground(QColor(7, 131, 7));
	setFormatFor(Comment, commentFormat);

	QTextCharFormat moveNumberFormat;
	moveNumberFormat.setFontWeight(QFont::Bold);
	setFormatFor(MoveNumber, moveNumberFormat);

	QTextCharFormat resultFormat;
	resultFormat.setFontWeight(QFont::Bold);
	setFormatFor(Result, resultFormat);
}

void MoveHighlighter::setFormatFor(Construct construct, const QTextCharFormat& format)
{
	m_formats[construct] = format;
	rehighlight();
}

QTextCharFormat MoveHighlighter::formatFor(Construct construct) const
{
	return m_formats[construct];
}

void MoveHighlighter::highlightBlock(const QString& text)
{
	int state = previousBlockState();
	int len = text.length();
	int start = 0;
	int pos = 0;
	int move = -1;
	MoveToken token;

	MoveData* blockData = new MoveData();
	blockData->curSide = -1;
	blockData->curMove = -1;

	/* Update the current move and side from previous block if possible.
	 * Comments for example can span multiple lines.
	 */ 
	if (currentBlock().previous().isValid())
	{
		MoveData* prevBlockData = dynamic_cast<MoveData *>(
			currentBlock().previous().userData());

		if (prevBlockData)
		{
			blockData->curMove = prevBlockData->curMove;
			blockData->curSide = prevBlockData->curSide;
		}
	}

	while (pos < len)
	{
		QChar ch = text.at(pos);

		switch (state)
		{
			default:
			case NormalState:
				start = pos;
				if (ch.isDigit())
				{
					if (text.mid(pos, 3) == "0-1" || text.mid(pos, 3) == "1-0")
					{
						pos += 2;
						setFormat(start, pos - start, m_formats[Result]);
						state = NormalState;
					}
					else if (text.mid(pos, 7) == "1/2-1/2")
					{
						pos += 6;
						setFormat(start, pos - start, m_formats[Result]);
						state = NormalState;
					} 
					else
					{
						move = ch.digitValue();
						state = InMoveNumber;
					}
				}
				else if (ch == '*')
				{
					setFormat(start, pos + 1 - start, m_formats[Result]);
					state = NormalState;
				}
				else if (ch == '{')
					state = InComment;
				else if (!ch.isSpace())
					state = InMove;
				break;

			case InComment:
				if (ch == '}')
				{
					token.type = Comment;
					token.side = blockData->curSide;
					token.move = blockData->curMove;
					blockData->tokens.insert(start, token);

					setFormat(start, pos + 1 - start, m_formats[Comment]);
					state = NormalState;
				}
				break;

			case InMoveNumber:
				if (ch == '.')
				{
					if (text.mid(pos, 3) == "...")
					{
						blockData->curSide = 0;
						pos += 2;
					}
					else
						blockData->curSide = 1;

					blockData->curMove = move;
					token.type = MoveNumber;
					token.side = blockData->curSide;
					token.move = move;
					blockData->tokens.insert(start, token);
					setFormat(start, pos + 1 - start, m_formats[MoveNumber]);
					state = NormalState;
				}
				else if (ch.isDigit())
				{
					move *= 10;
					move += ch.digitValue();
					state = InMoveNumber;
				}
				break;

			case InMove:
				if (ch == ' ')
				{
					blockData->curSide = !blockData->curSide;

					token.type = Move;
					token.side = blockData->curSide;
					token.move = blockData->curMove;
					blockData->tokens.insert(start, token);

					setFormat(start, pos - start, m_formats[Move]);
					state = NormalState;
				}
				break;
		}
		pos++;
	}
	if (state == InComment)
		setFormat(start, len - start, m_formats[Comment]);

	setCurrentBlockState(state);
	setCurrentBlockUserData(blockData);
}

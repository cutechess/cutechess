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

#ifndef MOVE_HIGHLIGHTER_H
#define MOVE_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QMap>

class QTextDocument;

class MoveHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	public:
		enum Construct
		{
			MoveNumber,
			Move,
			Result,
			Comment,
			LastConstrcut = Comment
		};

		struct MoveToken
		{
			Construct type;
			int side;
			int move;
		};

		struct MoveData : public QTextBlockUserData
		{
			QMap<int, MoveToken> tokens;
			int curMove;
			int curSide;
		};

		MoveHighlighter(QTextDocument* document);

		void setFormatFor(Construct construct, const QTextCharFormat& format);
		QTextCharFormat formatFor(Construct construct) const;

	protected:
		enum State
		{
			NormalState = -1,
			InMoveNumber,
			InMove,
			InComment
		};

		void highlightBlock(const QString& text);

	private:
		QTextCharFormat m_formats[LastConstrcut + 1];

};

#endif // MOVE_HIGHLIGHTER_H


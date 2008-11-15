/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "symbols.h"

const QString Symbols::BlackPawn = QString::fromUtf8("\342\231\237");
const QString Symbols::BlackRook = QString::fromUtf8("\342\231\234");
const QString Symbols::BlackKnight = QString::fromUtf8("\342\231\236");
const QString Symbols::BlackBishop = QString::fromUtf8("\342\231\235");
const QString Symbols::BlackQueen = QString::fromUtf8("\342\231\233");
const QString Symbols::BlackKing = QString::fromUtf8("\342\231\232");

const QString Symbols::WhitePawn = QString::fromUtf8("\342\231\231");
const QString Symbols::WhiteRook = QString::fromUtf8("\342\231\226");
const QString Symbols::WhiteKnight = QString::fromUtf8("\342\231\230");
const QString Symbols::WhiteBishop = QString::fromUtf8("\342\231\227");
const QString Symbols::WhiteQueen = QString::fromUtf8("\342\231\225");
const QString Symbols::WhiteKing = QString::fromUtf8("\342\231\224");

QString Symbols::symbolOf(Chess::Side side, Chess::Piece piece)
{
	QString symbol;

	switch (side)
	{
		case Chess::White:
		
			switch (piece)
			{
				case Chess::Pawn:
					symbol = Symbols::WhitePawn;
				break;

				case Chess::Rook:
					symbol = Symbols::WhiteRook;
				break;

				case Chess::Knight:
					symbol = Symbols::WhiteKnight;
				break;

				case Chess::Bishop:
					symbol = Symbols::WhiteBishop;
				break;

				case Chess::Queen:
					symbol = Symbols::WhiteBishop;
				break;

				case Chess::King:
					symbol = Symbols::WhiteKing;
				break;
				
				default:
					symbol = "";
				break;
			}
		break;

		case Chess::Black:
			
			switch (piece)
			{
				case Chess::Pawn:
					symbol = Symbols::WhitePawn;
				break;

				case Chess::Rook:
					symbol = Symbols::WhiteRook;
				break;

				case Chess::Knight:
					symbol = Symbols::WhiteKnight;
				break;

				case Chess::Bishop:
					symbol = Symbols::WhiteBishop;
				break;

				case Chess::Queen:
					symbol = Symbols::WhiteBishop;
				break;

				case Chess::King:
					symbol = Symbols::WhiteKing;
				break;
				
				default:
					symbol = "";
				break;
			}
		break;

		default:
			symbol = "";
		break;
	}

	return symbol;
}

QString Symbols::symbolOf(Chess::Side side, const QChar& piece)
{
	QString symbol;

	switch (side)
	{
		case Chess::White:

			if (piece.toLower() == QChar('k'))
				symbol = Symbols::WhiteKing;
			else if (piece.toLower() == QChar('q'))
				symbol = Symbols::WhiteQueen;
			else if (piece.toLower() == QChar('b'))
				symbol = Symbols::WhiteBishop;
			else if (piece.toLower() == QChar('n'))
				symbol = Symbols::WhiteKnight;
			else if (piece.toLower() == QChar('r'))
				symbol = Symbols::WhiteRook;
			else if (piece.toLower() == QChar('p'))
				symbol = Symbols::WhitePawn;
			else
				symbol = "";
		break;

		case Chess::Black:

			if (piece.toLower() == QChar('k'))
				symbol = Symbols::BlackKing;
			else if (piece.toLower() == QChar('q'))
				symbol = Symbols::BlackQueen;
			else if (piece.toLower() == QChar('b'))
				symbol = Symbols::BlackBishop;
			else if (piece.toLower() == QChar('n'))
				symbol = Symbols::BlackKnight;
			else if (piece.toLower() == QChar('r'))
				symbol = Symbols::BlackRook;
			else if (piece.toLower() == QChar('p'))
				symbol = Symbols::BlackPawn;
			else
				symbol = "";
		break;

		default:
			symbol = "";
		break;
	}

	return symbol;
}


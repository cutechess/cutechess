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

#include <QString>

#include "graphicschesspiece.h"


static QString pieceFileName(Chessboard::ChessSide side, Chessboard::ChessPiece piece)
{
	if (side == Chessboard::White) {
		switch (piece) {
		case Chessboard::Pawn:
			return ":/wpawn.svg";
		case Chessboard::Knight:
			return ":/wknight.svg";
		case Chessboard::Bishop:
			return ":/wbishop.svg";
		case Chessboard::Rook:
			return ":/wrook.svg";
		case Chessboard::Queen:
			return ":/wqueen.svg";
		case Chessboard::King:
			return ":/wking.svg";
		default:
			return "";
		}
	} else if (side == Chessboard::Black) {
		switch (piece) {
		case Chessboard::Pawn:
			return ":/bpawn.svg";
		case Chessboard::Knight:
			return ":/bknight.svg";
		case Chessboard::Bishop:
			return ":/bbishop.svg";
		case Chessboard::Rook:
			return ":/brook.svg";
		case Chessboard::Queen:
			return ":/bqueen.svg";
		case Chessboard::King:
			return ":/bking.svg";
		default:
			return "";
		}
	}
	
	return "";
}

GraphicsChessPiece::GraphicsChessPiece(Chessboard::ChessSide side,
                                       Chessboard::ChessPiece piece,
                                       QGraphicsItem* parent)
: QGraphicsSvgItem(pieceFileName(side, piece), parent)
{
	m_side = side;
	m_piece = piece;
}

Chessboard::ChessSide GraphicsChessPiece::side() const
{
	return m_side;
}

Chessboard::ChessPiece GraphicsChessPiece::piece() const
{
	return m_piece;
}

void GraphicsChessPiece::setPiece(Chessboard::ChessPiece piece)
{
	m_piece = piece;
}


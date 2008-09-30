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

GraphicsChessPiece::GraphicsChessPiece(Chessboard::ChessSide side,
                                       ChessPiece::PieceType piece,
                                       QGraphicsItem* parent)
	: QGraphicsSvgItem(parent)
{
	m_side = side;
	setPiece(piece);
}

Chessboard::ChessSide GraphicsChessPiece::side() const
{
	return m_side;
}

ChessPiece::PieceType GraphicsChessPiece::piece() const
{
	return m_piece;
}

void GraphicsChessPiece::setPiece(ChessPiece::PieceType piece)
{
	m_piece = piece;

	if (m_side == Chessboard::White)
	{
		switch (m_piece)
		{
			case ChessPiece::PT_Pawn:
				setElementId("wpawn");
			break;

			case ChessPiece::PT_Knight:
				setElementId("wknight");
			break;

			case ChessPiece::PT_Bishop:
				setElementId("wbishop");
			break;

			case ChessPiece::PT_Rook:
				setElementId("wrook");
			break;

			case ChessPiece::PT_Queen:
				setElementId("wqueen");
			break;

			case ChessPiece::PT_King:
				setElementId("wking");
			break;

			default:
			break;
		}
	}
	else if (m_side == Chessboard::Black)
	{
		switch (m_piece)
		{
			case ChessPiece::PT_Pawn:
				setElementId("bpawn");
			break;

			case ChessPiece::PT_Knight:
				setElementId("bknight");
			break;

			case ChessPiece::PT_Bishop:
				setElementId("bbishop");
			break;

			case ChessPiece::PT_Rook:
				setElementId("brook");
			break;

			case ChessPiece::PT_Queen:
				setElementId("bqueen");
			break;

			case ChessPiece::PT_King:
				setElementId("bking");
			break;

			default:
			break;
		}
	}
}

void GraphicsChessPiece::centerOnParent()
{
	Q_ASSERT(parentItem() != 0);

	// The size of the SVG image must be smaller than parent
	Q_ASSERT(sceneBoundingRect().width() <=
		parentItem()->sceneBoundingRect().width());

	Q_ASSERT(sceneBoundingRect().height() <=
		parentItem()->sceneBoundingRect().height());

	setPos((parentItem()->sceneBoundingRect().width() -
		sceneBoundingRect().width()) / 2.0,
		(parentItem()->sceneBoundingRect().height() -
		sceneBoundingRect().height()) / 2.0);
}


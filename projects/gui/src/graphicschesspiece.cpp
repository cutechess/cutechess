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

#include <QString>

#include "graphicschesspiece.h"

GraphicsChessPiece::GraphicsChessPiece(Chess::Side side,
                                       Chess::Piece piece,
                                       QGraphicsItem* parent)
	: QGraphicsSvgItem(parent),
	  m_side(side)
{
	setPiece(piece);
}

Chess::Side GraphicsChessPiece::side() const
{
	return m_side;
}

Chess::Piece GraphicsChessPiece::piece() const
{
	return m_piece;
}

void GraphicsChessPiece::setPiece(Chess::Piece piece)
{
	m_piece = piece;

	if (m_side == Chess::White)
	{
		switch (m_piece)
		{
			case Chess::Pawn:
				setElementId("wpawn");
			break;

			case Chess::Knight:
				setElementId("wknight");
			break;

			case Chess::Bishop:
				setElementId("wbishop");
			break;

			case Chess::Rook:
				setElementId("wrook");
			break;

			case Chess::Queen:
				setElementId("wqueen");
			break;

			case Chess::King:
				setElementId("wking");
			break;

			default:
			break;
		}
	}
	else if (m_side == Chess::Black)
	{
		switch (m_piece)
		{
			case Chess::Pawn:
				setElementId("bpawn");
			break;

			case Chess::Knight:
				setElementId("bknight");
			break;

			case Chess::Bishop:
				setElementId("bbishop");
			break;

			case Chess::Rook:
				setElementId("brook");
			break;

			case Chess::Queen:
				setElementId("bqueen");
			break;

			case Chess::King:
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

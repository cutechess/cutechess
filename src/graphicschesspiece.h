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

#ifndef GRAPHICSCHESSPIECE_H
#define GRAPHICSCHESSPIECE_H

#include <QGraphicsSvgItem>
#include "chessboard/chessboard.h"
#include "chessboard/chesspiece.h"

class GraphicsChessPiece : public QGraphicsSvgItem
{
	Q_OBJECT

	public:
		GraphicsChessPiece(Chessboard::ChessSide side,
		                   ChessPiece::PieceType piece,
		                   QGraphicsItem* parent = 0);

		Chessboard::ChessSide side() const;
		ChessPiece::PieceType piece() const;

		void setPiece(ChessPiece::PieceType piece);
		void centerOnParent();

	private:
		Chessboard::ChessSide m_side;
		ChessPiece::PieceType m_piece;

};

#endif // GRAPHICSCHESSPIECE_H


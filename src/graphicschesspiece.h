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

#ifndef GRAPHICSCHESSPIECE
#define GRAPHICSCHESSPIECE

#include <QGraphicsSvgItem>
#include "chessboard/chessboard.h"

class GraphicsChessPiece: public QGraphicsSvgItem
{
Q_OBJECT

public:
	GraphicsChessPiece(Chessboard::ChessSide side,
	                   Chessboard::ChessPiece piece,
	                   QGraphicsItem* parent = 0);

	Chessboard::ChessSide side() const;
	Chessboard::ChessPiece piece() const;

	void setPiece(Chessboard::ChessPiece piece);
	void centerOnParent();

private:
	Chessboard::ChessSide m_side;
	Chessboard::ChessPiece m_piece;
};

#endif // GRAPHICSCHESSPIECE


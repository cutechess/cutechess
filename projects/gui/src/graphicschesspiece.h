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

#ifndef GRAPHICSCHESSPIECE_H
#define GRAPHICSCHESSPIECE_H

#include <QGraphicsSvgItem>
#include <chessboard/chess.h>

class GraphicsChessPiece : public QGraphicsSvgItem
{
	Q_OBJECT

	public:
		GraphicsChessPiece(Chess::Side side,
		                   Chess::Piece piece,
		                   QGraphicsItem* parent = 0);

		Chess::Side side() const;
		Chess::Piece piece() const;

		void setPiece(Chess::Piece piece);
		void centerOnParent();

	private:
		Chess::Side m_side;
		Chess::Piece m_piece;

};

#endif // GRAPHICSCHESSPIECE_H


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

#include <QPainter>
#include <QSvgRenderer>
#include <../../lib/src/chessboard/chessmove.h>

#include "graphicschessboarditem.h"
#include "graphicschessboardsquareitem.h"
#include "graphicschesspiece.h"

const qreal GraphicsChessboardItem::size = 400;
const qreal GraphicsChessboardItem::borderSize = 25;

GraphicsChessboardItem::GraphicsChessboardItem(const QString& svgResource, QGraphicsItem* parent)
	: QObject(0), QGraphicsItem(parent)
{
	m_showBorder = true;
	m_borderColor = QColor("peru");
	
	m_renderer = new QSvgRenderer(svgResource);

	initChessboard();
	initChessPieces();
}

GraphicsChessboardItem::~GraphicsChessboardItem()
{
	delete m_renderer;
}

QRectF GraphicsChessboardItem::boundingRect() const
{
	if (isBorderVisible())
	{
		return QRectF(0, 0, GraphicsChessboardItem::size +
			(2 * GraphicsChessboardItem::borderSize), GraphicsChessboardItem::size +
			(2 * GraphicsChessboardItem::borderSize));
	}

	return QRectF(0, 0, GraphicsChessboardItem::size, GraphicsChessboardItem::size);
}

void GraphicsChessboardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	painter->fillRect(boundingRect(), m_borderColor);
}

void GraphicsChessboardItem::showBorder(bool visibility)
{
	m_showBorder = visibility;
}

bool GraphicsChessboardItem::isBorderVisible() const
{
	return m_showBorder;
}

void GraphicsChessboardItem::initChessboard()
{
	bool colorToggle = false;
	int row = 0;
	GraphicsChessboardSquareItem* square;

	for (int i = 0; i < 64; i++)
	{
		colorToggle = !colorToggle;

		if (colorToggle)
		{
			square = new GraphicsChessboardSquareItem(this,
				GraphicsChessboardSquareItem::LightSquare);
		}
		else
		{
			square = new GraphicsChessboardSquareItem(this,
				GraphicsChessboardSquareItem::DarkSquare);
		}

		square->setPositionInChessboard(i);
		
		if (isBorderVisible())
		{
			square->setPos(GraphicsChessboardSquareItem::size * (i % 8) +
				GraphicsChessboardItem::borderSize,
				GraphicsChessboardSquareItem::size * row +
				GraphicsChessboardItem::borderSize);
		}
		else
		{
			square->setPos(GraphicsChessboardSquareItem::size * (i % 8),
				GraphicsChessboardSquareItem::size * row);
		}

		m_squares[i] = square;
		
		if (i > 0 && (i + 1) % 8 == 0)
		{
			row++;
			colorToggle = !colorToggle;
		}
	}
}

void GraphicsChessboardItem::initChessPieces()
{
	// == BLACK PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Rook, m_squares[0]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Knight, m_squares[1]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Bishop, m_squares[2]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Queen, m_squares[3]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::King, m_squares[4]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Bishop, m_squares[5]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Knight, m_squares[6]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
		Chess::Rook, m_squares[7]));

	for (int i = 0; i < 8; i++)
	{
		m_pieces.push_back(new GraphicsChessPiece(Chess::Black,
			Chess::Pawn, m_squares[8 + i]));
	}

	// == WHITE PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Rook, m_squares[56]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Knight, m_squares[57]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Bishop, m_squares[58]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Queen, m_squares[59]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::King, m_squares[60]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Bishop, m_squares[61]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Knight, m_squares[62]));
	m_pieces.push_back(new GraphicsChessPiece(Chess::White,
		Chess::Rook, m_squares[63]));

	for (int i = 0; i < 8; i++)
	{
		m_pieces.push_back(new GraphicsChessPiece(Chess::White,
			Chess::Pawn, m_squares[48 + i]));
	}

	foreach (GraphicsChessPiece* piece, m_pieces)
	{
		// Associate the shared renderer with the chess pieces
		piece->setSharedRenderer(m_renderer);

		// Center the piece on its parent
		piece->centerOnParent();
	}
}

void GraphicsChessboardItem::makeMove(const Chess::Move& move)
{
	// HACK
	// Temporary solution until the internal and graphical board
	// are properly synchronized.
	int source = move.sourceSquare();
	int srcFile = (source % 10) - 1;
	int srcRank = (source / 10) - 2;
	source = srcRank * 8 + srcFile;
	
	int target = move.targetSquare();
	int trgFile = (target % 10) - 1;
	int trgRank = (target / 10) - 2;
	target = trgRank * 8 + trgFile;
	
	
	QList<QGraphicsItem*> sourceItems = m_squares[source]->childItems();
	QList<QGraphicsItem*> capturedItems = m_squares[target]->childItems();

	Q_ASSERT(!sourceItems.isEmpty());

	GraphicsChessPiece* piece = dynamic_cast<GraphicsChessPiece*>(sourceItems[0]);
	Q_ASSERT(piece != 0);

	// En passant capture
	if (capturedItems.isEmpty() && piece->piece() == Chess::Pawn) {
		if (piece->side() == Chess::White
		&&  target - source != -16
		&&  target - source != -8)
			capturedItems = m_squares[target + 8]->childItems();
		else if (piece->side() == Chess::Black
		     &&  target - source != 16
		     &&  target - source != 8)
			capturedItems = m_squares[target - 8]->childItems();
	}

	// Castling
	// HACK
	if (move.castlingSide() == 0)
	{
		foreach (QGraphicsItem* it, m_squares[target - 2]->childItems())
			it->setParentItem(m_squares[target + 1]);
	}
	else if (move.castlingSide() == 1)
	{
		foreach (QGraphicsItem* it, m_squares[target + 1]->childItems())
			it->setParentItem(m_squares[target - 1]);
	}

	// Promotion
	if (move.promotion() != Chess::NoPiece) {
		piece->setPiece((Chess::Piece)move.promotion());
		piece->centerOnParent();
	}

	// Hide all items in the target square if there's any
	if (!capturedItems.isEmpty())
	{
		foreach (QGraphicsItem* it, capturedItems) {
			it->hide();
			it->setParentItem(0);
		}
	}

	// Move all items from the source square to the target square
	foreach (QGraphicsItem* it, sourceItems)
		it->setParentItem(m_squares[target]);
}


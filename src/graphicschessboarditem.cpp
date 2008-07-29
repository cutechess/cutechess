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

#include "graphicschessboarditem.h"
#include "graphicschessboardsquareitem.h"
#include "graphicschesspiece.h"
#include "chessboard/chessmove.h"

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

		square->setPositionInChessboard(Chessboard::ChessSquare(i));
		
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
	// TODO: The pieces aren't centered within the square

	// == BLACK PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[0] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Rook, m_squares[0]);
	m_pieces[0]->setSharedRenderer(m_renderer);

	m_pieces[1] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Knight, m_squares[1]);
	m_pieces[1]->setSharedRenderer(m_renderer);

	m_pieces[2] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Bishop, m_squares[2]);
	m_pieces[2]->setSharedRenderer(m_renderer);

	m_pieces[3] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Queen, m_squares[3]);
	m_pieces[3]->setSharedRenderer(m_renderer);

	m_pieces[4] = new GraphicsChessPiece(Chessboard::Black, Chessboard::King, m_squares[4]);
	m_pieces[4]->setSharedRenderer(m_renderer);

	m_pieces[5] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Bishop, m_squares[5]);
	m_pieces[5]->setSharedRenderer(m_renderer);

	m_pieces[6] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Knight, m_squares[6]);
	m_pieces[6]->setSharedRenderer(m_renderer);

	m_pieces[7] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Rook, m_squares[7]);
	m_pieces[7]->setSharedRenderer(m_renderer);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[8 + i] = new GraphicsChessPiece(Chessboard::Black, Chessboard::Pawn, m_squares[8 + i]);
		m_pieces[8 + i]->setSharedRenderer(m_renderer);
	}

	// == WHITE PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[15] = new GraphicsChessPiece(Chessboard::White, Chessboard::Rook, m_squares[56]);
	m_pieces[15]->setSharedRenderer(m_renderer);

	m_pieces[16] = new GraphicsChessPiece(Chessboard::White, Chessboard::Knight, m_squares[57]);
	m_pieces[16]->setSharedRenderer(m_renderer);

	m_pieces[17] = new GraphicsChessPiece(Chessboard::White, Chessboard::Bishop, m_squares[58]);
	m_pieces[17]->setSharedRenderer(m_renderer);

	m_pieces[18] = new GraphicsChessPiece(Chessboard::White, Chessboard::Queen, m_squares[59]);
	m_pieces[18]->setSharedRenderer(m_renderer);

	m_pieces[19] = new GraphicsChessPiece(Chessboard::White, Chessboard::King, m_squares[60]);
	m_pieces[19]->setSharedRenderer(m_renderer);

	m_pieces[20] = new GraphicsChessPiece(Chessboard::White, Chessboard::Bishop, m_squares[61]);
	m_pieces[20]->setSharedRenderer(m_renderer);

	m_pieces[21] = new GraphicsChessPiece(Chessboard::White, Chessboard::Knight, m_squares[62]);
	m_pieces[21]->setSharedRenderer(m_renderer);

	m_pieces[22] = new GraphicsChessPiece(Chessboard::White, Chessboard::Rook, m_squares[63]);
	m_pieces[22]->setSharedRenderer(m_renderer);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[23 + i] = new GraphicsChessPiece(Chessboard::White, Chessboard::Pawn, m_squares[48 + i]);
		m_pieces[23 + i]->setSharedRenderer(m_renderer);
	}
}

void GraphicsChessboardItem::makeMove(const ChessMove& move)
{
	QList<QGraphicsItem*> sourceItems = m_squares[move.sourceSquare()]->childItems();
	QList<QGraphicsItem*> capturedItems = m_squares[move.targetSquare()]->childItems();

	Q_ASSERT(!sourceItems.isEmpty());

	GraphicsChessPiece* piece = dynamic_cast<GraphicsChessPiece*>(sourceItems[0]);
	Q_ASSERT(piece != 0);

	// En passant capture
	if (capturedItems.isEmpty() && piece->piece() == Chessboard::Pawn) {
		if (piece->side() == Chessboard::White
		&&  move.targetSquare() - move.sourceSquare() != -16
		&&  move.targetSquare() - move.sourceSquare() != -8)
			capturedItems = m_squares[move.targetSquare() + 8]->childItems();
		else if (piece->side() == Chessboard::Black
		     &&  move.targetSquare() - move.sourceSquare() != 16
		     &&  move.targetSquare() - move.sourceSquare() != 8)
			capturedItems = m_squares[move.targetSquare() - 8]->childItems();
	}

	// Castling
	if (piece->piece() == Chessboard::King) {
		if (piece->side() == Chessboard::White
		&&  move.sourceSquare() == Chessboard::E1) {
			if (move.targetSquare() == Chessboard::G1) {
				foreach (QGraphicsItem* it, m_squares[Chessboard::H1]->childItems())
					it->setParentItem(m_squares[Chessboard::F1]);
			} else if (move.targetSquare() == Chessboard::C1) {
				foreach (QGraphicsItem* it, m_squares[Chessboard::A1]->childItems())
					it->setParentItem(m_squares[Chessboard::D1]);
			}
		} else if (piece->side() == Chessboard::Black
		       &&  move.sourceSquare() == Chessboard::E8) {
			if (move.targetSquare() == Chessboard::G8) {
				foreach (QGraphicsItem* it, m_squares[Chessboard::H8]->childItems())
					it->setParentItem(m_squares[Chessboard::F8]);
			} else if (move.targetSquare() == Chessboard::C8) {
				foreach (QGraphicsItem* it, m_squares[Chessboard::A8]->childItems())
					it->setParentItem(m_squares[Chessboard::D8]);
			}
		}
	}

	// Promotion
	if (move.promotion() != Chessboard::NoPiece) {
		piece->setPiece(move.promotion());
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
		it->setParentItem(m_squares[move.targetSquare()]);
}


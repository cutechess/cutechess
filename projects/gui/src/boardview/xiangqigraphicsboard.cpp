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

#include "xiangqigraphicsboard.h"
#include <QApplication>
#include <QMargins>
#include <QPainter>
#include <QPalette>
#include <QPropertyAnimation>
#include <cmath>
#include <board/square.h>
#include "graphicspiece.h"

namespace {

class TargetHighlights : public QGraphicsObject
{
	public:
		TargetHighlights(QGraphicsItem* parentItem = nullptr)
			: QGraphicsObject(parentItem)
		{
			setFlag(ItemHasNoContents);
		}
		virtual QRectF boundingRect() const
		{
			return QRectF();
		}
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget)
		{
			Q_UNUSED(painter);
			Q_UNUSED(option);
			Q_UNUSED(widget);
		}
};

} // anonymous namespace

XiangqiGraphicsBoard::XiangqiGraphicsBoard(int files,
			     int ranks,
			     qreal squareSize,
			     QGraphicsItem* parent)
	: GraphicsBoard(files, ranks, squareSize, parent),
	  m_bgColor(QColor("#E49D61"))
{
	m_rect.setSize(QSizeF(m_squareSize * (m_files + 1),
			      m_squareSize * (m_ranks + 1)));
	m_rect.moveCenter(QPointF(0, 0));
}

XiangqiGraphicsBoard::~XiangqiGraphicsBoard()
{

}

QRectF XiangqiGraphicsBoard::boundingRect() const
{
	const auto margins = QMarginsF(0, 0, 0, 0);
	return m_rect.marginsAdded(margins);
}

int XiangqiGraphicsBoard::squareCol(qreal x) const
{
	return (x + m_rect.width() / 2.0 - m_squareSize / 2) / m_squareSize;
}

int XiangqiGraphicsBoard::squareRow(qreal y) const
{
	return (y + m_rect.height() / 2.0 - m_squareSize / 2) / m_squareSize;
}

QPointF XiangqiGraphicsBoard::topLeft() const
{
	return QPointF(m_rect.left() + m_squareSize, m_rect.top() + m_squareSize);
}

void XiangqiGraphicsBoard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	// set the background color
	QRectF rect(m_rect);
	const qreal rLeft = rect.left();
	rect.moveLeft(rLeft);
	painter->fillRect(rect, m_bgColor);

	// draw the green spots
	QPen pen(Qt::SolidLine);
	pen.setWidth(10);
	pen.setColor(Qt::green);
	painter->setPen(pen);

	qint16 gridSize = static_cast<qint16>(m_squareSize);
	int offsetX = -m_rect.width()/2.0;
	int offsetY = -m_rect.height()/2.0;
	painter->drawPoint(offsetX+gridSize-5,		offsetY+gridSize-5);
	painter->drawPoint(offsetX+gridSize*m_files+5,	offsetY+gridSize-5);
	painter->drawPoint(offsetX+gridSize-5,		offsetY+gridSize*m_ranks+5);
	painter->drawPoint(offsetX+gridSize*m_files+5,	offsetY+gridSize*m_ranks+5);

	// draw the outlines
	pen.setColor(Qt::black);
	pen.setWidth(4);
	painter->setPen(pen);
	painter->drawLine(offsetX+gridSize-5,		offsetY+gridSize-5,
			  offsetX+gridSize*m_files+5,	offsetY+gridSize-5);
	painter->drawLine(offsetX+gridSize-5,		offsetY+gridSize-5,
			  offsetX+gridSize-5,		offsetY+gridSize*m_ranks+5);
	painter->drawLine(offsetX+gridSize-5,		offsetY+gridSize*m_ranks+5,
			  offsetX+gridSize*m_files+5,	offsetY+gridSize*m_ranks+5);
	painter->drawLine(offsetX+gridSize*m_files+5,	offsetY+gridSize-5,
			  offsetX+gridSize*m_files+5,	offsetY+gridSize*m_ranks+5);

	// draw the vertical lines
	pen.setWidth(2);
	painter->setPen(pen);
	for (int i = 1; i <= m_files; i++)
	{
		if (m_files >= 9)
		{
			int y1 = int(m_ranks/2);
			int y2 = m_ranks - y1 + 1;
			painter->drawLine(offsetX+gridSize*i, offsetY+gridSize,
					  offsetX+gridSize*i, offsetY+gridSize*y1);
			painter->drawLine(offsetX+gridSize*i, offsetY+gridSize*y2,
					  offsetX+gridSize*i, offsetY+gridSize*m_ranks);
		}
		else
		{
			painter->drawLine(offsetX+gridSize*i,offsetY+gridSize,
					  offsetX+gridSize*i,offsetY+gridSize*m_ranks);
		}
	}

	// draw the river
	painter->drawLine(offsetX+gridSize,        offsetY+gridSize*5,
			  offsetX+gridSize,        offsetY+gridSize*6);
	painter->drawLine(offsetX+gridSize*m_files,offsetY+gridSize*5,
			  offsetX+gridSize*m_files,offsetY+gridSize*6);

	// draw the horizontal lines
	for (int i = 1; i <= m_ranks; i++)
	{
		painter->drawLine(offsetX+gridSize,   offsetY+gridSize*i,
				  offsetX+gridSize*m_files, offsetY+gridSize*i);
	}

	// draw the diagonal lines for the advisor
	int diagX1 = m_files / 2;
	int diagX2 = diagX1 + 2;
	int diagY1 = 1;
	int diagY2 = 3;
	painter->drawLine(offsetX+gridSize*diagX1,	offsetY+gridSize*diagY1,
			  offsetX+gridSize*diagX2,	offsetY+gridSize*diagY2);
	painter->drawLine(offsetX+gridSize*diagX2,	offsetY+gridSize*diagY1,
			  offsetX+gridSize*diagX1,	offsetY+gridSize*diagY2);

	diagY1 = m_ranks - 2;
	diagY2 = diagY1 + 2;
	painter->drawLine(offsetX+gridSize*diagX1,	offsetY+gridSize*diagY1,
			  offsetX+gridSize*diagX2,	offsetY+gridSize*diagY2);
	painter->drawLine(offsetX+gridSize*diagX2,	offsetY+gridSize*diagY1,
			  offsetX+gridSize*diagX1,	offsetY+gridSize*diagY2);

	if (m_files >= 9)
	{
		// write the Chinese characters in the river
		int fontSize = gridSize/2-5;
		painter->setFont(QFont("Arial", fontSize));
		QPointF pointA(offsetX+gridSize*2+(gridSize-fontSize)/2,
			       offsetY+gridSize*5.7);
		painter->drawText(pointA, "楚");
		QPointF pointB(offsetX+gridSize*3+(gridSize-fontSize)/2,
			       offsetY+gridSize*5.7);
		painter->drawText(pointB, "河");
		QPointF pointC(offsetX+gridSize*6+(gridSize-fontSize)/2,
			       offsetY+gridSize*5.7);
		painter->drawText(pointC, "汉");
		QPointF pointD(offsetX+gridSize*7+(gridSize-fontSize)/2,
			       offsetY+gridSize*5.7);
		painter->drawText(pointD, "界");

		// draw the polyline
		pen.setWidth(4);
		painter->setPen(pen);
		for (int j = 2; j > 0; j--)
		{
			for (int i = 1; i < 5; i++)  // polyline for the pawn
			{
				QPoint points1[] = {
					QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)-15),
					QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)-5),
					QPoint(offsetX+gridSize*(2*i-1)+15,offsetY+gridSize*(10-j*3)-5),
				};
				painter->drawPolyline(points1, 3);

				QPoint points2[] = {
					QPoint(offsetX+gridSize*(2*i+1)-15,offsetY+gridSize*(10-j*3)-5),
					QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)-5),
					QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)-15),
				};
				painter->drawPolyline(points2, 3);

				QPoint points3[] = {
					QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)+15),
					QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)+5),
					QPoint(offsetX+gridSize*(2*i-1)+15,offsetY+gridSize*(10-j*3)+5),
				};
				painter->drawPolyline(points3, 3);

				QPoint points4[] = {
					QPoint(offsetX+gridSize*(2*i+1)-15,offsetY+gridSize*(10-j*3)+5),
					QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)+5),
					QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)+15),
				};
				painter->drawPolyline(points4, 3);

				if (i < 3) // polyline for the cannon
				{
					int m = static_cast<int>(pow(2, 2*i-1));
					int n = static_cast<int>(pow(j+1, 2));
					QPoint points5[] = {
						QPoint(offsetX+gridSize*m-15,offsetY+gridSize*(n-1)-5),
						QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)-5),
						QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)-15)
					};
					painter->drawPolyline(points5, 3);

					QPoint points6[] = {
						QPoint(offsetX+gridSize*m+15,offsetY+gridSize*(n-1)-5),
						QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)-5),
						QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)-15)
					};
					painter->drawPolyline(points6, 3);

					QPoint points7[] = {
						QPoint(offsetX+gridSize*m-15,offsetY+gridSize*(n-1)+5),
						QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)+5),
						QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)+15),
					};
					painter->drawPolyline(points7, 3);

					QPoint points8[] = {
						QPoint(offsetX+gridSize*m+15,offsetY+gridSize*(n-1)+5),
						QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)+5),
						QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)+15),
					};
					painter->drawPolyline(points8, 3);
				}
			}
		}
	}

	// draw the mark for the board
	painter->setFont(QFont("Arial", gridSize/4));
	QString topText[] = {"1","2","3","4","5","6","7","8","9"};
	QString bottomText[] = {"一","二","三","四","五","六","七","八","九"};
	for (int i = 0; i < m_files; i++)
	{
		rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
			      m_rect.top() + 5, m_squareSize, m_coordSize);
		int strIndex = m_flipped ? m_files - i - 1 : i;
		auto textPtr = m_flipped ? bottomText : topText;
		painter->drawText(rect, Qt::AlignCenter, textPtr[strIndex]);

		rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
			      m_rect.bottom()-m_coordSize - 5, m_squareSize, m_coordSize);
		strIndex = m_files - strIndex - 1;
		textPtr = m_flipped ? topText : bottomText;
		painter->drawText(rect, Qt::AlignCenter, textPtr[strIndex]);
	}
}

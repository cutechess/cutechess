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

#include "graphicsboard.h"
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

GraphicsBoard::GraphicsBoard(int files,
			     int ranks,
                 QString variant,
			     qreal squareSize,
			     QGraphicsItem* parent)
	: QGraphicsItem(parent),
	  m_files(files),
	  m_ranks(ranks),
	  m_squareSize(squareSize),
	  m_coordSize(squareSize / 2.0),
	  m_lightColor(QColor(0xff, 0xce, 0x9e)),
	  m_darkColor(QColor(0xd1, 0x8b, 0x47)),
      m_bgColor(QColor("#E49D61")),
	  m_squares(files * ranks),
	  m_highlightAnim(nullptr),
      m_variant(variant),
	  m_flipped(false)
{
	Q_ASSERT(files > 0);
	Q_ASSERT(ranks > 0);
    const QList<QString> janggiVariantList =
            {"janggi", "janggicasual", "janggimodern", "janggitraditional"};
    m_janggiVariant = janggiVariantList.contains(m_variant);

    if(m_variant == "xiangqi" ||
       m_variant == "manchu" ||
       m_variant == "minixiangqi")
    {
        m_rect.setSize(QSizeF(squareSize * (files + 1),
                              squareSize * (ranks + 1)));
    }
    else if(m_janggiVariant)
    {
        m_rect.setSize(QSizeF(squareSize * (files + 1),
                              squareSize * (ranks + 1)));
    }
    else
    {
        m_rect.setSize(QSizeF(squareSize * files, squareSize * ranks));
    }

	m_rect.moveCenter(QPointF(0, 0));
	m_textColor = QApplication::palette().text().color();

	setCacheMode(DeviceCoordinateCache);
}

GraphicsBoard::~GraphicsBoard()
{
	delete m_highlightAnim;
}

int GraphicsBoard::type() const
{
	return Type;
}

QRectF GraphicsBoard::boundingRect() const
{
    if(m_variant == "xiangqi" ||
       m_variant == "manchu" ||
       m_variant == "minixiangqi")
    {
        const auto margins = QMarginsF(0, 0, 0, 0);
        return m_rect.marginsAdded(margins);
    }
    else if(m_janggiVariant)
    {
        const auto margins = QMarginsF(0, 0, 0, 0);
        return m_rect.marginsAdded(margins);
    }
    else
    {
        const auto margins = QMarginsF(m_coordSize, m_coordSize,
                           m_coordSize, m_coordSize);
        return m_rect.marginsAdded(margins);
    }
}

void GraphicsBoard::paint(QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget)
{
    if(m_variant == "xiangqi" ||
       m_variant == "manchu")
    {
        paintXiangqiBoard(painter, option, widget);
    }
    else if(m_variant == "minixiangqi")
    {
        paintMiniXiangqiBoard(painter, option, widget);
    }
    else if(m_janggiVariant)
    {
        paintJanggiBoard(painter, option, widget);
    }
    else
    {
        paintChessBoard(painter, option, widget);
    }
}

void GraphicsBoard::paintChessBoard(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect(m_rect.topLeft(), QSizeF(m_squareSize, m_squareSize));
    const qreal rLeft = rect.left();

    // paint squares
    for (int y = 0; y < m_ranks; y++)
    {
        rect.moveLeft(rLeft);
        for (int x = 0; x < m_files; x++)
        {
            if ((x % 2) == (y % 2))
                painter->fillRect(rect, m_lightColor);
            else
                painter->fillRect(rect, m_darkColor);
            rect.moveLeft(rect.left() + m_squareSize);
        }
        rect.moveTop(rect.top() + m_squareSize);
    }

    auto font = painter->font();
    font.setPointSizeF(font.pointSizeF() * 0.7);
    painter->setFont(font);
    painter->setPen(m_textColor);

    // paint file coordinates
    const QString alphabet = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < m_files; i++)
    {
        const qreal tops[] = {m_rect.top() - m_coordSize,
                              m_rect.bottom()};
        for (const auto top : tops)
        {
            rect = QRectF(m_rect.left() + (m_squareSize * i), top,
                          m_squareSize, m_coordSize);
            int file = m_flipped ? m_files - i - 1 : i;
            painter->drawText(rect, Qt::AlignCenter, alphabet[file]);
        }
    }

    // paint rank coordinates
    for (int i = 0; i < m_ranks; i++)
    {
        const qreal lefts[] = {m_rect.left() - m_coordSize,
                               m_rect.right()};
        for (const auto left : lefts)
        {
            rect = QRectF(left, m_rect.top() + (m_squareSize * i),
                          m_coordSize, m_squareSize);
            int rank = m_flipped ? i + 1 : m_ranks - i;
            const auto num = QString::number(rank);
            painter->drawText(rect, Qt::AlignCenter, num);
        }
    }
}

void GraphicsBoard::paintXiangqiBoard(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // set the background color
    QRectF rect(m_rect.topLeft(),
                QSizeF(m_squareSize*(m_files+1),
                       m_squareSize*(m_ranks+1)));
    const qreal rLeft = rect.left();
    rect.moveLeft(rLeft);
    painter->fillRect(rect, m_bgColor);

    // set the pen linetype to solid
    QPen pen(Qt::SolidLine);
    pen.setWidth(10);
    pen.setColor(Qt::green);
    painter->setPen(pen);

    //set the antialias feature
    painter->setRenderHint(QPainter::Antialiasing);
    qint16 gridSize = static_cast<qint16>(m_squareSize);
    //rect.moveTopLeft(m_rect.topLeft());
    int offsetX = -m_rect.width()/2.0;
    int offsetY = -m_rect.height()/2.0;
    //initialize the for green dot coord
    painter->drawPoint(offsetX+gridSize-5,  offsetY+gridSize-5);
    painter->drawPoint(offsetX+gridSize*9+5,offsetY+gridSize-5);
    painter->drawPoint(offsetX+gridSize-5,  offsetY+gridSize*10+5);
    painter->drawPoint(offsetX+gridSize*9+5,offsetY+gridSize*10+5);
    //set the pen color and linewidth
    pen.setColor(Qt::black);
    pen.setWidth(4);
    painter->setPen(pen);
    // 1.draw the outline(bold line)
    painter->drawLine(offsetX+gridSize-5,   offsetY+gridSize-5,
                      offsetX+gridSize*9+5, offsetY+gridSize-5);
    painter->drawLine(offsetX+gridSize-5,   offsetY+gridSize-5,
                      offsetX+gridSize-5,   offsetY+gridSize*10+5);
    painter->drawLine(offsetX+gridSize-5,   offsetY+gridSize*10+5,
                      offsetX+gridSize*9+5, offsetY+gridSize*10+5);
    painter->drawLine(offsetX+gridSize*9+5, offsetY+gridSize-5,
                      offsetX+gridSize*9+5, offsetY+gridSize*10+5);

    // 2.draw the 18 vertical line
    pen.setWidth(2);
    painter->setPen(pen);

    for (int i=1; i<=m_files; i++)
    {
        painter->drawLine(offsetX+gridSize*i, offsetY+gridSize,
                          offsetX+gridSize*i, offsetY+gridSize*m_ranks/2);
        painter->drawLine(offsetX+gridSize*i, offsetY+gridSize*6,
                          offsetX+gridSize*i, offsetY+gridSize*m_ranks);
    }

    // 3.draw the river
    painter->drawLine(offsetX+gridSize,        offsetY+gridSize*5,
                      offsetX+gridSize,        offsetY+gridSize*6);
    painter->drawLine(offsetX+gridSize*m_files,offsetY+gridSize*5,
                      offsetX+gridSize*m_files,offsetY+gridSize*6);

    // 4.draw the 10 horizontal line
    for(int i=1;i<=m_ranks; i++)
    {
        painter->drawLine(offsetX+gridSize,   offsetY+gridSize*i,
                          offsetX+gridSize*m_files, offsetY+gridSize*i);
    }

    // 5.draw the diagonal line for the advisor
    painter->drawLine(offsetX+gridSize*4, offsetY+gridSize,
                      offsetX+gridSize*6, offsetY+gridSize*3);
    painter->drawLine(offsetX+gridSize*6, offsetY+gridSize,
                      offsetX+gridSize*4, offsetY+gridSize*3);
    painter->drawLine(offsetX+gridSize*4, offsetY+gridSize*8,
                      offsetX+gridSize*6, offsetY+gridSize*10);
    painter->drawLine(offsetX+gridSize*6, offsetY+gridSize*8,
                      offsetX+gridSize*4, offsetY+gridSize*10);

    // 6. write the Chinese Character in the River
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

    int crfontSize = gridSize/4-5;
    painter->setFont(QFont("Arial", crfontSize));
    QPointF pointE(offsetX+gridSize*4.2,
                   offsetY+gridSize*5.6);
    painter->drawText(pointE, "Albert Contribute");

    // 7. draw the polyline
    pen.setWidth(4);
    painter->setPen(pen);
    for(int j=2; j>0; j--)
    {
        for(int i=1; i<5; i++)  // the polyline for the pawn
        {
            QPoint points1[3] = {
                QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)-15),
                QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)-5),
                QPoint(offsetX+gridSize*(2*i-1)+15,offsetY+gridSize*(10-j*3)-5),
            };
            painter->drawPolyline(points1, 3);

            QPoint points2[3] = {
                QPoint(offsetX+gridSize*(2*i+1)-15,offsetY+gridSize*(10-j*3)-5),
                QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)-5),
                QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)-15),
            };
            painter->drawPolyline(points2, 3);

            QPoint points3[3] = {
                QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)+15),
                QPoint(offsetX+gridSize*(2*i-1)+5, offsetY+gridSize*(10-j*3)+5),
                QPoint(offsetX+gridSize*(2*i-1)+15,offsetY+gridSize*(10-j*3)+5),
            };
            painter->drawPolyline(points3, 3);

            QPoint points4[3] = {
                QPoint(offsetX+gridSize*(2*i+1)-15,offsetY+gridSize*(10-j*3)+5),
                QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)+5),
                QPoint(offsetX+gridSize*(2*i+1)-5, offsetY+gridSize*(10-j*3)+15),
            };
            painter->drawPolyline(points4, 3);

            if(i < 3) // // the polyline for the cannon
            {
                int m = static_cast<int>(pow(2, 2*i-1));
                int n = static_cast<int>(pow(j+1, 2));
                QPoint points5[3] = {
                    QPoint(offsetX+gridSize*m-15,offsetY+gridSize*(n-1)-5),
                    QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)-5),
                    QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)-15)
                };
                painter->drawPolyline(points5, 3);

                QPoint points6[3] = {
                    QPoint(offsetX+gridSize*m+15,offsetY+gridSize*(n-1)-5),
                    QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)-5),
                    QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)-15)
                };
                painter->drawPolyline(points6, 3);

                QPoint points7[3] = {
                    QPoint(offsetX+gridSize*m-15,offsetY+gridSize*(n-1)+5),
                    QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)+5),
                    QPoint(offsetX+gridSize*m-5, offsetY+gridSize*(n-1)+15),
                };
                painter->drawPolyline(points7, 3);

                QPoint points8[3] = {
                    QPoint(offsetX+gridSize*m+15,offsetY+gridSize*(n-1)+5),
                    QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)+5),
                    QPoint(offsetX+gridSize*m+5, offsetY+gridSize*(n-1)+15),
                };
                painter->drawPolyline(points8, 3);
            }
        }
    }

    // 8.draw the mark for the board
    painter->setFont(QFont("Arial", gridSize/4));
    QString str[9] = {"一","二","三","四","五","六","七","八","九"};
    for (int i = 0; i < m_files; i++)
    {
        if(m_flipped)
        {
            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.top() + 5, m_squareSize, m_coordSize);
            int fileTop = m_flipped ? i : m_files - i - 1;
            painter->drawText(rect, Qt::AlignCenter, str[fileTop]);

            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.bottom()-m_coordSize - 5, m_squareSize, m_coordSize);
            int fileBot = m_flipped ? m_files - i : i + 1;
            const auto numBot = QString::number(fileBot);
            painter->drawText(rect, Qt::AlignCenter, numBot);
        }
        else
        {
            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.top() + 5, m_squareSize, m_coordSize);
            int fileTop = m_flipped ? m_files - i : i + 1;
            const auto numTop = QString::number(fileTop);
            painter->drawText(rect, Qt::AlignCenter, numTop);

            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.bottom()-m_coordSize-5, m_squareSize, m_coordSize);
            int fileBot = m_flipped ? i : m_files - i - 1;
            painter->drawText(rect, Qt::AlignCenter, str[fileBot]);
        }
    }
}

void GraphicsBoard::paintMiniXiangqiBoard(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // set the background color
    QRectF rect(m_rect.topLeft(),
                QSizeF(m_squareSize*(m_files+1),
                       m_squareSize*(m_ranks+1)));
    const qreal rLeft = rect.left();
    rect.moveLeft(rLeft);
    painter->fillRect(rect, m_bgColor);

    // set the pen linetype to solid
    QPen pen(Qt::SolidLine);
    pen.setWidth(10);
    pen.setColor(Qt::green);
    painter->setPen(pen);

    // set the antialias feature
    painter->setRenderHint(QPainter::Antialiasing);
    qint16 gridSize = static_cast<qint16>(m_squareSize);
    int offsetX = -m_rect.width()/2.0;
    int offsetY = -m_rect.height()/2.0;
    // initialize the four green dot
    painter->drawPoint(offsetX+gridSize-5,        offsetY+gridSize-5);
    painter->drawPoint(offsetX+gridSize*m_files+5,offsetY+gridSize-5);
    painter->drawPoint(offsetX+gridSize-5,        offsetY+gridSize*m_ranks+5);
    painter->drawPoint(offsetX+gridSize*m_files+5,offsetY+gridSize*m_ranks+5);
    // set the pen color and linewidth
    pen.setColor(Qt::black);
    pen.setWidth(4);
    painter->setPen(pen);
    // 1.draw the outline(bold line)
    painter->drawLine(offsetX+gridSize-5,        offsetY+gridSize-5,
                      offsetX+gridSize*m_files+5,offsetY+gridSize-5);
    painter->drawLine(offsetX+gridSize-5,        offsetY+gridSize-5,
                      offsetX+gridSize-5,        offsetY+gridSize*m_ranks+5);
    painter->drawLine(offsetX+gridSize-5,        offsetY+gridSize*m_ranks+5,
                      offsetX+gridSize*m_files+5,offsetY+gridSize*m_ranks+5);
    painter->drawLine(offsetX+gridSize*m_files+5,offsetY+gridSize-5,
                      offsetX+gridSize*m_files+5,offsetY+gridSize*m_ranks+5);

    // 2.draw the 7 horizontal line
    pen.setWidth(2);
    painter->setPen(pen);
    for(int i=1;i<=m_ranks; i++)
    {
        painter->drawLine(offsetX+gridSize,        offsetY+gridSize*i,
                          offsetX+gridSize*m_files,offsetY+gridSize*i);
    }

    // 3.draw the 7 vertical line
    for(int i=1;i<=m_files; i++)
    {
        painter->drawLine(offsetX+gridSize*i,offsetY+gridSize,
                          offsetX+gridSize*i,offsetY+gridSize*m_ranks);
    }
    // 4.draw the diagonal line for the advisor
    painter->drawLine(offsetX+gridSize*3, offsetY+gridSize,
                      offsetX+gridSize*5, offsetY+gridSize*3);
    painter->drawLine(offsetX+gridSize*5, offsetY+gridSize,
                      offsetX+gridSize*3, offsetY+gridSize*3);
    painter->drawLine(offsetX+gridSize*3, offsetY+gridSize*5,
                      offsetX+gridSize*5, offsetY+gridSize*m_ranks);
    painter->drawLine(offsetX+gridSize*5, offsetY+gridSize*5,
                      offsetX+gridSize*3, offsetY+gridSize*m_ranks);

    // 5.draw the makr for the board
    painter->setFont(QFont("Arial", gridSize/4));
    QString str[7] = {"一","二","三","四","五","六","七"};
    for (int i = 0; i < m_files; i++)
    {
        if(m_flipped)
        {
            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.top() + 5, m_squareSize, m_coordSize);
            int fileTop = m_flipped ? i : m_files - i - 1;
            painter->drawText(rect, Qt::AlignCenter, str[fileTop]);

            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.bottom()-m_coordSize-5, m_squareSize, m_coordSize);
            int fileBot = m_flipped ? m_files - i : i + 1;
            const auto numBot = QString::number(fileBot);
            painter->drawText(rect, Qt::AlignCenter, numBot);
        }
        else
        {
            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.top() + 5, m_squareSize, m_coordSize);
            int fileTop = m_flipped ? m_files - i : i + 1;
            const auto numTop = QString::number(fileTop);
            painter->drawText(rect, Qt::AlignCenter, numTop);

            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i),
                          m_rect.bottom()-m_coordSize-5, m_squareSize, m_coordSize);
            int fileBot = m_flipped ? i : m_files - i - 1;
            painter->drawText(rect, Qt::AlignCenter, str[fileBot]);
        }
    }
}

void GraphicsBoard::paintJanggiBoard(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // set the background color
    QRectF rect(m_rect.topLeft(),
                QSizeF(m_squareSize*(m_files+1),
                       m_squareSize*(m_ranks+1)));
    const qreal rLeft = rect.left();
    rect.moveLeft(rLeft);
    painter->fillRect(rect, m_bgColor);

    // set the pen linetype to solid
    QPen pen(Qt::SolidLine);
    pen.setWidth(10);
    pen.setColor(Qt::green);
    painter->setPen(pen);

    // set the antialias feature
    painter->setRenderHint(QPainter::Antialiasing);
    qint16 gridSize = static_cast<qint16>(m_squareSize);
    int offsetX = -m_rect.width()/2.0;
    int offsetY = -m_rect.height()/2.0;
    // initialize the four green dot coord
    painter->drawPoint(offsetX+gridSize-5,        offsetY+gridSize-5);
    painter->drawPoint(offsetX+gridSize*m_files+5,offsetY+gridSize-5);
    painter->drawPoint(offsetX+gridSize-5,        offsetY+gridSize*m_ranks+5);
    painter->drawPoint(offsetX+gridSize*m_files+5,offsetY+gridSize*m_ranks+5);

    //set the pen color and linewidth
    pen.setColor(Qt::black);
    pen.setWidth(4);
    painter->setPen(pen);
    // 1.draw the outline(bold line)
    painter->drawLine(offsetX+gridSize-5,        offsetY+gridSize-5,
                      offsetX+gridSize*m_files+5,offsetY+gridSize-5);
    painter->drawLine(offsetX+gridSize-5,        offsetY+gridSize-5,
                      offsetX+gridSize-5,        offsetY+gridSize*m_ranks+5);
    painter->drawLine(offsetX+gridSize-5,        offsetY+gridSize*m_ranks+5,
                      offsetX+gridSize*m_files+5,offsetY+gridSize*m_ranks+5);
    painter->drawLine(offsetX+gridSize*m_files+5,offsetY+gridSize-5,
                      offsetX+gridSize*m_files+5,offsetY+gridSize*m_ranks+5);

    // 2.draw the 10 horizontal line
    pen.setWidth(2);
    painter->setPen(pen);
    for(int i=1;i<=m_ranks; i++)
    {
        painter->drawLine(offsetX+gridSize,        offsetY+gridSize*i,
                          offsetX+gridSize*m_files,offsetY+gridSize*i);
    }

    // 2.draw the 10 vertical line
    for(int i=1;i<=m_files; i++)
    {
        painter->drawLine(offsetX+gridSize*i,offsetY+gridSize,
                          offsetX+gridSize*i,offsetY+gridSize*m_ranks);
    }
    // 4.draw the diagonal line for the advisor
    painter->drawLine(offsetX+gridSize*((m_files+1)/2-1), offsetY+gridSize,
                      offsetX+gridSize*((m_files+1)/2+1), offsetY+gridSize*3);
    painter->drawLine(offsetX+gridSize*((m_files+1)/2+1), offsetY+gridSize,
                      offsetX+gridSize*((m_files+1)/2-1), offsetY+gridSize*3);
    painter->drawLine(offsetX+gridSize*((m_files+1)/2-1), offsetY+gridSize*(m_ranks-2),
                      offsetX+gridSize*((m_files+1)/2+1), offsetY+gridSize*m_ranks);
    painter->drawLine(offsetX+gridSize*((m_files+1)/2+1), offsetY+gridSize*(m_ranks-2),
                      offsetX+gridSize*((m_files+1)/2-1), offsetY+gridSize*m_ranks);

    // 5.draw the mark
    painter->setFont(QFont("Arial", gridSize/4));

    // paint file coordinates
    const QString alphabet = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < m_files; i++)
    {
        const qreal tops[] = {m_rect.top(),
                              m_rect.bottom()-m_coordSize};
        for (const auto top : tops)
        {
            rect = QRectF(m_rect.left() + m_coordSize + (m_squareSize * i), top,
                          m_squareSize, m_coordSize);
            int file = m_flipped ? m_files - i - 1 : i;
            painter->drawText(rect, Qt::AlignCenter, alphabet[file]);
        }
    }

    // paint rank coordinates
    for (int i = 0; i < m_ranks; i++)
    {
        const qreal lefts[] = {m_rect.left(),
                               m_rect.right()-m_coordSize};
        for (const auto left : lefts)
        {
            rect = QRectF(left, m_rect.top()+m_coordSize + (m_squareSize * i),
                          m_coordSize, m_squareSize);
            int rank = m_flipped ? i + 1 : m_ranks - i;
            if(rank == 10)
                rank = 0;
            const auto num = QString::number(rank);
            painter->drawText(rect, Qt::AlignCenter, num);
        }
    }
}

Chess::Square GraphicsBoard::squareAt(const QPointF& point) const
{
	if (!m_rect.contains(point))
		return Chess::Square();

    int offsetX = m_rect.width()/2.0;
    int offsetY = m_rect.height()/2.0;
    int col = 0, row = 0;

    if(m_variant == "xiangqi" ||
       m_variant == "manchu" ||
       m_variant == "minixiangqi")
    {
        col = (point.x() + offsetX - m_squareSize / 2) / m_squareSize;
        row = (point.y() + offsetY - m_squareSize / 2) / m_squareSize;
    }
    else if(m_janggiVariant)
    {
        col = (point.x() + offsetX - m_squareSize / 2) / m_squareSize;
        row = (point.y() + offsetY - m_squareSize / 2) / m_squareSize;
    }
    else
    {
        col = (point.x() + offsetX) / m_squareSize;
        row = (point.y() + offsetY) / m_squareSize;
    }

	if (m_flipped)
		return Chess::Square(m_files - col - 1, row);
	return Chess::Square(col, m_ranks - row - 1);
}

QPointF GraphicsBoard::squarePos(const Chess::Square& square) const
{
	if (!square.isValid())
		return QPointF();

    qreal x=0, y=0;

    if(m_variant == "xiangqi" ||
       m_variant == "manchu" ||
       m_variant == "minixiangqi")
    {
        x = m_rect.left() + m_squareSize;
        y = m_rect.top() + m_squareSize;
    }
    else if(m_janggiVariant)
    {
        x = m_rect.left() + m_squareSize;
        y = m_rect.top() + m_squareSize;
    }
    else
    {
        x = m_rect.left() + m_squareSize / 2;
        y = m_rect.top() + m_squareSize / 2;
    }

	if (m_flipped)
	{
		x += m_squareSize * (m_files - square.file() - 1);
		y += m_squareSize * square.rank();
	}
	else
	{
		x += m_squareSize * square.file();
		y += m_squareSize * (m_ranks - square.rank() - 1);
	}

	return QPointF(x, y);
}

Chess::Piece GraphicsBoard::pieceTypeAt(const Chess::Square& square) const
{
	GraphicsPiece* piece = pieceAt(square);
	if (piece == nullptr)
		return Chess::Piece();
	return piece->pieceType();
}

GraphicsPiece* GraphicsBoard::pieceAt(const Chess::Square& square) const
{
	if (!square.isValid())
		return nullptr;

	GraphicsPiece* piece = m_squares.at(squareIndex(square));
	Q_ASSERT(piece == nullptr || piece->container() == this);
	return piece;
}

GraphicsPiece* GraphicsBoard::takePieceAt(const Chess::Square& square)
{
	int index = squareIndex(square);
	if (index == -1)
		return nullptr;

	GraphicsPiece* piece = m_squares.at(index);
	if (piece == nullptr)
		return nullptr;

	m_squares[index] = nullptr;
	piece->setParentItem(nullptr);
	piece->setContainer(nullptr);

	return piece;
}

void GraphicsBoard::clearSquares()
{
	qDeleteAll(m_squares);
	m_squares.clear();
}

void GraphicsBoard::setSquare(const Chess::Square& square, GraphicsPiece* piece)
{
	Q_ASSERT(square.isValid());

	int index = squareIndex(square);
	delete m_squares[index];

	if (piece == nullptr)
		m_squares[index] = nullptr;
	else
	{
		m_squares[index] = piece;
		piece->setContainer(this);
		piece->setParentItem(this);
		piece->setPos(squarePos(square));
	}
}

void GraphicsBoard::movePiece(const Chess::Square& source,
			      const Chess::Square& target)
{
	GraphicsPiece* piece = pieceAt(source);
	Q_ASSERT(piece != nullptr);

	m_squares[squareIndex(source)] = nullptr;
	setSquare(target, piece);
}

int GraphicsBoard::squareIndex(const Chess::Square& square) const
{
	if (!square.isValid())
		return -1;

	return square.rank() * m_files + square.file();
}

void GraphicsBoard::clearHighlights()
{
	if (m_highlightAnim != nullptr)
	{
		m_highlightAnim->setDirection(QAbstractAnimation::Backward);
		m_highlightAnim->start(QAbstractAnimation::DeleteWhenStopped);
		m_highlightAnim = nullptr;
	}
}

void GraphicsBoard::setHighlights(const QList<Chess::Square>& squares)
{
	clearHighlights();
	if (squares.isEmpty())
		return;

	TargetHighlights* targets = new TargetHighlights(this);

	QRectF rect;
	rect.setSize(QSizeF(m_squareSize / 3, m_squareSize / 3));
	rect.moveCenter(QPointF(0, 0));
	QPen pen(Qt::white, m_squareSize / 20);
	QBrush brush(Qt::black);

	for (const auto& sq : squares)
	{
		QGraphicsEllipseItem* dot = new QGraphicsEllipseItem(rect, targets);

		dot->setCacheMode(DeviceCoordinateCache);
		dot->setPen(pen);
		dot->setBrush(brush);
		dot->setPos(squarePos(sq));
	}

	m_highlightAnim = new QPropertyAnimation(targets, "opacity");
	targets->setParent(m_highlightAnim);

	m_highlightAnim->setStartValue(0.0);
	m_highlightAnim->setEndValue(1.0);
	m_highlightAnim->setDuration(500);
	m_highlightAnim->setEasingCurve(QEasingCurve::InOutQuad);
	m_highlightAnim->start(QAbstractAnimation::KeepWhenStopped);
}

bool GraphicsBoard::isFlipped() const
{
	return m_flipped;
}

void GraphicsBoard::setFlipped(bool flipped)
{
	if (flipped == m_flipped)
		return;

	clearHighlights();
	m_flipped = flipped;
	update();
}

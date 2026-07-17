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




#include "capturedpieceswidget.h"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSvgRenderer>
#include <QMap>
#include <memory>

#include "boardview/graphicspiecereserve.h"
#include "boardview/graphicspiece.h"

#include <chessgame.h>
#include <board/boardfactory.h>
#include <board/board.h>

namespace {
const qreal s_squareSize = 24; // smaller thumbnails
const qreal s_spacing = 4;
const qreal s_rowSpacing = 15;
}

CapturedPiecesWidget::CapturedPiecesWidget(QWidget* parent)
    : QWidget(parent), m_game(nullptr)
{
    QVBoxLayout* l = new QVBoxLayout(this);

    m_view = new QGraphicsView(this);
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_scene = new QGraphicsScene(this);
    m_view->setScene(m_scene);

    l->addWidget(m_view);
    setLayout(l);
}

void CapturedPiecesWidget::setGame(ChessGame* game)
{
    if (m_game)
        disconnect(m_game, nullptr, this, nullptr);

    m_game = game;

    if (m_game)
    {
        connect(m_game, SIGNAL(moveMade(Chess::GenericMove,QString,QString)),
                this, SLOT(updateCapturedPieces()));
        connect(m_game, SIGNAL(fenChanged(QString)),
                this, SLOT(updateCapturedPieces()));
    }

    updateCapturedPieces();
}

void CapturedPiecesWidget::updateCapturedPieces()
{
    m_scene->clear();

    if (!m_game)
        return;

    Chess::Board* board = m_game->board();
    if (!board)
        return;

    QSvgRenderer* renderer = new QSvgRenderer(QString(":/default.svg"), m_scene);

    // Build captured piece lists (each element is the captured piece itself)
    QVector<Chess::Piece> whiteCaptured; // pieces captured by White (these are Black pieces)
    QVector<Chess::Piece> blackCaptured; // pieces captured by Black (these are White pieces)

    if (board->variantHasDrops())
    {
        const auto types = board->reservePieceTypes();
        for (const auto& ptype : types)
        {
            int cntWhite = board->reserveCount(Chess::Piece(Chess::Side::White, ptype.type()));
            int cntBlack = board->reserveCount(Chess::Piece(Chess::Side::Black, ptype.type()));
            for (int i = 0; i < cntWhite; ++i)
                whiteCaptured.append(Chess::Piece(Chess::Side::Black, ptype.type()));
            for (int i = 0; i < cntBlack; ++i)
                blackCaptured.append(Chess::Piece(Chess::Side::White, ptype.type()));
        }
    }
    else
    {
        std::unique_ptr<Chess::Board> startBoard(Chess::BoardFactory::create(board->variant()));
        if (startBoard)
        {
            startBoard->initialize();
            startBoard->reset();
        }

        QMap<int,int> startCounts[2];
        QMap<int,int> currCounts[2];

        if (startBoard)
        {
            for (int y = 0; y < startBoard->height(); ++y)
                for (int x = 0; x < startBoard->width(); ++x)
                {
                    Chess::Square sq(x, y);
                    Chess::Piece pc = startBoard->pieceAt(sq);
                    if (pc.isValid())
                        startCounts[pc.side()][pc.type()]++;
                }
        }

        for (int y = 0; y < board->height(); ++y)
            for (int x = 0; x < board->width(); ++x)
            {
                Chess::Square sq(x, y);
                Chess::Piece pc = board->pieceAt(sq);
                if (pc.isValid())
                    currCounts[pc.side()][pc.type()]++;
            }

        for (auto it = startCounts[Chess::Side::Black].constBegin(); it != startCounts[Chess::Side::Black].constEnd(); ++it)
        {
            int type = it.key();
            int captured = it.value() - currCounts[Chess::Side::Black].value(type, 0);
            for (int i = 0; i < captured; ++i)
                whiteCaptured.append(Chess::Piece(Chess::Side::Black, type));
        }

        for (auto it = startCounts[Chess::Side::White].constBegin(); it != startCounts[Chess::Side::White].constEnd(); ++it)
        {
            int type = it.key();
            int captured = it.value() - currCounts[Chess::Side::White].value(type, 0);
            for (int i = 0; i < captured; ++i)
                blackCaptured.append(Chess::Piece(Chess::Side::White, type));
        }
    }

    // Sort by piece type
    auto cmp = [](const Chess::Piece &a, const Chess::Piece &b){ return a.type() < b.type(); };
    std::sort(whiteCaptured.begin(), whiteCaptured.end(), cmp);
    std::sort(blackCaptured.begin(), blackCaptured.end(), cmp);

    // two rows — whiteCaptured on top, blackCaptured on bottom
    qreal x = 0;
    int SPACE_BETWEEN_PIECES = 24;
    for (int i = 0; i < whiteCaptured.size(); ++i)
    {
        Chess::Piece pc = whiteCaptured.at(i);
        GraphicsPiece* g = new GraphicsPiece(pc, s_squareSize, board->representation(pc), renderer);
        m_scene->addItem(g);
        g->setPos(x + i * (s_squareSize + s_spacing)+SPACE_BETWEEN_PIECES, SPACE_BETWEEN_PIECES);
    }
    for (int i = 0; i < blackCaptured.size(); ++i)
    {
        Chess::Piece pc = blackCaptured.at(i);
        GraphicsPiece* g = new GraphicsPiece(pc, s_squareSize, board->representation(pc), renderer);
        m_scene->addItem(g);
        g->setPos(x + i * (s_squareSize + s_spacing)+SPACE_BETWEEN_PIECES, s_squareSize + s_rowSpacing+SPACE_BETWEEN_PIECES);
    }

    m_scene->setSceneRect(m_scene->itemsBoundingRect());
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);



    int cols = qMax(whiteCaptured.size(), blackCaptured.size());
    qreal width = cols > 0 ? cols * (s_squareSize + s_spacing) - s_spacing : s_squareSize;
    qreal height = s_squareSize * 2 + s_rowSpacing;
    m_scene->setSceneRect(0, 0, width, height);

    m_view->resetTransform();
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setMinimumHeight(static_cast<int>(height + 4));
}

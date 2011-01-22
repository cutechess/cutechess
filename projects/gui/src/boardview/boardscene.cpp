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

#include "boardscene.h"
#include <QSvgRenderer>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <board/board.h>
#include "graphicsboard.h"
#include "graphicspiecereserve.h"
#include "graphicspiece.h"
#include "piecechooser.h"


static qreal s_squareSize = 50;

BoardScene::BoardScene(QObject* parent)
	: QGraphicsScene(parent),
	  m_board(0),
	  m_direction(Forward),
	  m_squares(0),
	  m_reserve(0),
	  m_chooser(0),
	  m_anim(0),
	  m_renderer(new QSvgRenderer(QString(":/default.svg"), this))
{
}

BoardScene::~BoardScene()
{
	delete m_board;
}

void BoardScene::setBoard(Chess::Board* board)
{
	clear();
	m_history.clear();
	m_transition.clear();
	m_squares = 0;
	m_reserve = 0;
	m_chooser = 0;
	m_board = board;
}

void BoardScene::populate()
{
	Q_ASSERT(m_board != 0);

	clear();
	m_history.clear();
	m_transition.clear();
	m_squares = 0;
	m_reserve = 0;
	m_chooser = 0;

	m_squares = new GraphicsBoard(m_board->width(),
				      m_board->height(),
				      s_squareSize);
	addItem(m_squares);

	if (m_board->variantHasDrops())
	{
		m_reserve = new GraphicsPieceReserve(s_squareSize);
		addItem(m_reserve);
		m_reserve->setX(m_squares->boundingRect().right() +
				m_reserve->boundingRect().right() + 7);

		QList<Chess::Piece> types(m_board->reservePieceTypes());
		foreach (const Chess::Piece& piece, types)
		{
			int count = m_board->reserveCount(piece);
			for (int i = 0; i < count; i++)
				m_reserve->addPiece(createPiece(piece));
		}
	}

	setSceneRect(itemsBoundingRect());

	for (int x = 0; x < m_board->width(); x++)
	{
		for (int y = 0; y < m_board->height(); y++)
		{
			Chess::Square sq(x, y);
			GraphicsPiece* piece(createPiece(m_board->pieceAt(sq)));

			if (piece != 0)
				m_squares->setSquare(sq, piece);
		}
	}

	updateMoves();
}

void BoardScene::setFenString(const QString& fenString)
{
	Q_ASSERT(m_board != 0);

	bool ok = m_board->setFenString(fenString);
	Q_ASSERT(ok); Q_UNUSED(ok);

	populate();
}

void BoardScene::makeMove(const Chess::Move& move)
{
	stopAnimation();

	Chess::BoardTransition transition;
	m_board->makeMove(move, &transition);
	m_history << transition;
	applyTransition(transition, Forward);
}

void BoardScene::makeMove(const Chess::GenericMove& move)
{
	makeMove(m_board->moveFromGenericMove(move));
}

void BoardScene::undoMove()
{
	stopAnimation();

	m_board->undoMove();
	applyTransition(m_history.takeLast(), Backward);
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	stopAnimation();

	if (event->button() != Qt::LeftButton)
	{
		event->ignore();
		return;
	}

	if (m_chooser != 0)
	{
		bool ok = sendEvent(m_chooser, event);
		Q_ASSERT(ok); Q_UNUSED(ok);
		return;
	}

	QGraphicsItem* item = itemAt(event->scenePos());
	GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(item);
	if (m_targets.contains(piece))
	{
		m_sourcePos = piece->scenePos();
		piece->setParentItem(0);
		piece->setPos(m_sourcePos);

		QGraphicsScene::mousePressEvent(event);
	}
	else
		event->ignore();
}

void BoardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(mouseGrabberItem());
	if (piece != 0 && event->button() == Qt::LeftButton)
	{
		QPointF targetPos(m_squares->mapFromScene(event->scenePos()));
		tryMove(piece, targetPos);
	}

	QGraphicsScene::mouseReleaseEvent(event);
}

void BoardScene::onTransitionFinished()
{
	foreach (const Chess::BoardTransition::Move& move, m_transition.moves())
	{
		if (m_direction == Forward)
			m_squares->makeMove(move.source, move.target);
		else
			m_squares->makeMove(move.target, move.source);
	}

	foreach (const Chess::BoardTransition::Drop& drop, m_transition.drops())
	{
		if (m_direction == Forward)
			m_squares->setSquare(drop.target,
					     m_reserve->takePiece(drop.piece));
		else
			m_reserve->addPiece(m_squares->takePieceAt(drop.target));
	}

	foreach (const Chess::Square& square, m_transition.squares())
	{
		Chess::Piece type = m_board->pieceAt(square);
		if (type != m_squares->pieceTypeAt(square))
			m_squares->setSquare(square, createPiece(type));
	}

	foreach (const Chess::Piece& piece, m_transition.reserve())
	{
		int count = m_reserve->pieceCount(piece);
		int newCount = m_board->reserveCount(piece);

		while (newCount > count)
		{
			m_reserve->addPiece(createPiece(piece));
			count++;
		}
		while (newCount < count)
		{
			delete m_reserve->takePiece(piece);
			count--;
		}
	}

	m_transition.clear();
	updateMoves();
}

void BoardScene::onPromotionChosen(const Chess::Piece& promotion)
{
	if (!promotion.isValid())
	{
		GraphicsPiece* piece = m_squares->pieceAt(m_promotionMove.sourceSquare());
		m_anim = pieceAnimation(piece, m_sourcePos);
		m_anim->start(QAbstractAnimation::DeleteWhenStopped);
	}
	else
	{
		m_promotionMove.setPromotion(promotion.type());
		emit humanMove(m_promotionMove, m_board->sideToMove());
	}
}

GraphicsPiece* BoardScene::createPiece(const Chess::Piece& piece)
{
	Q_ASSERT(m_board != 0);
	Q_ASSERT(m_renderer != 0);
	Q_ASSERT(m_squares != 0);

	if (!piece.isValid())
		return 0;

	return new GraphicsPiece(piece,
				 s_squareSize,
				 m_board->pieceSymbol(piece),
				 m_renderer);
}

QPropertyAnimation* BoardScene::pieceAnimation(GraphicsPiece* piece,
					       const QPointF& endPoint) const
{
	Q_ASSERT(piece != 0);

	QPointF startPoint(piece->scenePos());
	QPropertyAnimation* anim = new QPropertyAnimation(piece, "pos");
	connect(anim, SIGNAL(finished()), piece, SLOT(restoreParent()));

	anim->setStartValue(startPoint);
	anim->setEndValue(endPoint);
	anim->setEasingCurve(QEasingCurve::InOutQuad);
	anim->setDuration(300);

	piece->setParentItem(0);
	piece->setPos(startPoint);

	return anim;
}

QPropertyAnimation* BoardScene::pieceAnimation(GraphicsPiece* piece,
					       const Chess::Square& target) const
{
	QPointF point(m_squares->mapToScene(m_squares->squarePos(target)));
	return pieceAnimation(piece, point);
}

void BoardScene::stopAnimation()
{
	if (m_anim != 0 && m_anim->state() == QAbstractAnimation::Running)
		m_anim->setCurrentTime(m_anim->totalDuration());
}

void BoardScene::tryMove(GraphicsPiece* piece, const QPointF& targetPos)
{
	Chess::Square target(m_squares->squareAt(targetPos));

	// Illegal move
	if (!m_targets.contains(piece, target))
	{
		m_anim = pieceAnimation(piece, m_sourcePos);
		m_anim->start(QAbstractAnimation::DeleteWhenStopped);
	}
	// Normal move
	else if (piece->container() == m_squares)
	{
		Chess::Square source(m_squares->squareAt(m_squares->mapFromScene(m_sourcePos)));

		QList<Chess::Piece> promotions;
		foreach (const Chess::GenericMove& move, m_moves)
		{
			if (move.sourceSquare() != source
			||  move.targetSquare() != target)
				continue;

			if (move.promotion() != 0)
				promotions << Chess::Piece(m_board->sideToMove(),
							   move.promotion());
			else
				promotions << Chess::Piece();
		}
		Q_ASSERT(!promotions.isEmpty());

		Chess::GenericMove move(source, target, 0);
		if (promotions.size() > 1)
		{
			m_promotionMove = move;
			selectPiece(promotions, SLOT(onPromotionChosen(Chess::Piece)));
		}
		else
		{
			move.setPromotion(promotions.first().type());
			emit humanMove(move, m_board->sideToMove());
		}
	}
	// Piece drop
	else if (piece->container() == m_reserve)
	{
		Chess::GenericMove move(Chess::Square(), target,
					piece->pieceType().type());
		emit humanMove(move, m_board->sideToMove());
	}
}

void BoardScene::selectPiece(const QList<Chess::Piece>& types,
			     const char* member)
{
	QList<GraphicsPiece*> list;
	foreach (const Chess::Piece& type, types)
		list << createPiece(type);

	m_chooser = new PieceChooser(list, s_squareSize);
	connect(m_chooser, SIGNAL(pieceChosen(Chess::Piece)), this, member);
	addItem(m_chooser);
	m_chooser->reveal();
}

void BoardScene::applyTransition(const Chess::BoardTransition& transition,
				 MoveDirection direction)
{
	m_transition = transition;
	m_direction = direction;

	QParallelAnimationGroup* group = new QParallelAnimationGroup;
	connect(group, SIGNAL(finished()), this, SLOT(onTransitionFinished()));
	m_anim = group;

	foreach (const Chess::BoardTransition::Move& move, transition.moves())
	{
		Chess::Square source = move.source;
		Chess::Square target = move.target;
		if (direction == Backward)
			qSwap(source, target);

		GraphicsPiece* piece = m_squares->pieceAt(source);
		if (piece == 0)
		{
			piece = createPiece(m_board->pieceAt(target));
			m_squares->setSquare(source, piece);
		}
		group->addAnimation(pieceAnimation(piece, target));
	}

	foreach (const Chess::BoardTransition::Drop& drop, transition.drops())
	{
		if (direction == Forward)
		{
			GraphicsPiece* piece = m_reserve->piece(drop.piece);
			group->addAnimation(pieceAnimation(piece, drop.target));
		}
		else
		{
			GraphicsPiece* piece = m_squares->pieceAt(drop.target);
			group->addAnimation(pieceAnimation(piece, m_reserve->scenePos()));
		}
	}

	group->start(QAbstractAnimation::DeleteWhenStopped);
}

void BoardScene::updateMoves()
{
	m_targets.clear();
	m_moves.clear();
	if (!m_board->result().isNone())
		return;

	QVector<Chess::Move> moves(m_board->legalMoves());
	foreach (const Chess::Move& move, moves)
	{
		Chess::GenericMove gmove(m_board->genericMove(move));
		m_moves << gmove;
		GraphicsPiece* piece = 0;

		if (gmove.sourceSquare().isValid())
			piece = m_squares->pieceAt(gmove.sourceSquare());
		else
		{
			Chess::Piece pieceType(m_board->sideToMove(),
					       gmove.promotion());
			piece = m_reserve->piece(pieceType);
		}
		Q_ASSERT(piece != 0);

		Chess::Square target(gmove.targetSquare());
		if (!m_targets.contains(piece, target))
			m_targets.insert(piece, target);
	}
}

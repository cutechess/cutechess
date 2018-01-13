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
#include <QSequentialAnimationGroup>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QSettings>
#include <algorithm>
#include <board/board.h>
#include "graphicsboard.h"
#include "graphicspiecereserve.h"
#include "graphicspiece.h"
#include "piecechooser.h"

namespace {

const qreal s_squareSize = 50;

} // anonymous namespace

BoardScene::BoardScene(QObject* parent)
	: QGraphicsScene(parent),
	  m_board(nullptr),
	  m_direction(Forward),
	  m_squares(nullptr),
	  m_reserve(nullptr),
	  m_chooser(nullptr),
	  m_anim(nullptr),
	  m_renderer(new QSvgRenderer(QString(":/default.svg"), this)),
	  m_highlightPiece(nullptr),
	  m_moveArrows(nullptr)
{
}

BoardScene::~BoardScene()
{
	stopAnimation();
	delete m_board;
}

Chess::Board* BoardScene::board() const
{
	return m_board;
}

void BoardScene::setBoard(Chess::Board* board)
{
	stopAnimation();
	if (m_board != board)
		delete m_board;

	clear();
	m_history.clear();
	m_transition.clear();
	m_squares = nullptr;
	m_reserve = nullptr;
	m_chooser = nullptr;
	m_highlightPiece = nullptr;
	m_moveArrows = nullptr;
	m_board = board;
}

void BoardScene::populate()
{
	Q_ASSERT(m_board != nullptr);

	stopAnimation();
	clear();
	m_history.clear();
	m_transition.clear();
	m_squares = nullptr;
	m_reserve = nullptr;
	m_chooser = nullptr;
	m_highlightPiece = nullptr;
	m_moveArrows = nullptr;

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

		const auto types = m_board->reservePieceTypes();
		for (const auto& piece : types)
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

			if (piece != nullptr)
				m_squares->setSquare(sq, piece);
		}
	}

	updateMoves();
}

void BoardScene::setFenString(const QString& fenString)
{
	Q_ASSERT(m_board != nullptr);

	bool ok = m_board->setFenString(fenString);
	Q_ASSERT(ok); Q_UNUSED(ok);

	populate();
}

void BoardScene::makeMove(const Chess::Move& move)
{
	stopAnimation();
	delete m_moveArrows;
	m_moveArrows = new QGraphicsItemGroup(m_squares);
	m_moveArrows->setZValue(1);

	Q_ASSERT(!move.isNull());
	Q_ASSERT(m_board->isLegalMove(move));

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
	delete m_moveArrows;
	m_moveArrows = nullptr;

	m_board->undoMove();
	applyTransition(m_history.takeLast(), Backward);
}

void BoardScene::cancelUserMove()
{
	GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(mouseGrabberItem());
	if (piece == nullptr)
	{
		if (!m_chooser.isNull())
			m_chooser->cancelChoice();
		return;
	}

	m_anim = pieceAnimation(piece, m_sourcePos);
	m_anim->start(QAbstractAnimation::DeleteWhenStopped);

	m_highlightPiece = nullptr;
	m_squares->clearHighlights();
}

void BoardScene::flip()
{
	stopAnimation();
	m_squares->setFlipped(!m_squares->isFlipped());

	QParallelAnimationGroup* group = new QParallelAnimationGroup;
	m_anim = group;

	for (int y = 0; y < m_board->height(); y++)
	{
		for (int x = 0; x < m_board->width(); x++)
		{
			auto sq = Chess::Square(x, y);
			auto pc = m_squares->pieceAt(sq);
			if (!pc)
				continue;
			group->addAnimation(pieceAnimation(pc, squarePos(sq)));
		}
	}

	if (m_moveArrows)
	{
		qreal angle = 180 - m_moveArrows->rotation();
		m_moveArrows->setRotation(angle);
	}

	group->start(QAbstractAnimation::DeleteWhenStopped);
}

void BoardScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsPiece* piece = pieceAt(event->scenePos());
	if (piece == m_highlightPiece || m_anim != nullptr || m_chooser != nullptr)
		return QGraphicsScene::mouseMoveEvent(event);

	if (m_targets.contains(piece)
	&&  QSettings().value("ui/highlight_legal_moves", true).toBool())
	{
		m_highlightPiece = piece;
		m_squares->setHighlights(m_targets.values(piece));
	}
	else
	{
		m_highlightPiece = nullptr;
		m_squares->clearHighlights();
	}

	QGraphicsScene::mouseMoveEvent(event);
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	stopAnimation();

	if (m_chooser != nullptr)
	{
		bool ok = sendEvent(m_chooser, event);
		Q_ASSERT(ok); Q_UNUSED(ok);
		return;
	}

	GraphicsPiece* piece = pieceAt(event->scenePos());
	if (piece == nullptr || event->button() != Qt::LeftButton)
		return;

	if (m_targets.contains(piece))
	{
		piece->setFlag(QGraphicsItem::ItemIsMovable, true);
		m_sourcePos = piece->scenePos();
		piece->setParentItem(nullptr);
		piece->setPos(m_sourcePos);

		QGraphicsScene::mousePressEvent(event);
	}
	else
		piece->setFlag(QGraphicsItem::ItemIsMovable, false);
}

void BoardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(mouseGrabberItem());
	if (piece != nullptr && event->button() == Qt::LeftButton)
	{
		QPointF targetPos(m_squares->mapFromScene(event->scenePos()));
		tryMove(piece, targetPos);
	}

	QGraphicsScene::mouseReleaseEvent(event);
}

void BoardScene::onTransitionFinished()
{
	const auto drops = m_transition.drops();
	if (m_direction == Backward)
	{
		for (const auto& drop : drops)
			m_reserve->addPiece(m_squares->takePieceAt(drop.target));
	}

	const auto moves = m_transition.moves();
	for (const auto& move : moves)
	{
		if (m_direction == Forward)
			m_squares->movePiece(move.source, move.target);
		else
			m_squares->movePiece(move.target, move.source);
	}

	if (m_direction == Forward)
	{
		for (const auto& drop : drops)
			m_squares->setSquare(drop.target,
					     m_reserve->takePiece(drop.piece));
	}

	const auto squares = m_transition.squares();
	for (const auto& square : squares)
	{
		Chess::Piece type = m_board->pieceAt(square);
		if (type != m_squares->pieceTypeAt(square))
			m_squares->setSquare(square, createPiece(type));
	}

	const auto reserve = m_transition.reserve();
	for (const auto& piece : reserve)
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
	m_chooser = nullptr;
	if (!promotion.isValid() && !m_board->variantHasOptionalPromotions())
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

void BoardScene::onGameFinished(ChessGame* game, Chess::Result result)
{
	Q_UNUSED(game);

	stopAnimation();
	cancelUserMove();

	auto text = new QGraphicsTextItem(result.toShortString(), m_squares);
	auto font = text->font();
	font.setPointSize(10);
	font.setBold(true);
	text->setFont(font);
	text->setOpacity(0.9);
	const auto rect = text->boundingRect();
	qreal x = rect.width() / 2;
	qreal y = rect.height() / 2;
	text->setPos(-x, -y);
	text->setTransformOriginPoint(x, y);

	auto group = new QSequentialAnimationGroup;
	text->setParent(group);
	m_anim = group;

	auto scAnim = new QPropertyAnimation(text, "scale");
	scAnim->setStartValue(text->scale());
	scAnim->setEndValue(6.5);
	scAnim->setEasingCurve(QEasingCurve::InOutQuad);
	scAnim->setDuration(500);
	group->addAnimation(scAnim);

	auto opAnim = new QPropertyAnimation(text, "opacity");
	opAnim->setStartValue(text->opacity());
	opAnim->setEndValue(0.0);
	opAnim->setEasingCurve(QEasingCurve::InOutQuad);
	opAnim->setDuration(2500);
	group->addAnimation(opAnim);

	group->start(QAbstractAnimation::DeleteWhenStopped);
}

QPointF BoardScene::squarePos(const Chess::Square& square) const
{
	return m_squares->mapToScene(m_squares->squarePos(square));
}

GraphicsPiece* BoardScene::pieceAt(const QPointF& pos) const
{
	const auto items = this->items(pos);
	for (auto item : items)
	{
		GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(item);
		if (piece != nullptr)
			return piece;
	}

	return nullptr;
}

GraphicsPiece* BoardScene::createPiece(const Chess::Piece& piece)
{
	Q_ASSERT(m_board != nullptr);
	Q_ASSERT(m_renderer != nullptr);
	Q_ASSERT(m_squares != nullptr);

	if (!piece.isValid())
		return nullptr;

	return new GraphicsPiece(piece,
				 s_squareSize,
				 m_board->representation(piece),
				 m_renderer);
}

QPropertyAnimation* BoardScene::pieceAnimation(GraphicsPiece* piece,
					       const QPointF& endPoint) const
{
	Q_ASSERT(piece != nullptr);

	QPointF startPoint(piece->scenePos());
	QPropertyAnimation* anim = new QPropertyAnimation(piece, "pos");
	connect(anim, SIGNAL(finished()), piece, SLOT(restoreParent()));

	anim->setStartValue(startPoint);
	anim->setEndValue(endPoint);
	anim->setEasingCurve(QEasingCurve::InOutQuad);
	anim->setDuration(300);

	piece->setParentItem(nullptr);
	piece->setPos(startPoint);

	return anim;
}

void BoardScene::stopAnimation()
{
	if (m_anim != nullptr && m_anim->state() == QAbstractAnimation::Running)
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
		const auto moves = m_moves;
		for (const auto& move : moves)
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

	m_highlightPiece = nullptr;
	m_squares->clearHighlights();
}

void BoardScene::selectPiece(const QList<Chess::Piece>& types,
			     const char* member)
{
	QList<GraphicsPiece*> list;
	for (const auto& type : types)
		list << createPiece(type);

	m_chooser = new PieceChooser(list, s_squareSize);
	connect(m_chooser, SIGNAL(pieceChosen(Chess::Piece)), this, member);
	addItem(m_chooser);
	m_chooser->reveal();
}

void BoardScene::addMoveArrow(const QPointF& sourcePos,
			      const QPointF& targetPos)
{
	Q_ASSERT(m_moveArrows != nullptr);

	QLineF l1(sourcePos, targetPos);
	QLineF l2(l1.normalVector());

	l1.setLength(l1.length() - s_squareSize / 2.5);
	l2.setLength(s_squareSize / 3.0);
	l2.translate(l2.dx() / -2.0, l2.dy() / -2.0);

	QPolygonF polygon;
	polygon << l2.p1() << l1.p2() << l2.p2();

	QGraphicsPolygonItem* item = new QGraphicsPolygonItem(polygon);
	item->setPen(QPen(QBrush(Qt::yellow), 2));
	item->setBrush(Qt::yellow);
	item->setOpacity(0.6);

	m_moveArrows->addToGroup(item);
}

void BoardScene::applyTransition(const Chess::BoardTransition& transition,
				 MoveDirection direction)
{
	m_transition = transition;
	m_direction = direction;

	QParallelAnimationGroup* group = new QParallelAnimationGroup;
	connect(group, SIGNAL(finished()), this, SLOT(onTransitionFinished()));
	m_anim = group;

	const auto drops = transition.drops();
	if (direction == Backward)
	{
		for (const auto& drop : drops)
		{
			GraphicsPiece* piece = m_squares->pieceAt(drop.target);
			group->addAnimation(pieceAnimation(piece, m_reserve->scenePos()));
		}
	}

	const auto moves = transition.moves();
	for (const auto& move : moves)
	{
		Chess::Square source = move.source;
		Chess::Square target = move.target;

		if (direction == Backward)
			std::swap(source, target);
		else
			addMoveArrow(m_squares->squarePos(source),
					 m_squares->squarePos(target));

		GraphicsPiece* piece = m_squares->pieceAt(source);
		if (piece == nullptr)
		{
			piece = createPiece(m_board->pieceAt(target));
			m_squares->setSquare(source, piece);
		}
		group->addAnimation(pieceAnimation(piece, squarePos(target)));
	}

	if (direction == Forward)
	{
		for (const auto& drop : drops)
		{
			addMoveArrow(m_squares->mapFromItem(m_reserve, QPointF()),
					 m_squares->squarePos(drop.target));

			GraphicsPiece* piece = m_reserve->piece(drop.piece);
			group->addAnimation(pieceAnimation(piece, squarePos(drop.target)));
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

	const auto moves = m_board->legalMoves();
	for (const auto& move : moves)
	{
		Chess::GenericMove gmove(m_board->genericMove(move));
		m_moves << gmove;
		GraphicsPiece* piece = nullptr;

		if (gmove.sourceSquare().isValid())
			piece = m_squares->pieceAt(gmove.sourceSquare());
		else
		{
			Chess::Piece pieceType(m_board->sideToMove(),
					       gmove.promotion());
			piece = m_reserve->piece(pieceType);
		}
		Q_ASSERT(piece != nullptr);

		Chess::Square target(gmove.targetSquare());
		if (!m_targets.contains(piece, target))
			m_targets.insert(piece, target);
	}
}

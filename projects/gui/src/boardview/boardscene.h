/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#ifndef BOARDSCENE_H
#define BOARDSCENE_H

#include <QGraphicsScene>
#include <QMultiMap>
#include <QPointer>
#include <board/square.h>
#include <board/genericmove.h>
#include <board/boardtransition.h>
#include <board/result.h>
namespace Chess
{
	class Board;
	class Move;
	class Side;
	class Piece;
}
class ChessGame;
class QSvgRenderer;
class QAbstractAnimation;
class QPropertyAnimation;
class GraphicsBoard;
class GraphicsPieceReserve;
class GraphicsPiece;
class PieceChooser;


/*!
 * \brief A graphical surface for displaying a chessgame.
 *
 * BoardScene is the top-level container for the board, the
 * chess pieces, etc. It also manages the deletion and creation
 * of pieces, visualising moves made in a game, emitting moves
 * made by a human player, etc.
 *
 * BoardScene is that class that connects to the players and game
 * objects to synchronize the graphical side with the internals.
 */
class BoardScene : public QGraphicsScene
{
	Q_OBJECT

	public:
		/*! Creates a new BoardScene object. */
		explicit BoardScene(QObject* parent = nullptr);
		/*! Destroys the scene and all its items. */
		virtual ~BoardScene();

		/*! Returns the current internal board object. */
		Chess::Board* board() const;
		/*!
		 * Clears the scene and sets \a board as the internal board.
		 *
		 * The scene takes ownership of the board, so it's usually
		 * best to give the scene its own copy of a board.
		 */
		void setBoard(Chess::Board* board);

	public slots:
		/*!
		 * Clears the scene, creates a new board, and populates
		 * it with chess pieces.
		 *
		 * The scene is populated to match the current FEN string of
		 * the internal board, so the internal board must be fully
		 * initialized before calling this function.
		 */
		void populate();
		/*! Re-populates the scene according to \a fenString. */
		void setFenString(const QString& fenString);
		/*! Makes the move \a move in the scene. */
		void makeMove(const Chess::Move& move);
		/*! Makes the move \a move in the scene. */
		void makeMove(const Chess::GenericMove& move);
		/*! Reverses the last move that was made in the scene. */
		void undoMove();
		/*! Flips the board, with animation. */
		void flip();
		/*!
		 * Cancels any ongoing user move and flashes \a result
		 * over the board.
		 */
		void onGameFinished(ChessGame* game, Chess::Result result);

	signals:
		/*!
		 * This signal is emitted when a human player has made a move.
		 *
		 * The move is guaranteed to be legal.
		 * The move was made by the player on \a side side.
		 */
		void humanMove(const Chess::GenericMove& move,
			       const Chess::Side& side);

	protected:
		// Inherited from QGraphicsScene
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	private slots:
		void onTransitionFinished();
		void onPromotionChosen(const Chess::Piece& promotion);

	private:
		void cancelUserMove();

		enum MoveDirection
		{
			Forward,
			Backward
		};

		QPointF squarePos(const Chess::Square& square) const;
		GraphicsPiece* pieceAt(const QPointF& pos) const;
		GraphicsPiece* createPiece(const Chess::Piece& piece);
		QPropertyAnimation* pieceAnimation(GraphicsPiece* piece,
						   const QPointF& endPoint) const;
		void stopAnimation();
		void tryMove(GraphicsPiece* piece, const QPointF& targetPos);
		void selectPiece(const QList<Chess::Piece>& types,
				 const char* member);
		void addMoveArrow(const QPointF& sourcePos,
				  const QPointF& targetPos);
		void applyTransition(const Chess::BoardTransition& transition,
				     MoveDirection direction);
		void updateMoves();

		Chess::Board* m_board;
		MoveDirection m_direction;
		Chess::BoardTransition m_transition;
		QList<Chess::BoardTransition> m_history;
		QPointF m_sourcePos;
		GraphicsBoard* m_squares;
		GraphicsPieceReserve* m_reserve;
		QPointer<PieceChooser> m_chooser;
		QPointer<QAbstractAnimation> m_anim;
		QSvgRenderer* m_renderer;
		QMultiMap<GraphicsPiece*, Chess::Square> m_targets;
		QList<Chess::GenericMove> m_moves;
		Chess::GenericMove m_promotionMove;
		GraphicsPiece* m_highlightPiece;
		QGraphicsItemGroup* m_moveArrows;
};

#endif // BOARDSCENE_H

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

#ifndef CHESSBOARDVIEW_H
#define CHESSBOARDVIEW_H

#include <QGraphicsView>

class QResizeEvent;

/*!
 * \brief The ChessboardView class provides correct resizing of the visual
 * chessboard.
 *
 * %ChessboardView doesn't add any extra funtionality compared to
 * \e QGraphicsView except correct resizing. Correct resizing meaning:
 *
 * - The visual chessboard is fit in to the view and scaled correctly when
 *   the view resizes.
 *
 * - The visual chessboard is scaled correctly when expanding or shrinking
 *   a dock widget.
*/
class ChessboardView : public QGraphicsView
{
	public:
		/*!
		 * Constructs a ChessboardView.
		*/
		ChessboardView(QWidget* parent = 0);

	protected:
		/*!
		 * Fired after viewport has been given a new geometry.
		*/
		void resizeEvent(QResizeEvent* event);

};

#endif // CHESSBOARDVIEW_H


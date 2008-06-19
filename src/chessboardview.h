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

#ifndef CHESSBOARDVIEW_H
#define CHESSBOARDVIEW_H

#include <QGraphicsView>

class QResizeEvent;

/**
 * ChessboardView class handles resizing of the visual chessboard correctly.
 *
 * ChessboardView doesn't add any extra funtionality compared to QGraphicsView
 * except proper resizing. Proper resizing meaning: 
 *
 * - The visual chessboard is fit in to the view and scaled correctly when
 *   the main window resizes.
 *
 * - The visual chessboard is scaled correctly when expanding or shrinking
 *   a dock widget.
*/
class ChessboardView : public QGraphicsView
{
	public:
		ChessboardView(QWidget* parent = 0);

	protected:
		void resizeEvent(QResizeEvent* event);

};

#endif // CHESSBOARDVIEW_H


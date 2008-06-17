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

#ifndef CHESSBOARDWIDGET_H
#define CHESSBOARDWIDGET_H

#include <QGraphicsView>

class QResizeEvent;
class ChessboardItem;

class ChessboardWidget : public QGraphicsView
{
	Q_OBJECT	

	public:
		ChessboardWidget();

	protected:
		void resizeEvent(QResizeEvent* event);
	
	private:
		ChessboardItem* m_board;
};

#endif // CHESSBOARDWIDGET_H


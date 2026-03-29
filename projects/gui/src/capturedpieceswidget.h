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













/*
    Captured pieces dock widget
*/
#ifndef CAPTUREDPIECESWIDGET_H
#define CAPTUREDPIECESWIDGET_H

#include <QWidget>
#include <QPointer>

class QGraphicsView;
class QGraphicsScene;
class ChessGame;

class CapturedPiecesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CapturedPiecesWidget(QWidget* parent = nullptr);
    void setGame(ChessGame* game);

private slots:
    void updateCapturedPieces();

private:
    QPointer<ChessGame> m_game;
    QGraphicsView* m_view;
    QGraphicsScene* m_scene;
};

#endif // CAPTUREDPIECESWIDGET_H

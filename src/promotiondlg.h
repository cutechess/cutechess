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

#ifndef PROMOTIONDLG_H
#define PROMOTIONDLG_H

#include <QDialog>

#include "chessboard/chess.h"

class QRadioButton;
class QSignalMapper;

/*!
 * \brief The PromotionDialog class provides a dialog for chess piece promotion.
*/
class PromotionDialog : public QDialog
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new promotion dialog.
		*/
		PromotionDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
		/*!
		 * Returns the user selected promotion type.
		 *
		 * This method always returns a valid promotion type even if
		 * the user cancelled the dialog. By default the promotion type
		 * is queen.
		*/
		Chess::Piece promotionType() const;
	
	private slots:
		void selectPromotionType(int type);

	private:
		Chess::Piece m_promotionType;
		QRadioButton* m_queenRadio;
		QRadioButton* m_knightRadio;
		QRadioButton* m_rookRadio;
		QRadioButton* m_bishopRadio;
		QSignalMapper* m_signalMapper;

};

#endif // PROMOTIONDLG_H

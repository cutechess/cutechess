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

#ifndef PROMOTIONDLG_H
#define PROMOTIONDLG_H

#include <QDialog>
namespace Chess { class Board; }

/*!
 * \brief The PromotionDialog class provides a dialog for chess piece promotion.
*/
class PromotionDialog : public QDialog
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new promotion dialog.
		 *
		 * \param board A board object for converting the promotion piece
		 * types into strings or piece symbols.
		 * \param promotions A list of accepted promotion piece types.
		 */
		PromotionDialog(const Chess::Board* board,
				const QList<int>& promotions,
				QWidget* parent = 0,
				Qt::WindowFlags f = 0);
		/*!
		 * Returns the user selected promotion type.
		 *
		 * This method always returns a valid promotion type even if
		 * the user cancelled the dialog.
		 */
		int promotionType() const;
	
	private slots:
		void selectPromotionType(int type);

	private:
		int m_promotionType;
};

#endif // PROMOTIONDLG_H

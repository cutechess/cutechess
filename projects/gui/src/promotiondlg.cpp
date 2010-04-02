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

#include "promotiondlg.h"
#include <board/board.h>
#include <QtGui>


PromotionDialog::PromotionDialog(const Chess::Board* board,
				 const QList<int>& promotions,
				 QWidget* parent,
				 Qt::WindowFlags f)
	: QDialog(parent, f)
{
	Q_ASSERT(board != 0);
	Q_ASSERT(promotions.size() > 1);

	setWindowTitle(tr("Promotion"));

	// Radio buttons that will control the type of the promotion
	QList<QRadioButton*> radioButtons;
	// Labels that will show the chess symbol
	QList<QLabel*> labels;

	foreach (int prom, promotions)
	{
		if (prom == Chess::Piece::NoPiece)
		{
			radioButtons << new QRadioButton(tr("No promotion"));
			labels << new QLabel("-");
			continue;
		}

		radioButtons << new QRadioButton(board->pieceString(prom));
		labels << new QLabel(board->pieceSymbol(Chess::Piece(Chess::White, prom)));
	}

	// Set default promotion type
	radioButtons.first()->setChecked(true);
	m_promotionType = promotions.first();

	QLabel* promoteToLabel = new QLabel(tr("Promote to:"));

	// Promote and Cancel buttons at the bottom
	QPushButton* promoteButton = new QPushButton(tr("Promote"));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"));
	QDialogButtonBox* dlgButtonBox = new QDialogButtonBox(Qt::Horizontal);
	dlgButtonBox->addButton(promoteButton, QDialogButtonBox::AcceptRole);
	dlgButtonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

	/*
	 * The layout consist of two layouts put together; a vertical
	 * box layout and a grid layout inside of it. The grid layout
	 * is the more complex one so we discuss it here.
	 *
	 * The grid layout hold the chess symbol labels and the radio
	 * buttons. It also holds an extra padding so that the look
	 * of the dialog is more professional.
	 *
	 * The base layout of the grid is:
	 *
	 * +---------------------------------------+
	 * | <extra space> | Symbol | Radio button |
	 * +---------------------------------------+
	 *
	 * The extra space is done with setColumnMinimumWidth() and proper
	 * resizing with setColumnStretch() so that the radio button
	 * cell will expand to fill the empty space.
	*/
	QVBoxLayout* layout = new QVBoxLayout();
	QGridLayout* innerLayout = new QGridLayout();
	this->setLayout(layout);

	layout->addWidget(promoteToLabel);
	layout->addLayout(innerLayout);

	QSignalMapper* signalMapper = new QSignalMapper(this);

	for (int i = 0; i < radioButtons.size(); i++)
	{
		innerLayout->addWidget(labels[i], i, 1);
		innerLayout->addWidget(radioButtons[i], i, 2);

		// Use signal mapper to map all radio button signals to one
		// one slot: selectPromotionType()
		connect(radioButtons[i], SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
		signalMapper->setMapping(radioButtons[i], promotions[i]);
	}
	innerLayout->setColumnMinimumWidth(0, 20);
	innerLayout->setColumnStretch(2, 1);

	layout->addStretch(1);
	layout->addWidget(dlgButtonBox);

	// Connect the signals
	connect(promoteButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(selectPromotionType(int)));
}

void PromotionDialog::selectPromotionType(int type)
{
	m_promotionType = type;
}

int PromotionDialog::promotionType() const
{
	return m_promotionType;
}

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
#include <QtGui>
#include <board/westernboard.h>

PromotionDialog::PromotionDialog(QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setWindowTitle(tr("Promote"));

	// Radio buttons that will control the type of the promotion
	m_queenRadio = new QRadioButton(tr("Queen"));
	m_knightRadio = new QRadioButton(tr("Knight"));
	m_rookRadio = new QRadioButton(tr("Rook"));
	m_bishopRadio = new QRadioButton(tr("Bishop"));

	// Assign Queen as the default promotion type
	m_queenRadio->setChecked(true);
	m_promotionType = 5;

	QLabel* promoteToLabel = new QLabel(tr("Promote to:"));

	// TODO: Capablanca pieces (archbishop and chancellor) don't have
	// Unicode symbols, so use SVGs instead.
	
	// Labels that will show the Unicode chess symbol graphics
	QLabel* queenLabel = new QLabel("Q");
	QLabel* knightLabel = new QLabel("N");
	QLabel* rookLabel = new QLabel("R");
	QLabel* bishopLabel = new QLabel("B");

	// Double the original point size of the symbol labels
	// so that they're more visible
	QFont labelFont = queenLabel->font();
	labelFont.setPointSize(2 * labelFont.pointSize());
	queenLabel->setFont(labelFont);
	knightLabel->setFont(labelFont);
	rookLabel->setFont(labelFont);
	bishopLabel->setFont(labelFont);

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

	innerLayout->addWidget(queenLabel, 0, 1);
	innerLayout->addWidget(m_queenRadio, 0, 2);

	innerLayout->addWidget(knightLabel, 1, 1);
	innerLayout->addWidget(m_knightRadio, 1, 2);

	innerLayout->addWidget(rookLabel, 2, 1);
	innerLayout->addWidget(m_rookRadio, 2, 2);

	innerLayout->addWidget(bishopLabel, 3, 1);
	innerLayout->addWidget(m_bishopRadio, 3, 2);

	innerLayout->setColumnMinimumWidth(0, 20);
	innerLayout->setColumnStretch(2, 1);

	layout->addStretch(1);
	layout->addWidget(dlgButtonBox);

	// Connect the signals
	connect(promoteButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	// Use signal mapper to map all radio button signals to one
	// one slot: selectPromotionType()
	m_signalMapper = new QSignalMapper(this);

	connect(m_queenRadio, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
	m_signalMapper->setMapping(m_queenRadio, 5);

	connect(m_knightRadio, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
	m_signalMapper->setMapping(m_knightRadio, 2);

	connect(m_rookRadio, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
	m_signalMapper->setMapping(m_rookRadio, 4);

	connect(m_bishopRadio, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
	m_signalMapper->setMapping(m_bishopRadio, 3);

	connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(selectPromotionType(int)));
}

void PromotionDialog::selectPromotionType(int type)
{
	m_promotionType = type;
}

int PromotionDialog::promotionType() const
{
	return m_promotionType;
}

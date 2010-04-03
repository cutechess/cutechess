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

#include "newgamedlg.h"

#include <QAbstractItemView>
#include <QSortFilterProxyModel>

#include <board/board.h>
#include <enginemanager.h>

#include "cutechessapp.h"
#include "engineconfigurationmodel.h"
#include "engineconfigurationdlg.h"


NewGameDialog::NewGameDialog(QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);

	m_engines = new EngineConfigurationModel(
		CuteChessApplication::instance()->engineManager(), this);

	// Add Start button to the standard button box at the bottom
	QPushButton* startButton = new QPushButton(tr("Start"));
	m_buttonBox->addButton(startButton, QDialogButtonBox::AcceptRole);

	connect(startButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	connect(m_configureWhiteEngineButton, SIGNAL(clicked(bool)), this,
		SLOT(configureWhiteEngine()));
	connect(m_configureBlackEngineButton, SIGNAL(clicked(bool)), this,
		SLOT(configureBlackEngine()));

	m_proxyModel = new QSortFilterProxyModel(this);
	m_proxyModel->setSourceModel(m_engines);
	m_proxyModel->sort(0);
	m_proxyModel->setDynamicSortFilter(true);

	m_whiteEngineComboBox->setModel(m_proxyModel);
	m_blackEngineComboBox->setModel(m_proxyModel);

	if (m_engines->rowCount() > 0)
	{
		// TODO: The selected engine is not remembered
		m_whiteEngineComboBox->setCurrentIndex(0);
		m_blackEngineComboBox->setCurrentIndex(0);
	}

	QStringList variants = Chess::Board::registry()->items().keys();
	m_variantComboBox->addItems(variants);
}

NewGameDialog::PlayerType NewGameDialog::playerType(Chess::Side side) const
{
	Q_ASSERT(side != Chess::NoSide);

	if (side == Chess::White)
		return (m_whitePlayerHumanRadio->isChecked()) ? Human : CPU;
	else
		return (m_blackPlayerHumanRadio->isChecked()) ? Human : CPU;
}

int NewGameDialog::selectedEngineIndex(Chess::Side side) const
{
	Q_ASSERT(side != Chess::NoSide);

	int i;
	if (side == Chess::White)
		i = m_whiteEngineComboBox->currentIndex();
	else
		i = m_blackEngineComboBox->currentIndex();

	return m_proxyModel->mapToSource(m_proxyModel->index(i, 0)).row();
}

QString NewGameDialog::selectedVariant() const
{
	return m_variantComboBox->currentText();
}

void NewGameDialog::configureWhiteEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);

	int i = selectedEngineIndex(Chess::White);
	dlg.applyEngineInformation(
		CuteChessApplication::instance()->engineManager()->engines().at(i));

	if (dlg.exec() == QDialog::Accepted)
	{
		CuteChessApplication::instance()->engineManager()->updateEngineAt(i,
			dlg.engineConfiguration());
	}
}

void NewGameDialog::configureBlackEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);

	int i = selectedEngineIndex(Chess::Black);
	dlg.applyEngineInformation(
		CuteChessApplication::instance()->engineManager()->engines().at(i));

	if (dlg.exec() == QDialog::Accepted)
	{
		CuteChessApplication::instance()->engineManager()->updateEngineAt(i,
			dlg.engineConfiguration());
	}
}

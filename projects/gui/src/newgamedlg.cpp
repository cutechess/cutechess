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

#include <QAbstractItemView>

#include "newgamedlg.h"
#include "engineconfigurationmodel.h"
#include "engineconfigurationdlg.h"

NewGameDialog::NewGameDialog(EngineConfigurationModel* engineConfigurations,
                             QWidget* parent)
	: QDialog(parent),
	  m_engines(engineConfigurations)
{
	setupUi(this);

	// Add Start button to the standard button box at the bottom
	QPushButton* startButton = new QPushButton(tr("Start"));
	m_buttonBox->addButton(startButton, QDialogButtonBox::AcceptRole);

	connect(startButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	connect(m_configureWhiteEngineButton, SIGNAL(clicked(bool)), this,
		SLOT(configureWhiteEngine()));
	connect(m_configureBlackEngineButton, SIGNAL(clicked(bool)), this,
		SLOT(configureBlackEngine()));

	m_whiteEngineComboBox->setModel(m_engines);
	m_blackEngineComboBox->setModel(m_engines);

	if (m_engines->rowCount() > 0)
	{
		// TODO: The selected engine is not remembered
		m_whiteEngineComboBox->setCurrentIndex(0);
		m_blackEngineComboBox->setCurrentIndex(0);
	}
}

NewGameDialog::PlayerType NewGameDialog::whitePlayerType() const
{
	return (m_whitePlayerHumanRadio->isChecked()) ? Human : CPU;
}

NewGameDialog::PlayerType NewGameDialog::blackPlayerType() const
{
	return (m_blackPlayerHumanRadio->isChecked()) ? Human : CPU;
}

int NewGameDialog::selectedWhiteEngine() const
{
	return m_whiteEngineComboBox->currentIndex();
}

int NewGameDialog::selectedBlackEngine() const
{
	return m_blackEngineComboBox->currentIndex();
}

void NewGameDialog::configureWhiteEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);
	int selected = m_whiteEngineComboBox->currentIndex();

	dlg.applyEngineInformation(m_engines->configurations().at(selected));

	if (dlg.exec() == QDialog::Accepted)
	{
		m_engines->setConfiguration(m_engines->index(selected, 0),
			dlg.engineConfiguration());
	}
}

void NewGameDialog::configureBlackEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);
	int selected  = m_blackEngineComboBox->currentIndex();

	dlg.applyEngineInformation(m_engines->configurations().at(selected));

	if (dlg.exec() == QDialog::Accepted)
	{
		m_engines->setConfiguration(m_engines->index(selected, 0),
			dlg.engineConfiguration());
	}
}


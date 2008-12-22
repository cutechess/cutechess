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

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>

#include "engineconfigurationdlg.h"

EngineConfigurationDialog::EngineConfigurationDialog(
	EngineConfigurationDialog::DialogMode mode, QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);

	if (mode == EngineConfigurationDialog::AddEngine)
	{
		setWindowTitle(tr("Add Engine"));
	}
	else
	{
		setWindowTitle(tr("Edit Engine"));
	}

	m_protocolCombo->addItem("Xboard");
	m_protocolCombo->addItem("UCI");

	connect(m_browseCmdBtn, SIGNAL(clicked(bool)), this, SLOT(browseCommand()));
	connect(m_browseWorkingDirBtn, SIGNAL(clicked(bool)), this,
		SLOT(browseWorkingDir()));
}

void EngineConfigurationDialog::applyEngineInformation(
	const EngineConfiguration& engine)
{
	m_nameEdit->setText(engine.name());
	m_commandEdit->setText(engine.command());
	m_workingDirEdit->setText(engine.workingDirectory());
	m_protocolCombo->setCurrentIndex(engine.protocol());
}

EngineConfiguration EngineConfigurationDialog::engineConfiguration()
{
	EngineConfiguration engine;
	engine.setName(m_nameEdit->text());
	engine.setCommand(m_commandEdit->text());
	engine.setWorkingDirectory(m_workingDirEdit->text());
	engine.setProtocol(EngineConfiguration::ChessEngineProtocol(
		m_protocolCombo->currentIndex()));

	return engine;
}

void EngineConfigurationDialog::browseCommand()
{
	// Use file extensions only on Windows
	#ifdef Q_WS_WIN
	QString filter = tr("Executables (*.exe *.bat *.cmd);;All Files (*.*)");
	#else
	QString filter = tr("All Files (*)");
	#endif

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Select Engine Executable"), m_commandEdit->text(), filter);
	if (fileName.isEmpty())
		return;

	if (m_workingDirEdit->text().isEmpty())
	{
		QString path(QFileInfo(fileName).absolutePath());
		m_workingDirEdit->setText(QDir::toNativeSeparators(path));
	}

	fileName = QDir::toNativeSeparators(fileName);
	// Paths with spaces must be wrapped in quotes
	if (fileName.contains(' '))
	{
		fileName.push_front('\"');
		fileName.push_back('\"');
	}
	m_commandEdit->setText(fileName);
}

void EngineConfigurationDialog::browseWorkingDir()
{
	const QString directory = QFileDialog::getExistingDirectory(this,
		tr("Select Engine Working Directory"), m_workingDirEdit->text());
	if (directory.isEmpty())
		return;

	m_workingDirEdit->setText(QDir::toNativeSeparators(directory));
}


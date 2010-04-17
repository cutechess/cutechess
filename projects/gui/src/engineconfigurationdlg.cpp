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

#include "engineconfigurationdlg.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <chessengine.h>


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
		setWindowTitle(tr("Configure Engine"));
	}

	QStringList protocols = ChessEngine::registry()->items().keys();
	m_protocolCombo->addItems(protocols);

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

	int i = m_protocolCombo->findText(engine.protocol());
	m_protocolCombo->setCurrentIndex(i);
}

EngineConfiguration EngineConfigurationDialog::engineConfiguration()
{
	EngineConfiguration engine;
	engine.setName(m_nameEdit->text());
	engine.setCommand(m_commandEdit->text());
	engine.setWorkingDirectory(m_workingDirEdit->text());
	engine.setProtocol(m_protocolCombo->currentText());

	return engine;
}

void EngineConfigurationDialog::browseCommand()
{
	// Use file extensions only on Windows
	#ifdef Q_WS_WIN
	const QString filter = tr("Executables (*.exe *.bat *.cmd);;All Files (*.*)");
	#else
	const QString filter = tr("All Files (*)");
	#endif

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Select Engine Executable"), m_commandEdit->text(), filter);
	if (fileName.isEmpty())
		return;

	if (m_workingDirEdit->text().isEmpty())
	{
		m_workingDirEdit->setText(QDir::toNativeSeparators(
			QFileInfo(fileName).absolutePath()));
	}

	if (m_nameEdit->text().isEmpty())
	{
		m_nameEdit->setText(QFileInfo(fileName).baseName());
	}

	// Paths with spaces must be wrapped in quotes
	if (fileName.contains(' '))
	{
		fileName.prepend('\"');
		fileName.append('\"');
	}
	m_commandEdit->setText(QDir::toNativeSeparators(fileName));
}

void EngineConfigurationDialog::browseWorkingDir()
{
	const QString directory = QFileDialog::getExistingDirectory(this,
		tr("Select Engine Working Directory"), m_workingDirEdit->text());
	if (directory.isEmpty())
		return;

	m_workingDirEdit->setText(QDir::toNativeSeparators(directory));
}


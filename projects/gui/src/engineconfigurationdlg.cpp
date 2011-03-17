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

#include <enginefactory.h>
#include <engineoption.h>
#include <chessplayer.h>
#include <enginebuilder.h>

#include "engineoptionmodel.h"
#include "engineoptiondelegate.h"

EngineConfigurationDialog::EngineConfigurationDialog(
	EngineConfigurationDialog::DialogMode mode, QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);

	if (mode == EngineConfigurationDialog::AddEngine)
		setWindowTitle(tr("Add Engine"));
	else
		setWindowTitle(tr("Configure Engine"));

	m_protocolCombo->addItems(EngineFactory::protocols());

	m_engineOptionModel = new EngineOptionModel(this);
	m_optionsView->setModel(m_engineOptionModel);
	m_optionsView->setItemDelegate(new EngineOptionDelegate());

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

	m_initStringEdit->setPlainText(engine.initStrings().join("\n"));

	if (engine.whiteEvalPov())
		m_whitePovCheck->setCheckState(Qt::Checked);

	foreach (EngineOption* option, engine.options())
		m_options << option->copy();
	m_engineOptionModel->setOptions(m_options);
}

EngineConfiguration EngineConfigurationDialog::engineConfiguration()
{
	EngineConfiguration engine;
	engine.setName(m_nameEdit->text());
	engine.setCommand(m_commandEdit->text());
	engine.setWorkingDirectory(m_workingDirEdit->text());
	engine.setProtocol(m_protocolCombo->currentText());

	QString initStr(m_initStringEdit->toPlainText());
	if (!initStr.isEmpty())
		engine.setInitStrings(initStr.split('\n'));

	engine.setWhiteEvalPov(m_whitePovCheck->checkState() == Qt::Checked);

	engine.setOptions(m_options);  // takes ownership of option pointers

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
		m_workingDirEdit->setText(QDir::toNativeSeparators(
			QFileInfo(fileName).absolutePath()));

	if (m_nameEdit->text().isEmpty())
		m_nameEdit->setText(QFileInfo(fileName).baseName());

	// Paths with spaces must be wrapped in quotes
	if (fileName.contains(' '))
	{
		fileName.prepend('\"');
		fileName.append('\"');
	}
	m_commandEdit->setText(QDir::toNativeSeparators(fileName));

	EngineBuilder builder(engineConfiguration());
	ChessPlayer* engine = builder.create(0, 0, this);
	if (engine != 0)
		connect(engine, SIGNAL(ready()), this, SLOT(onEngineReady()));
}

void EngineConfigurationDialog::browseWorkingDir()
{
	const QString directory = QFileDialog::getExistingDirectory(this,
		tr("Select Engine Working Directory"), m_workingDirEdit->text());

	if (directory.isEmpty())
		return;

	m_workingDirEdit->setText(QDir::toNativeSeparators(directory));
}

void EngineConfigurationDialog::onEngineReady()
{
	ChessEngine* engine = qobject_cast<ChessEngine*>(QObject::sender());
	Q_ASSERT(engine != 0);

	// make copies of the engine options
	foreach (EngineOption* option, engine->options())
		m_options << option->copy();

	engine->quit();
	m_engineOptionModel->setOptions(m_options);
}

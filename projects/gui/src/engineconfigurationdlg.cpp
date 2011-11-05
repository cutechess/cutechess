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
#include "ui_engineconfigdlg.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

#include <enginefactory.h>
#include <engineoption.h>
#include <chessplayer.h>
#include <enginebuilder.h>

#include "engineoptionmodel.h"
#include "engineoptiondelegate.h"

EngineConfigurationDialog::EngineConfigurationDialog(
	EngineConfigurationDialog::DialogMode mode, QWidget* parent)
	: QDialog(parent),
	  m_engineOptionModel(new EngineOptionModel(this)),
	  m_engine(0),
	  ui(new Ui::EngineConfigurationDialog)
{
	ui->setupUi(this);

	if (mode == EngineConfigurationDialog::AddEngine)
		setWindowTitle(tr("Add Engine"));
	else
		setWindowTitle(tr("Configure Engine"));

	ui->m_progressBar->setRange(0, 0);
	ui->m_progressBar->hide();

	ui->m_protocolCombo->addItems(EngineFactory::protocols());

	ui->m_optionsView->setModel(m_engineOptionModel);
	ui->m_optionsView->setItemDelegate(new EngineOptionDelegate());

	m_optionDetectionTimer = new QTimer(this);
	m_optionDetectionTimer->setSingleShot(true);
	m_optionDetectionTimer->setInterval(5000);

	connect(ui->m_browseCmdBtn, SIGNAL(clicked(bool)),
		this, SLOT(browseCommand()));
	connect(ui->m_browseWorkingDirBtn, SIGNAL(clicked(bool)),
		this, SLOT(browseWorkingDir()));
	connect(ui->m_detectBtn, SIGNAL(clicked()),
		this, SLOT(detectEngineOptions()));
	connect(ui->m_restoreBtn, SIGNAL(clicked()),
		this, SLOT(restoreDefaults()));
	connect(ui->m_tabs, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChanged(int)));
	connect(ui->m_buttonBox, SIGNAL(accepted()),
		this, SLOT(onAccepted()));
}

EngineConfigurationDialog::~EngineConfigurationDialog()
{
	qDeleteAll(m_options);
	delete ui;
}

void EngineConfigurationDialog::applyEngineInformation(
	const EngineConfiguration& engine)
{
	ui->m_nameEdit->setText(engine.name());
	ui->m_commandEdit->setText(engine.command());
	ui->m_workingDirEdit->setText(engine.workingDirectory());

	int i = ui->m_protocolCombo->findText(engine.protocol());
	ui->m_protocolCombo->setCurrentIndex(i);

	ui->m_initStringEdit->setPlainText(engine.initStrings().join("\n"));

	if (engine.whiteEvalPov())
		ui->m_whitePovCheck->setCheckState(Qt::Checked);

	foreach (EngineOption* option, engine.options())
		m_options << option->copy();
	m_engineOptionModel->setOptions(m_options);
	ui->m_restoreBtn->setDisabled(m_options.isEmpty());

	m_variants = engine.supportedVariants();

	m_oldCommand = engine.command();
	m_oldPath = engine.workingDirectory();
	m_oldProtocol = engine.protocol();
}

EngineConfiguration EngineConfigurationDialog::engineConfiguration()
{
	EngineConfiguration engine;
	engine.setName(ui->m_nameEdit->text());
	engine.setCommand(ui->m_commandEdit->text());
	engine.setWorkingDirectory(ui->m_workingDirEdit->text());
	engine.setProtocol(ui->m_protocolCombo->currentText());

	QString initStr(ui->m_initStringEdit->toPlainText());
	if (!initStr.isEmpty())
		engine.setInitStrings(initStr.split('\n'));

	engine.setWhiteEvalPov(ui->m_whitePovCheck->checkState() == Qt::Checked);

	QList<EngineOption*> optionCopies;
	foreach (EngineOption* option, m_options)
		optionCopies << option->copy();

	engine.setOptions(optionCopies);

	engine.setSupportedVariants(m_variants);

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
		tr("Select Engine Executable"), ui->m_commandEdit->text(), filter);

	if (fileName.isEmpty())
		return;

	if (ui->m_workingDirEdit->text().isEmpty())
		ui->m_workingDirEdit->setText(QDir::toNativeSeparators(
			QFileInfo(fileName).absolutePath()));

	if (ui->m_nameEdit->text().isEmpty())
		ui->m_nameEdit->setText(QFileInfo(fileName).baseName());

	// Paths with spaces must be wrapped in quotes
	if (fileName.contains(' '))
	{
		fileName.prepend('\"');
		fileName.append('\"');
	}
	ui->m_commandEdit->setText(QDir::toNativeSeparators(fileName));
}

void EngineConfigurationDialog::browseWorkingDir()
{
	const QString directory = QFileDialog::getExistingDirectory(this,
		tr("Select Engine Working Directory"), ui->m_workingDirEdit->text());

	if (directory.isEmpty())
		return;

	ui->m_workingDirEdit->setText(QDir::toNativeSeparators(directory));
}

void EngineConfigurationDialog::detectEngineOptions()
{
	if (m_engine != 0)
		return;

	if (QObject::sender() != ui->m_detectBtn
	&&  ui->m_commandEdit->text() == m_oldCommand
	&&  ui->m_workingDirEdit->text() == m_oldPath
	&&  ui->m_protocolCombo->currentText() == m_oldProtocol)
	{
		emit detectionFinished();
		return;
	}

	m_oldCommand = ui->m_commandEdit->text();
	m_oldPath = ui->m_workingDirEdit->text();
	m_oldProtocol = ui->m_protocolCombo->currentText();

	ui->m_detectBtn->setEnabled(false);
	ui->m_restoreBtn->setEnabled(false);
	ui->m_progressBar->show();

	EngineBuilder builder(engineConfiguration());
	m_engine = qobject_cast<ChessEngine*>(builder.create(0, 0, this));

	if (m_engine != 0)
	{
		connect(m_engine, SIGNAL(ready()),
			this, SLOT(onEngineReady()));
		connect(m_engine, SIGNAL(disconnected()),
			this, SLOT(onEngineQuit()));
		connect(m_engine, SIGNAL(destroyed()),
			this, SIGNAL(detectionFinished()));
		connect(m_optionDetectionTimer, SIGNAL(timeout()),
			m_engine, SLOT(kill()));

		m_optionDetectionTimer->start();
	}
	else
	{
		ui->m_detectBtn->setEnabled(true);
		ui->m_restoreBtn->setEnabled(true);
		ui->m_progressBar->hide();
		emit detectionFinished();
	}
}

void EngineConfigurationDialog::onEngineReady()
{
	Q_ASSERT(m_engine != 0);
	Q_ASSERT(m_engine->state() != ChessPlayer::Disconnected);

	qDeleteAll(m_options);
	m_options.clear();

	// Make copies of the engine options
	foreach (const EngineOption* option, m_engine->options())
		m_options << option->copy();

	m_engineOptionModel->setOptions(m_options);
	m_variants = m_engine->variants();

	m_engine->quit();
}

void EngineConfigurationDialog::onEngineQuit()
{
	Q_ASSERT(m_engine != 0);
	Q_ASSERT(m_engine->state() == ChessPlayer::Disconnected);

	disconnect(m_optionDetectionTimer, 0, m_engine, 0);
	m_optionDetectionTimer->stop();

	m_engine->deleteLater();
	m_engine = 0;

	ui->m_detectBtn->setEnabled(true);
	ui->m_restoreBtn->setDisabled(m_options.isEmpty());
	ui->m_progressBar->hide();
}

void EngineConfigurationDialog::onTabChanged(int index)
{
	if (index == 1)
		detectEngineOptions();
}

void EngineConfigurationDialog::onAccepted()
{
	connect(this, SIGNAL(detectionFinished()),
		this, SLOT(accept()));
	detectEngineOptions();
}

void EngineConfigurationDialog::restoreDefaults()
{
	foreach (EngineOption* option, m_options)
		option->setValue(option->defaultValue());

	m_engineOptionModel->setOptions(m_options);
}

/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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
#include <QMessageBox>
#include <QSettings>

#include <enginefactory.h>
#include <engineoption.h>
#include <chessplayer.h>
#include <enginebuilder.h>

#include "engineoptionmodel.h"
#include "engineoptiondelegate.h"

#ifdef QT_DEBUG
#include <modeltest.h>
#endif

EngineConfigurationDialog::EngineConfigurationDialog(
	EngineConfigurationDialog::DialogMode mode, QWidget* parent)
	: QDialog(parent),
          m_hasError(false),
	  m_engineOptionModel(new EngineOptionModel(this)),
	  m_engine(nullptr),
	  ui(new Ui::EngineConfigurationDialog)
{
	ui->setupUi(this);

	#ifdef QT_DEBUG
	new ModelTest(m_engineOptionModel, this);
	#endif

	if (mode == EngineConfigurationDialog::AddEngine)
		setWindowTitle(tr("Add Engine"));
	else
		setWindowTitle(tr("Configure Engine"));

	ui->m_progressBar->setRange(0, 0);
	ui->m_progressBar->hide();

	ui->m_protocolCombo->addItems(EngineFactory::protocols());

	ui->m_optionsView->setModel(m_engineOptionModel);
	connect(m_engineOptionModel, SIGNAL(modelReset()),
		this, SLOT(resizeColumns()));

	EngineOptionDelegate* delegate = new EngineOptionDelegate(this);
	ui->m_optionsView->setItemDelegate(delegate);
	connect(ui->m_workingDirEdit, SIGNAL(textChanged(QString)),
		delegate, SLOT(setEngineDirectory(QString)));

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
	connect(ui->m_nameEdit, SIGNAL(textChanged(QString)),
		this, SLOT(onNameOrCommandChanged()));
	connect(ui->m_commandEdit, SIGNAL(textChanged(QString)),
		this, SLOT(onNameOrCommandChanged()));
	connect(ui->m_buttonBox, SIGNAL(accepted()),
		this, SLOT(onAccepted()));
	connect(ui->m_protocolCombo, &QComboBox::currentTextChanged,
		[=](const QString& text)
	{
		if (text == "xboard")
			ui->m_whitePovCheck->setEnabled(true);
		else
		{
			ui->m_whitePovCheck->setChecked(false);
			ui->m_whitePovCheck->setEnabled(false);
		}
	});

	ui->m_tabs->setTabEnabled(1, false);
	ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
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

	const auto options = engine.options();
	for (const EngineOption* option : options)
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
	for (const EngineOption* option : qAsConst(m_options))
		optionCopies << option->copy();

	engine.setOptions(optionCopies);

	engine.setSupportedVariants(m_variants);

	return engine;
}

void EngineConfigurationDialog::setReservedNames(const QSet<QString>& names)
{
	m_reservedNames = names;
}

void EngineConfigurationDialog::browseCommand()
{
	// Use file extensions only on Windows
	#ifdef Q_OS_WIN32
	const QString filter = tr("Executables (*.exe *.bat *.cmd);;All Files (*.*)");
	#else
	const QString filter = tr("All Files (*)");
	#endif

	// Try to open the dialog in a directory that best matches the
	// previous command and working directory
	QString defaultDir;
	const QFileInfo oldInfo(ui->m_commandEdit->text());
	if (oldInfo.exists())
		defaultDir = oldInfo.absoluteFilePath();
	else
	{
		const QString relFilePath = oldInfo.filePath();
		const QString wdir = ui->m_workingDirEdit->text();
		const QDir dir(wdir);

		if (!relFilePath.isEmpty() && dir.exists(relFilePath))
			defaultDir = dir.absoluteFilePath(relFilePath);
		else if (!wdir.isEmpty() && dir.exists())
			defaultDir = dir.absolutePath();
		else
			defaultDir = QSettings().value("ui/default_engine_location").toString();
	}

	auto dlg = new QFileDialog(
		this, tr("Select Engine Executable"),
		defaultDir, filter);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptOpen);


	connect(dlg, &QFileDialog::fileSelected,
		this, &EngineConfigurationDialog::setExecutable);
	dlg->open();
}

void EngineConfigurationDialog::setExecutable(const QString& file)
{
	QString fileName = file;
	const QFileInfo info(fileName);
	QString prefix;
	QString suffix(info.suffix().toLower());
	bool isExe = (suffix == "exe" || suffix == "bat" || suffix == "cmd");
	QString name = isExe ? info.completeBaseName() : info.fileName();

	if (ui->m_workingDirEdit->text().isEmpty())
		ui->m_workingDirEdit->setText(QDir::toNativeSeparators(
			info.absolutePath()));

	if (ui->m_nameEdit->text().isEmpty())
		ui->m_nameEdit->setText(name);

	// Use a relative path in the "command" field if possible
	const QDir dir(ui->m_workingDirEdit->text());
	if (info.absoluteFilePath().startsWith(dir.absolutePath()))
	{
		fileName = dir.relativeFilePath(info.absoluteFilePath());

		#ifndef Q_OS_WIN32
		if (isExe)
			prefix = "wine ";
		else if (fileName == info.fileName())
			fileName.prepend("./");
		#endif
	}

	// Paths with spaces must be wrapped in quotes
	if (fileName.contains(' '))
	{
		fileName.prepend('\"');
		fileName.append('\"');
	}

	ui->m_commandEdit->setText(QDir::toNativeSeparators(prefix + fileName));
}

void EngineConfigurationDialog::browseWorkingDir()
{
	auto dlg = new QFileDialog(
		this, tr("Select Engine Working Directory"),
		ui->m_workingDirEdit->text());
	dlg->setFileMode(QFileDialog::Directory);
	dlg->setOption(QFileDialog::ShowDirsOnly);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptOpen);

	connect(dlg, &QFileDialog::fileSelected, [=](const QString& dir)
	{
		ui->m_workingDirEdit->setText(QDir::toNativeSeparators(dir));
	});
	dlg->open();
}

void EngineConfigurationDialog::detectEngineOptions()
{
	if (m_engine != nullptr)
		return;

	if (!m_hasError
	&&  QObject::sender() != ui->m_detectBtn
	&&  ui->m_commandEdit->text() == m_oldCommand
	&&  ui->m_workingDirEdit->text() == m_oldPath
	&&  ui->m_protocolCombo->currentText() == m_oldProtocol)
	{
		emit detectionFinished();
		return;
	}
	m_hasError = false;

	m_oldCommand = ui->m_commandEdit->text();
	m_oldPath = ui->m_workingDirEdit->text();
	m_oldProtocol = ui->m_protocolCombo->currentText();

	ui->m_detectBtn->setEnabled(false);
	ui->m_restoreBtn->setEnabled(false);
	ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	ui->m_buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
	ui->m_progressBar->show();

	EngineBuilder builder(engineConfiguration());
	QString error;
	m_engine = qobject_cast<ChessEngine*>(builder.create(nullptr, nullptr, this, &error));

	if (m_engine != nullptr)
	{
		connect(m_engine, SIGNAL(ready()),
			this, SLOT(onEngineReady()));
		connect(m_engine, SIGNAL(disconnected()),
			this, SLOT(onEngineQuit()));
		connect(m_engine, SIGNAL(destroyed()),
			this, SIGNAL(detectionFinished()));
	}
	else
	{
		m_hasError = true;
		QMessageBox::critical(this, tr("Engine Error"), error);

		ui->m_detectBtn->setEnabled(true);
		ui->m_restoreBtn->setEnabled(true);
		ui->m_progressBar->hide();
		emit detectionFinished();
	}
}

void EngineConfigurationDialog::onEngineReady()
{
	Q_ASSERT(m_engine != nullptr);
	Q_ASSERT(m_engine->state() != ChessPlayer::Disconnected);

	qDeleteAll(m_options);
	m_options.clear();

	// Make copies of the engine options
	const auto options = m_engine->options();
	for (const EngineOption* option : options)
		m_options << option->copy();

	m_engineOptionModel->setOptions(m_options);
	m_variants = m_engine->variants();

	m_engine->quit();
}

void EngineConfigurationDialog::onEngineQuit()
{
	if (m_engine != nullptr)
	{
		Q_ASSERT(m_engine->state() == ChessPlayer::Disconnected);
		if (m_engine->hasError())
		{
			m_hasError = true;
			QMessageBox::critical(this, tr("Engine Error"),
			                      m_engine->errorString());
		}
		m_engine->deleteLater();
		m_engine = nullptr;
	}

	ui->m_detectBtn->setEnabled(true);
	ui->m_restoreBtn->setDisabled(m_options.isEmpty());
	ui->m_progressBar->hide();
	ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	ui->m_buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
}

void EngineConfigurationDialog::onTabChanged(int index)
{
	if (index == 1)
		detectEngineOptions();
}

void EngineConfigurationDialog::onNameOrCommandChanged()
{
	bool ok = !ui->m_nameEdit->text().isEmpty() &&
		  !ui->m_commandEdit->text().isEmpty();

	ui->m_tabs->setTabEnabled(1, ok);
	ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

void EngineConfigurationDialog::onDetectionFinished()
{
	if (!m_hasError)
		accept();
}

void EngineConfigurationDialog::onAccepted()
{
	if (m_reservedNames.contains(ui->m_nameEdit->text()))
	{
		QMessageBox::warning(this, tr("Duplicate engine name"),
				     tr("An engine with this name already exists"));
		return;
	}

	connect(this, SIGNAL(detectionFinished()),
	        this, SLOT(onDetectionFinished()));
	detectEngineOptions();
}

void EngineConfigurationDialog::resizeColumns()
{
	for (int i = 0; i < m_engineOptionModel->columnCount(); i++)
		ui->m_optionsView->resizeColumnToContents(i);
}

void EngineConfigurationDialog::restoreDefaults()
{
	for (EngineOption* option : qAsConst(m_options))
		option->setValue(option->defaultValue());

	m_engineOptionModel->setOptions(m_options);
}

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

#include "importprogressdlg.h"
#include "ui_importprogressdlg.h"
#include "pgnimporter.h"

#include <QFileInfo>

ImportProgressDialog::ImportProgressDialog(PgnImporter* pgnImporter,
					   QWidget* parent)
	: QDialog(parent),
	  m_lastUpdateSecs(0),
	  m_importError(false),
	  ui(new Ui::ImportProgressDialog)
{
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);

	QFileInfo info(pgnImporter->fileName());

	ui->m_fileNameLabel->setText(
	    QString(tr("Importing \"%1\"").arg(info.fileName())));

	m_totalFileSize = info.size();

	connect(ui->m_buttonBox, SIGNAL(rejected()), pgnImporter,
		SLOT(cancel()));
	connect(pgnImporter, SIGNAL(finished()), this,
		SLOT(onImporterFinished()));
	connect(pgnImporter, SIGNAL(error(int)), this,
		SLOT(onImportError(int)));
	connect(pgnImporter, SIGNAL(databaseReadStatus(const QTime&, int, qint64)),
		this, SLOT(updateImportStatus(const QTime&, int, qint64)));
}

ImportProgressDialog::~ImportProgressDialog()
{
	delete ui;
}

void ImportProgressDialog::updateImportStatus(const QTime& startTime,
                                             int numReadGames, qint64 numReadBytes)
{
	int elapsed = startTime.secsTo(QTime::currentTime());
	if (elapsed == 0)
		return;

	// Update the status once a second
	if (elapsed <= m_lastUpdateSecs)
		return;

	m_lastUpdateSecs = elapsed;

	ui->m_importProgressBar->setMinimum(0);
	ui->m_importProgressBar->setMaximum(100);
	ui->m_importProgressBar->setValue(int((double(numReadBytes) / m_totalFileSize) * 100));

	int remainingSecs = (m_totalFileSize - numReadBytes) / (numReadBytes / elapsed);

	ui->m_statusLabel->setText(QString(tr("%1 games/sec - %2")).arg((int)numReadGames / elapsed)
	    .arg(humaniseTime(remainingSecs)));
}

void ImportProgressDialog::onImporterFinished()
{
	if (!m_importError)
		accept();  // close the dialog automatically if no error occured
}

void ImportProgressDialog::onImportError(int error)
{
	m_importError = true;
	setWindowTitle(tr("Import failed"));

	switch (error)
	{
		case PgnImporter::FileDoesNotExist:
			ui->m_statusLabel->setText(tr("Import failed: file does not exist"));
			break;

		case PgnImporter::IoError:
			ui->m_statusLabel->setText(tr("Import failed: I/O error"));
			break;

		default:
			ui->m_statusLabel->setText(tr("Import failed: unknown error"));
			break;
	}

	// replace the cancel button with close button because the
	// the import operation is already finished
	ui->m_buttonBox->clear();
	ui->m_buttonBox->addButton(QDialogButtonBox::Close);
}

QString ImportProgressDialog::humaniseTime(int sec)
{
	if (sec <= 5)
		return QString(tr("About 5 seconds"));

	if (sec <= 10)
		return QString(tr("About 10 seconds"));

	if (sec <= 60)
		return QString(tr("Less than a minute"));

	if (sec <= 120)
		return QString(tr("About a minute"));

	return QString(tr("About %1 minutes").arg(sec / 60));
}

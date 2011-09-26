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

#include "importprogressdlg.h"
#include "ui_importprogressdlg.h"
#include "pgnimporter.h"

#include <QFileInfo>

ImportProgressDialog::ImportProgressDialog(PgnImporter* pgnImporter,
					   QWidget* parent)
	: QDialog(parent),
	  m_pgnImporter(pgnImporter),
	  m_lastUpdateSecs(0),
	  m_importError(false),
	  ui(new Ui::ImportProgressDialog)
{
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);

	QFileInfo info(m_pgnImporter->fileName());

	ui->m_fileNameLabel->setText(info.fileName());
	ui->m_statusLabel->setText(tr("Importing"));
	ui->m_totalGamesLabel->setText(tr("0 games imported"));

	connect(ui->m_buttonBox, SIGNAL(rejected()), m_pgnImporter,
		SLOT(abort()));
	connect(m_pgnImporter, SIGNAL(finished()), this,
		SLOT(onImporterFinished()));
	connect(m_pgnImporter, SIGNAL(error(int)), this,
		SLOT(onImportError(int)));
	connect(m_pgnImporter, SIGNAL(databaseReadStatus(const QTime&, int)),
		this, SLOT(updateImportStatus(const QTime&, int)));
}

ImportProgressDialog::~ImportProgressDialog()
{
	delete ui;
}

void ImportProgressDialog::updateImportStatus(const QTime& startTime,
                                             int numReadGames)
{
	int elapsed = startTime.secsTo(QTime::currentTime());
	if (elapsed == 0)
		return;

	// Update the status once a second
	if (elapsed <= m_lastUpdateSecs)
		return;

	m_lastUpdateSecs = elapsed;

	ui->m_statusLabel->setText(
		QString(tr("Importing, %1 games/sec")).arg((int)numReadGames / elapsed));
	ui->m_totalGamesLabel->setText(
		QString(tr("%1 games imported")).arg(numReadGames));
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

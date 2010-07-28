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
#include "pgnimporter.h"

#include <QFileInfo>

ImportProgressDialog::ImportProgressDialog(PgnImporter* pgnImporter,
                                           QWidget* parent)
	: QDialog(parent),
	  m_pgnImporter(pgnImporter),
	  m_lastUpdateSecs(0)
{
	setupUi(this);

	QFileInfo info(m_pgnImporter->fileName());

	m_importSourceLabel->setText(info.fileName());
	m_importSpeedLabel->setText(tr("Unknown"));
	m_importTotalLabel->setText(tr("Unknown"));

	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(abortImport()));
	connect(m_pgnImporter, SIGNAL(finished()), this, SLOT(accept()));
	connect(m_pgnImporter, SIGNAL(databaseReadStatus(const QTime&, int)),
		this, SLOT(updateImportStatus(const QTime&, int)));
}

void ImportProgressDialog::abortImport()
{
	m_pgnImporter->abort();
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

	m_importSpeedLabel->setText(
		QString("%1 games per second").arg((int)numReadGames / elapsed ));
	m_importTotalLabel->setText(
		QString("%1 games").arg(numReadGames));
}

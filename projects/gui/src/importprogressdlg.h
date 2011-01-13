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

#ifndef IMPORT_PROGRESS_DIALOG_H
#define IMPORT_PROGRESS_DIALOG_H

#include "ui_importprogressdlg.h"
#include <QDialog>

class PgnImporter;

class ImportProgressDialog : public QDialog, private Ui::ImportProgressDialog
{
	Q_OBJECT

	public:
		ImportProgressDialog(PgnImporter* pgnImporter);

	private slots:
		void abortImport();
		void updateImportStatus(const QTime& startTime, int numReadGames);

	private:
		PgnImporter* m_pgnImporter;
		int m_lastUpdateSecs;

};

#endif // IMPORT_PROGRESS_DIALOG_H


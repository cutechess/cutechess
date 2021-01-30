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

#ifndef RESULTFORMATDLG_H
#define RESULTFORMATDLG_H

#include <QDialog>

namespace Ui {
	class ResultFormatDlg;
}

class ResultFormatDlg : public QDialog
{
	Q_OBJECT

	public:
		/* Creates a new ResultFormatDlg */
		explicit ResultFormatDlg(QWidget* parent = nullptr);
		virtual ~ResultFormatDlg();
		/*!
		 * Sets available result formats according to map \a formats.
		 * The current format is given by \a currentFormat.
		 */
		void setFormats(const QMap<QString, QString>& formats,
				const QString& currentFormat = "default");
		/*! Returns the selected result format. */
		QString resultFormat();

	private:
		Ui::ResultFormatDlg *ui;
};

#endif // RESULTFORMATDLG_H

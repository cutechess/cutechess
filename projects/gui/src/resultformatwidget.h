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

#ifndef RESULTFORMATWIDGET_H
#define RESULTFORMATWIDGET_H

#include <QWidget>

namespace Ui {
	class ResultFormatWidget;
}

class ResultFormatWidget : public QWidget
{
	Q_OBJECT

	public:
		/* Creates a new ResultFormatWidget. */
		explicit ResultFormatWidget(QWidget* parent = nullptr);
		virtual ~ResultFormatWidget();
		/*!
		 * Sets available result formats according to map \a formats.
		 * The current format is given by \a currentFormat.
		 */
		void setFormats(const QMap<QString, QString>& formats,
				const QString& currentFormat = "default");
		/*! Returns the selected result format. */
		QString resultFormat();

	private:
		Ui::ResultFormatWidget *ui;
};

#endif // RESULTFORMATWIDGET_H

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

#ifndef PATHLINEEDIT_H
#define PATHLINEEDIT_H

#include <QLineEdit>
#include <QDir>
class QToolButton;

class PathLineEdit : public QLineEdit
{
	Q_OBJECT

	public:
		enum PathType
		{
			FilePath,
			FolderPath
		};

		explicit PathLineEdit(PathType pathType, QWidget* parent = nullptr);

		void setDefaultDirectory(const QString& dir);

	protected:
		virtual void resizeEvent(QResizeEvent* event);

	private slots:
		void browse();

	private:
		PathType m_pathType;
		QToolButton* m_browseBtn;
		QDir m_defaultDir;
};

#endif // PATHLINEEDIT_H

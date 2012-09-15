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

#include "pathlineedit.h"
#include <QFileDialog>
#include <QToolButton>
#include <QResizeEvent>

PathLineEdit::PathLineEdit(PathType pathType, QWidget* parent)
	: QLineEdit(parent),
	  m_pathType(pathType)
{
	m_browseBtn = new QToolButton(this);
	m_browseBtn->setText("...");

	// Focus must stay on the QLineEdit, otherwise the editor
	// will be closed prematurely
	m_browseBtn->setCursor(Qt::ArrowCursor);
	m_browseBtn->setFocusPolicy(Qt::NoFocus);

	connect(m_browseBtn, SIGNAL(clicked()), this, SLOT(browse()));
}

void PathLineEdit::resizeEvent(QResizeEvent* event)
{
	int height = event->size().height();

	// Place the browse button to the right
	setContentsMargins(0, 0, height, 0);
	m_browseBtn->resize(height, height);
	m_browseBtn->move(width() - height, 0);
}

void PathLineEdit::browse()
{
	QFileDialog dlg(this);
	if (m_pathType == FilePath)
	{
		dlg.setFileMode(QFileDialog::AnyFile);
	}
	else
	{
		dlg.setFileMode(QFileDialog::Directory);
		dlg.setOption(QFileDialog::ShowDirsOnly);
	}

	if (dlg.exec() == QDialog::Accepted)
	{
		QString val(dlg.selectedFiles().first());
		if (!val.isEmpty())
			setText(val);
	}
}

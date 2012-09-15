#include "pathlineedit.h"
#include <QDirModel>
#include <QFileDialog>
#include <QToolButton>
#include <QtEvents>

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

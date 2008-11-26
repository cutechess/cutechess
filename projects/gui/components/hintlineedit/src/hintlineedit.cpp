/*
    Copyright (c) 2008 Arto Jonsson

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

#include "hintlineedit.h"

HintLineEdit::HintLineEdit(QWidget* parent)
	: QLineEdit(parent)
{
}

HintLineEdit::HintLineEdit(const QString& contents, QWidget* parent)
	: QLineEdit(contents, parent)
{
}

void HintLineEdit::setHintText(const QString& hint)
{
	m_hintText = hint;
}

QString HintLineEdit::hintText() const
{
	return m_hintText;
}

void HintLineEdit::paintEvent(QPaintEvent* event)
{
	QLineEdit::paintEvent(event);

	if (!hasFocus() && text().isEmpty() && !m_hintText.isEmpty())
	{
		QPainter painter(this);
		painter.setPen(palette().brush(QPalette::Disabled, QPalette::Text).color());

		QStyleOptionFrameV2 option;
		initStyleOption(&option);

		QRect contentsRect = style()->subElementRect(QStyle::SE_LineEditContents,
			&option, this);

		// Handle right-to-left and left-to-right
		if (layoutDirection() == Qt::LeftToRight)
		{
			contentsRect.setLeft(contentsRect.x() + style()->pixelMetric(
				QStyle::PM_DefaultFrameWidth, &option, this));
		}
		else
		{
			contentsRect.setRight(contentsRect.width() - style()->pixelMetric(
				QStyle::PM_DefaultFrameWidth, &option, this));
		}

		painter.drawText(contentsRect, QStyle::visualAlignment(layoutDirection(), Qt::AlignLeft | Qt::AlignVCenter),
			m_hintText);
	}
}


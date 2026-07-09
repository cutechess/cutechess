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

#include "engineoptiondelegate.h"

#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QEvent>
#include <QtDebug>
#include "pathlineedit.h"

#ifdef Q_OS_MACOS
#include <QApplication>
#include <QStyle>
#include <QStyleOptionButton>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QAbstractItemModel>

namespace
{
// Rect of the check indicator within a value cell. Ask the style for the exact
// rect it uses for item-view check indicators so the size and vertical
// centering match everything else. Expects an option already passed through
// initStyleOption() (so HasCheckIndicator and widget are set).
QRect macCheckIndicatorRect(const QStyleOptionViewItem& option)
{
	QStyle* style = option.widget ? option.widget->style() : QApplication::style();
	return style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator,
				     &option, option.widget);
}
} // anonymous namespace
#endif

EngineOptionDelegate::EngineOptionDelegate(QWidget* parent)
	: QStyledItemDelegate(parent)
{
}

void EngineOptionDelegate::setEngineDirectory(const QString& dir)
{
	m_engineDir = dir;
}

bool EngineOptionDelegate::eventFilter(QObject* object, QEvent* event)
{
	// Make sure the PathLineEdit object stays alive when the
	// file dialog opens and gets focus.
	if (event->type() == QEvent::FocusOut)
	{
		auto editor = qobject_cast<PathLineEdit*>(object);
		if (editor)
			return false;
	}

	return QStyledItemDelegate::eventFilter(object, event);
}

QWidget* EngineOptionDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
	if (index.data(Qt::EditRole).canConvert(QMetaType(QMetaType::QVariantMap)))
	{
		const QVariantMap map = index.data(Qt::EditRole).toMap();

		if (!map.isEmpty())
		{
			const QString optionType = map.value("type").toString();

			if (optionType == "combo")
			{
				QComboBox* editor = new QComboBox(parent);
				editor->addItems(map.value("choices").toStringList());

				return editor;
			}
			else if (optionType == "spin")
			{
				QSpinBox* editor = new QSpinBox(parent);

				bool ok;
				int minValue = map.value("min").toInt(&ok);
				if (ok)
					editor->setMinimum(minValue);

				int maxValue = map.value("max").toInt(&ok);
				if (ok)
					editor->setMaximum(maxValue);

				return editor;
			}
			else if (optionType == "text")
			{
				QLineEdit* editor = new QLineEdit(parent);
				return editor;
			}
			else if (optionType == "file")
			{
				PathLineEdit* editor = new PathLineEdit(
					PathLineEdit::FilePath, parent);
				return editor;
			}
			else if (optionType == "folder")
			{
				PathLineEdit* editor = new PathLineEdit(
					PathLineEdit::FolderPath, parent);
				return editor;
			}
		}
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void EngineOptionDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const
{
	// TODO: default options

	if (index.data(Qt::EditRole).canConvert(QMetaType(QMetaType::QVariantMap)))
	{
		const QVariantMap map = index.data(Qt::EditRole).toMap();

		if (!map.isEmpty())
		{
			const QString optionType = map.value("type").toString();

			if (optionType == "combo")
			{
				QComboBox* optionEditor = qobject_cast<QComboBox*>(editor);
				optionEditor->setCurrentIndex(
					map.value("choices").toStringList().indexOf(map.value("value").toString()));
			}
			else if (optionType == "spin")
			{
				QSpinBox* optionEditor = qobject_cast<QSpinBox*>(editor);

				bool ok;
				int intValue = map.value("value").toInt(&ok);

				if (ok)
					optionEditor->setValue(intValue);
				else
					optionEditor->setValue(0);
			}
			else if (optionType == "text")
			{
				QLineEdit* optionEditor = qobject_cast<QLineEdit*>(editor);
				optionEditor->setText(map.value("value").toString());
			}
			else if (optionType == "file" || optionType == "folder")
			{
				PathLineEdit* optionEditor = qobject_cast<PathLineEdit*>(editor);
				optionEditor->setText(map.value("value").toString());
				optionEditor->setDefaultDirectory(m_engineDir);
			}
		}
	}
	QStyledItemDelegate::setEditorData(editor, index);
}

void EngineOptionDelegate::setModelData(QWidget* editor,
                                        QAbstractItemModel* model,
                                        const QModelIndex& index) const
{
	if (index.data(Qt::EditRole).canConvert(QMetaType(QMetaType::QVariantMap)))
	{
		const QVariantMap map = index.data(Qt::EditRole).toMap();

		if (!map.isEmpty())
		{
			const QString optionType = map.value("type").toString();

			if (optionType == "combo")
			{
				QComboBox* optionEditor = qobject_cast<QComboBox*>(editor);
				model->setData(index, optionEditor->currentText());
			}
			else if (optionType == "spin")
			{
				QSpinBox* optionEditor = qobject_cast<QSpinBox*>(editor);
				optionEditor->interpretText();
				model->setData(index, optionEditor->value());
			}
		}
	}
	QStyledItemDelegate::setModelData(editor, model, index);
}

#ifdef Q_OS_MACOS
void EngineOptionDelegate::paint(QPainter* painter,
				 const QStyleOptionViewItem& option,
				 const QModelIndex& index) const
{
	const QVariant checkData = index.data(Qt::CheckStateRole);
	if (!checkData.isValid())
	{
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}

	QStyleOptionViewItem opt(option);
	initStyleOption(&opt, index);

	// Resolve the indicator rect while HasCheckIndicator is still set, then
	// paint the cell without it (QMacStyle never draws it anyway).
	const QRect target = macCheckIndicatorRect(opt);
	opt.features &= ~QStyleOptionViewItem::HasCheckIndicator;
	opt.text.clear();
	QStyledItemDelegate::paint(painter, opt, index);

	// Draw the checkbox ourselves. QMacStyle ignores the option rect for
	// PE_IndicatorCheckBox and paints at the painter origin, so render the
	// indicator into an origin-anchored pixmap and blit it into place.
	const qreal ratio = painter->device()->devicePixelRatioF();
	QPixmap pixmap(target.size() * ratio);
	pixmap.setDevicePixelRatio(ratio);
	pixmap.fill(Qt::transparent);

	QPainter pixmapPainter(&pixmap);
	QStyleOptionButton indicator;
	indicator.rect = QRect(QPoint(0, 0), target.size());
	indicator.state = QStyle::State_Enabled
		| (static_cast<Qt::CheckState>(checkData.toInt()) == Qt::Checked
		   ? QStyle::State_On : QStyle::State_Off);
	QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox,
					     &indicator, &pixmapPainter);
	pixmapPainter.end();

	painter->drawPixmap(target.topLeft(), pixmap);
}

bool EngineOptionDelegate::editorEvent(QEvent* event,
				       QAbstractItemModel* model,
				       const QStyleOptionViewItem& option,
				       const QModelIndex& index)
{
	const QVariant checkData = index.data(Qt::CheckStateRole);
	const Qt::ItemFlags flags = index.flags();
	if (!checkData.isValid()
	||  !(flags & Qt::ItemIsUserCheckable)
	||  !(flags & Qt::ItemIsEnabled))
		return QStyledItemDelegate::editorEvent(event, model, option, index);

	// Toggle on a left-button release over the indicator, or Space / Select.
	if (event->type() == QEvent::MouseButtonRelease)
	{
		QStyleOptionViewItem opt(option);
		initStyleOption(&opt, index);
		auto* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() != Qt::LeftButton
		||  !macCheckIndicatorRect(opt).contains(mouseEvent->position().toPoint()))
			return false;
	}
	else if (event->type() == QEvent::KeyPress)
	{
		auto* keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() != Qt::Key_Space && keyEvent->key() != Qt::Key_Select)
			return false;
	}
	else
		return false;

	const Qt::CheckState current = static_cast<Qt::CheckState>(checkData.toInt());
	const Qt::CheckState next = (current == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
	return model->setData(index, next, Qt::CheckStateRole);
}
#endif

#include "engineoptiondelegate.h"

#include <QSpinBox>
#include <QComboBox>

#include <enginecombooption.h>
#include <enginespinoption.h>

EngineOptionDelegate::EngineOptionDelegate(QWidget* parent)
	: QStyledItemDelegate(parent)
{
}

QWidget* EngineOptionDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
	// only combo option and spin option require a custom editor
	if (index.data(Qt::EditRole).canConvert<EngineComboOption>())
	{
		EngineComboOption comboOption = index.data().value<EngineComboOption>();

		QComboBox* editor = new QComboBox(parent);
		editor->addItems(comboOption.choices());

		return editor;
	}
	else if (index.data(Qt::EditRole).canConvert<EngineSpinOption>())
	{
		EngineSpinOption spinOption = index.data().value<EngineSpinOption>();

		QSpinBox* editor = new QSpinBox(parent);
		editor->setMinimum(spinOption.min());
		editor->setMaximum(spinOption.max());

		return editor;
	}
	else
		return QStyledItemDelegate::createEditor(parent, option, index);
}

void EngineOptionDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const
{
	if (index.data().canConvert<EngineComboOption>())
	{
		EngineComboOption comboOption = index.data().value<EngineComboOption>();
		QComboBox* optionEditor = qobject_cast<QComboBox*>(editor);

		if (comboOption.isValid(comboOption.value()))
			optionEditor->setCurrentIndex(comboOption.choices().indexOf(comboOption.value().toString()));
		else if (comboOption.isValid(comboOption.defaultValue()))
			optionEditor->setCurrentIndex(comboOption.choices().indexOf(comboOption.defaultValue().toString()));
		else
		{
			if (optionEditor->count() > 0)
				optionEditor->setCurrentIndex(0);
		}
	}
	else if (index.data().canConvert<EngineSpinOption>())
	{
		EngineSpinOption spinOption = index.data().value<EngineSpinOption>();
		QSpinBox* optionEditor = qobject_cast<QSpinBox*>(editor);

		if (spinOption.isValid(spinOption.value()))
			optionEditor->setValue(spinOption.value().toInt());
		else if (spinOption.isValid(spinOption.defaultValue()))
			optionEditor->setValue(spinOption.defaultValue().toInt());
		else
			optionEditor->setValue(optionEditor->minimum());
	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void EngineOptionDelegate::setModelData(QWidget* editor,
                                        QAbstractItemModel* model,
                                        const QModelIndex& index) const
{
	if (index.data().canConvert<EngineComboOption>())
	{
		QComboBox* optionEditor = qobject_cast<QComboBox*>(editor);
		model->setData(index, optionEditor->currentText());
	}
	else if (index.data().canConvert<EngineSpinOption>())
	{
		QSpinBox* optionEditor = qobject_cast<QSpinBox*>(editor);
		model->setData(index, optionEditor->value());
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

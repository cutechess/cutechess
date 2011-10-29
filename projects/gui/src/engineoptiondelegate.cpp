#include "engineoptiondelegate.h"

#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>

EngineOptionDelegate::EngineOptionDelegate(QWidget* parent)
	: QStyledItemDelegate(parent)
{
}

QWidget* EngineOptionDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
	if (index.data(Qt::EditRole).canConvert(QVariant::Map))
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
		}
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void EngineOptionDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const
{
	// TODO: default options

	if (index.data(Qt::EditRole).canConvert(QVariant::Map))
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
		}
	}
	QStyledItemDelegate::setEditorData(editor, index);
}

void EngineOptionDelegate::setModelData(QWidget* editor,
                                        QAbstractItemModel* model,
                                        const QModelIndex& index) const
{
	if (index.data(Qt::EditRole).canConvert(QVariant::Map))
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
			else if (optionType == "text")
			{
				QLineEdit* optionEditor = qobject_cast<QLineEdit*>(editor);
				model->setData(index, optionEditor->text());
			}
		}
	}
	QStyledItemDelegate::setModelData(editor, model, index);
}

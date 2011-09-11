#ifndef ENGINE_OPTION_DELEGATE_H
#define ENGINE_OPTION_DELEGATE_H

#include <QStyledItemDelegate>

class EngineOptionDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	public:
		EngineOptionDelegate(QWidget* parent = 0);

		// Inherited from QStyledItemDelegate
		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
					      const QModelIndex& index) const;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model,
					  const QModelIndex& index) const;

};

#endif // ENGINE_OPTION_DELEGATE_H

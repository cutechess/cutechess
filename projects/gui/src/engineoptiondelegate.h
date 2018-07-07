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

#ifndef ENGINE_OPTION_DELEGATE_H
#define ENGINE_OPTION_DELEGATE_H

#include <QStyledItemDelegate>

class EngineOptionDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	public:
		EngineOptionDelegate(QWidget* parent = nullptr);

		// Inherited from QStyledItemDelegate
		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
					      const QModelIndex& index) const;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model,
					  const QModelIndex& index) const;

	public slots:
		void setEngineDirectory(const QString& dir);

	protected:
		virtual bool eventFilter(QObject* object, QEvent* event);

	private:
		QString m_engineDir;
};

#endif // ENGINE_OPTION_DELEGATE_H

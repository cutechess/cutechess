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

#ifndef TREE_VIEW_ITEM_H
#define TREE_VIEW_ITEM_H

#include <QList>

class TreeViewItem
{
	public:
		TreeViewItem(TreeViewItem* parent = 0);
		~TreeViewItem();

		void setParent(TreeViewItem* parent);
		TreeViewItem* parent() const;

		void addChild(TreeViewItem* child);
		void addChildAt(TreeViewItem* child, int index);
		QList<TreeViewItem*> children() const;

		void setRow(int row);
		int row() const;

		void setData(void* data);
		void* data() const;

	private:
		TreeViewItem* m_parent;
		int m_row;
		void* m_data;
		QList<TreeViewItem*> m_children;

};

#endif // TREE_VIEW_ITEM_H


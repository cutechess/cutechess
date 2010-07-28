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

#include "treeviewitem.h"

TreeViewItem::TreeViewItem(TreeViewItem* parent)
	: m_parent(parent),
	  m_row(0),
	  m_data(0)
{
}

TreeViewItem::~TreeViewItem()
{
	qDeleteAll(m_children);
}

void TreeViewItem::setParent(TreeViewItem* parent)
{
	m_parent = parent;
}

TreeViewItem* TreeViewItem::parent() const
{
	return m_parent;
}

void TreeViewItem::addChild(TreeViewItem* child)
{
	Q_ASSERT(child != NULL);

	child->setParent(this);
	m_children << child;
}

void TreeViewItem::addChildAt(TreeViewItem* child, int index)
{
	Q_ASSERT(child != NULL);

	child->setParent(this);
	m_children.insert(index, child);

	// Update the row values
	for (int i = index + 1; i < m_children.count(); i++)
		m_children.at(i)->setRow(i);
}

QList<TreeViewItem*> TreeViewItem::children() const
{
	return m_children;
}

void TreeViewItem::setRow(int row)
{
	m_row = row;
}

int TreeViewItem::row() const
{
	return m_row;
}

void TreeViewItem::setData(void* data)
{
	m_data = data;
}

void* TreeViewItem::data() const
{
	return m_data;
}

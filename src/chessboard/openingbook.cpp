/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QString>
#include <QFile>
#include <QDataStream>
#include "openingbook.h"


bool OpeningBook::load(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QDataStream in(&file);
	
	m_map.clear();
	while (!in.atEnd())
		loadEntry(in);
	
	return true;
}

bool OpeningBook::save(const QString& filename) const
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QDataStream out(&file);
	
	Map::const_iterator it;
	for (it = m_map.constBegin(); it != m_map.constEnd(); ++it)
		saveEntry(it, out);
	
	return true;
}

BookMove OpeningBook::move(quint64 key) const
{
	BookMove move = { {0, 0}, {0, 0}, 0 };
	
	// There can be multiple entries/moves with the same key.
	// We need to find the all to choose the best one
	QList<Entry> entries = m_map.values(key);
	if (entries.size() == 0)
		return move;
	
	// Calculate the total weight of all available moves
	int totalWeight = 0;
	QList<Entry>::const_iterator it;
	for (it = entries.constBegin(); it != entries.constEnd(); ++it)
		totalWeight += it->weight;
	
	// Pick a move randomly, with the highest-weighted move having
	// the highest probability of getting being picked.
	int pick = qrand() % totalWeight;
	int currentWeight = 0;
	for (it = entries.constBegin(); it != entries.constEnd(); ++it)
	{
		currentWeight += it->weight;
		if (currentWeight > pick)
			return it->move;
	}
	
	return move;
}

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

#include <QDataStream>
#include "polyglotbook.h"


static BookMove moveFromBits(quint16 pgMove)
{
	BookMove move;
	
	move.target.file = (quint16)(pgMove << 13) >> 13;
	move.target.rank = (quint16)(pgMove << 10) >> 13;
	move.source.file = (quint16)(pgMove << 7) >> 13;
	move.source.rank = (quint16)(pgMove << 4) >> 13;
	move.promotion = (quint16)(pgMove << 1) >> 13;
	if (move.promotion > 0)
		move.promotion++;
	
	return move;
}

static quint16 moveToBits(const BookMove& move)
{
	quint16 target = move.target.file | (move.target.rank << 3);
	quint16 source = (move.source.file << 6) | (move.source.rank << 9);
	quint16 promotion = 0;
	if (move.promotion > 0)
		promotion = (move.promotion - 1) << 12;
	
	return target | source | promotion;
}


void PolyglotBook::loadEntry(QDataStream& in)
{
	quint64 key;
	quint16 pgMove;
	quint16 weight;
	quint32 learn;
	
	// Read the data. No need to worry about endianess,
	// because QDataStream uses big-endian by default.
	in >> key >> pgMove >> weight >> learn;
	
	Entry entry = { moveFromBits(pgMove), weight };
	m_map.insert(key, entry);
}

void PolyglotBook::saveEntry(const Map::const_iterator& it,
                             QDataStream& out) const
{
	quint32 learn = 0;
	quint64 key = it.key();
	quint16 pgMove = moveToBits(it.value().move);
	quint16 weight = it.value().weight;
	
	// Store the data. Again, big-endian is used by default.
	out << key << pgMove << weight << learn;
}

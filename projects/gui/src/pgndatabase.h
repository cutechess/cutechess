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

#ifndef PGN_DATABASE_H
#define PGN_DATABASE_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <pgngame.h>
#include <pgngameentry.h>

class PgnDatabase : public QObject
{
	Q_OBJECT

	public:
		PgnDatabase(const QString& fileName, QObject* parent = 0);

		void setEntries(const QList<PgnGameEntry>& entries);
		QList<PgnGameEntry> entries() const;
		QString fileName() const;
		QDateTime lastModified() const;
		void setLastModified(const QDateTime& lastModified);
		QString displayName() const;
		void setDisplayName(const QString& displayName);
		bool game(const PgnGameEntry& entry, PgnGame* game);

	private:
		QList<PgnGameEntry> m_entries;
		QString m_fileName;
		QDateTime m_lastModified;
		QString m_displayName;

};

#endif // PGN_DATABASE_H

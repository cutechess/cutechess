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

#include "econode.h"
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <QMutex>
#include "pgngame.h"
#include "pgnstream.h"

static QStringList s_openings;
static EcoNode* s_root = 0;

class EcoDeleter
{
	public:
		~EcoDeleter()
		{
			delete s_root;
		}
};
static EcoDeleter s_ecoDeleter;

QDataStream& operator<<(QDataStream& out, const EcoNode* node)
{
	if (!node)
		return out;
	out << node->m_ecoCode
	    << node->m_opening
	    << node->m_variation
	    << node->m_children;

	return out;
}

QDataStream& operator>>(QDataStream& in, EcoNode*& node)
{
	node = new EcoNode;

	in >> node->m_ecoCode
	   >> node->m_opening
	   >> node->m_variation
	   >> node->m_children;

	return in;
}

static int ecoFromString(const QString& ecoString)
{
	if (ecoString.length() < 2)
		return -1;
	int hundreds = ecoString.at(0).toUpper().toLatin1() - 'A';

	bool ok = false;
	int tens = ecoString.right(ecoString.length() - 1).toInt(&ok);
	if (!ok)
		return -1;

	return hundreds * 100 + tens;
}

void EcoNode::initialize()
{
	static QMutex mutex;
	if (s_root)
		return;

	mutex.lock();
	if (!s_root)
	{
		Q_INIT_RESOURCE(eco);

		QFile file(":/eco.bin");
		if (!file.open(QIODevice::ReadOnly))
			qWarning("Could not open ECO file");
		else
		{
			QDataStream in(&file);
			in.setVersion(QDataStream::Qt_4_6);
			in >> s_openings >> s_root;
		}
	}
	mutex.unlock();
}

void EcoNode::initialize(PgnStream& in)
{
	if (s_root)
		return;

	if (!in.isOpen())
	{
		qWarning("The pgn stream is not open");
		return;
	}

	s_root = new EcoNode;
	EcoNode* current = s_root;
	QMap<QString, int> tmpOpenings;

	PgnGame game;
	while (game.read(in))
	{
		current = s_root;
		foreach (const PgnGame::MoveData& move, game.moves())
		{
			QString san = move.moveString;
			EcoNode* node = current->child(san);
			if (node == 0)
			{
				node = new EcoNode;
				current->addChild(san, node);
			}
			current = node;
		}
		if (current == s_root)
			continue;

		current->m_ecoCode = ecoFromString(game.tagValue("ECO"));

		QString val = game.tagValue("Opening");
		if (!val.isEmpty())
		{
			int index = tmpOpenings.value(val, -1);
			if (index == -1)
			{
				index = tmpOpenings.count();
				tmpOpenings[val] = index;
				s_openings.append(val);
			}
			current->m_opening = index;
		}

		current->m_variation = game.tagValue("Variation");
	}
}

const EcoNode* EcoNode::root()
{
	if (!s_root)
		initialize();
	return s_root;
}

const EcoNode* EcoNode::find(const QVector<PgnGame::MoveData>& moves)
{
	if (!s_root)
		return 0;

	EcoNode* current = s_root;
	EcoNode* valid = 0;

	foreach (const PgnGame::MoveData& move, moves)
	{
		EcoNode* node = current->child(move.moveString);
		if (node == 0)
			return valid;
		if (!node->opening().isEmpty())
			valid = node;
		current = node;
	}

	return 0;
}

void EcoNode::write(const QString& fileName)
{
	if (!s_root)
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		qWarning("Could not open file %s", qPrintable(fileName));
		return;
	}

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_6);
	out << s_openings << s_root;
}

EcoNode::EcoNode()
	: m_ecoCode(-1),
	  m_opening(-1)
{
}

EcoNode::~EcoNode()
{
	qDeleteAll(m_children.values());
}

bool EcoNode::isLeaf() const
{
	return m_ecoCode != -1;
}

QString EcoNode::ecoCode() const
{
	if (m_ecoCode == -1)
		return QString();

	QChar segment('A' + m_ecoCode / 100);
	return segment + QString("%1").arg(m_ecoCode % 100, 2, 10, QChar('0'));
}

QString EcoNode::opening() const
{
	return m_opening >= 0 ? s_openings[m_opening] : QString();
}

QString EcoNode::variation() const
{
	return m_variation;
}

EcoNode* EcoNode::child(const QString& sanMove) const
{
	return m_children.value(sanMove);
}

void EcoNode::addChild(const QString& sanMove, EcoNode* child)
{
	m_children[sanMove] = child;
}

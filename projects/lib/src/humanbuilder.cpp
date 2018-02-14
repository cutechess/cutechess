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

#include "humanbuilder.h"
#include "humanplayer.h"

HumanBuilder::HumanBuilder(const QString& name, bool playAfterTimeout)
	: PlayerBuilder(name),
	  m_playAfterTimeout(playAfterTimeout)
{
}

bool HumanBuilder::isHuman() const
{
	return true;
}

ChessPlayer* HumanBuilder::create(QObject *receiver,
				  const char *method,
				  QObject *parent,
				  QString* error) const
{
	Q_UNUSED(error);

	ChessPlayer* player = new HumanPlayer(parent);
	if (!name().isEmpty())
	{
		player->setName(name());
		player->setCanPlayAfterTimeout(m_playAfterTimeout);
	}
	if (receiver != nullptr && method != nullptr)
		QObject::connect(player, SIGNAL(debugMessage(QString)),
				 receiver, method);

	return player;
}

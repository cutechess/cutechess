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

#include "gamepropertiesdlg.h"

GamePropertiesDialog::GamePropertiesDialog(QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);
}

void GamePropertiesDialog::setEvent(const QString& event)
{
	m_eventEdit->setText(event);
}

void GamePropertiesDialog::setSite(const QString& site)
{
	m_siteEdit->setText(site);
}

void GamePropertiesDialog::setRound(int round)
{
	m_roundSpin->setValue(round);
}

QString GamePropertiesDialog::event() const
{
	return m_eventEdit->text();
}

QString GamePropertiesDialog::site() const
{
	return m_siteEdit->text();
}

int GamePropertiesDialog::round() const
{
	return m_roundSpin->value();
}

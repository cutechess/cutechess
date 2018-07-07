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

#include "stringvalidator.h"
#include <QAbstractItemModel>

StringValidator::StringValidator(QObject* parent)
	: QValidator(parent),
	  m_model(nullptr)
{
}

void StringValidator::setModel(const QAbstractItemModel* model)
{
	m_model = model;
	m_startIndex = model->index(0, 0);
}

void StringValidator::setStartIndex(const QModelIndex& index)
{
	m_startIndex = index;
}

QValidator::State StringValidator::validate(QString& input, int& pos) const
{
	Q_UNUSED(pos);

	if (m_model == nullptr || m_model->rowCount() == 0)
		return Acceptable;

	if (input.isEmpty())
		return Intermediate;

	if (!m_model->match(m_startIndex, Qt::EditRole, input, 1,
		Qt::MatchFlags(Qt::MatchFixedString | Qt::MatchWrap)).isEmpty())
		return Acceptable;

	if (!m_model->match(m_startIndex, Qt::EditRole, input, 1,
		Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)).isEmpty())
		return Intermediate;

	return Invalid;
}

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

#ifndef ENGINECONFIGPROXYMODEL_H
#define ENGINECONFIGPROXYMODEL_H

#include <QSortFilterProxyModel>

/*!
 * \brief A proxy model for sorting and filtering engine configurations.
 *
 * In addition to QSortFilterProxyModel's functionality,
 * EngineConfigurationProxyModel can filter engines based on their
 * features, eg. the chess variants they support. This is useful when
 * setting up games or tournaments.
 */
class EngineConfigurationProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:
		/*! Creates a new EngineConfigurationProxyModel. */
		explicit EngineConfigurationProxyModel(QObject *parent = nullptr);

		/*!
		 * Sets the chess variant used to filter the contents
		 * of the source model to \a variant.
		 */
		void setFilterVariant(const QString& variant);

	protected:
		// Reimplemented from QSortFilterProxyModel
		virtual bool filterAcceptsRow(int sourceRow,
					      const QModelIndex& sourceParent) const;

	private:
		QString m_filterVariant;
};

#endif // ENGINECONFIGPROXYMODEL_H

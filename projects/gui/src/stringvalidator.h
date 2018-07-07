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

#ifndef STRINGVALIDATOR_H
#define STRINGVALIDATOR_H

#include <QValidator>
#include <QModelIndex>
class QAbstractItemModel;

/*!
 * \brief A model-based validator for strings
 *
 * The StringValidator class validates strings by searching for a
 * case-insensitive partial or exact match in a QAbstractItemModel.
 *
 * \internal Later this class could be extended to use a QStringList
 * as the lookup source.
 */
class StringValidator : public QValidator
{
	Q_OBJECT

	public:
		/*! Creates a new StringValidator object. */
		explicit StringValidator(QObject* parent = nullptr);

		/*! Uses \a model as the validation model. */
		void setModel(const QAbstractItemModel* model);
		/*!
		 * Sets the index where the lookup starts to \a index.
		 *
		 * By default \a model->index(0, 0) is used.
		 */
		void setStartIndex(const QModelIndex& index);

		// Inherited from QValidator
		virtual State validate(QString& input, int& pos) const;
		
	private:
		const QAbstractItemModel* m_model;
		QModelIndex m_startIndex;
};

#endif // STRINGVALIDATOR_H

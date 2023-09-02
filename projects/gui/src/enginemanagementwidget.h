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

#ifndef ENGINEMANAGEMENTWIDGET_H
#define ENGINEMANAGEMENTWIDGET_H

#include <QWidget>
#include <engineconfiguration.h>
#include <QTimer>

class EngineManager;
class QSortFilterProxyModel;
class QModelIndex;

namespace Ui {
	class EngineManagementWidget;
}

/*!
 * \brief The EngineManagementWidget class provides a dialog for chess engine
 * management.
*/
class EngineManagementWidget : public QWidget
{
	Q_OBJECT

	public:
		/*! Creates a new engine management widget. */
		EngineManagementWidget(QWidget* parent = nullptr);
		/*! Destroys the widget. */
		virtual ~EngineManagementWidget();

		/*!
		 * Returns true if configuration has changed;
		 * otherwise returns false.
		 */
		bool hasConfigChanged() const;

		/*! Saves the engine configuration to engines.json. */
		void saveConfig();
	
	private slots:
		void updateUi();
		void updateSearch(const QString& terms);
		void addEngine();
		void addEngines();
		void configureEngine();
		void configureEngine(const QModelIndex& index);
		void removeEngine();
		void cloneEngine();
		void browseDefaultLocation();
		void onDetectionTimer();

	private:
		void updateEngineCount();
		void detectEngine(const QString& file,
				  const QString& proto);

		EngineManager* m_engineManager;
		bool m_hasChanged;
		QSortFilterProxyModel* m_filteredModel;
		QTimer m_detectionTimer;
		struct DetectionData
		{
			QString name;
			QString protocol;
		};
		QVector<DetectionData> m_detectionList;
		Ui::EngineManagementWidget* ui;
};

#endif // ENGINEMANAGEMENTWIDGET_H

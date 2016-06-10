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

#include "evalwidget.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QVector>
#include <chessplayer.h>

EvalWidget::EvalWidget(QWidget *parent)
	: QWidget(parent),
	  m_player(nullptr),
	  m_table(new QTableWidget(0, 5, this)),
	  m_depth(-1)
{
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->verticalHeader()->hide();

	QStringList headers;
	headers << tr("Depth") << tr("Time") << tr("Nodes")
		<< tr("Score") << tr("PV");
	m_table->setHorizontalHeaderLabels(headers);
	m_table->setColumnWidth(0, 60);
	m_table->setColumnWidth(1, 60);
	m_table->setColumnWidth(2, 100);
	m_table->setColumnWidth(3, 60);
	m_table->horizontalHeader()->setStretchLastSection(true);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_table);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
}

void EvalWidget::clear()
{
	m_depth = -1;
	m_table->clearContents();
	m_table->setRowCount(0);
}

void EvalWidget::setPlayer(ChessPlayer* player)
{
	if (m_player)
	{
		m_player->disconnect(this);
		m_player->disconnect(m_table);
		clear();
	}
	m_player = player;

	connect(player, SIGNAL(startedThinking(int)),
		this, SLOT(clear()));
	connect(player, SIGNAL(thinking(MoveEvaluation)),
		this, SLOT(onEval(MoveEvaluation)));
}

void EvalWidget::onEval(const MoveEvaluation& eval)
{
	QVector<QTableWidgetItem*> items;

	items << new QTableWidgetItem(QString::number(eval.depth()))
	      << new QTableWidgetItem(QString::number(eval.time()))
	      << new QTableWidgetItem(QString::number(eval.nodeCount()))
	      << new QTableWidgetItem(QString::number(eval.score()))
	      << new QTableWidgetItem(eval.pv());

	if (eval.depth() != m_depth)
	{
		m_table->insertRow(0);
		m_depth = eval.depth();
	}
	for (int i = 0; i < items.size(); i++)
		m_table->setItem(0, i, items.at(i));
}

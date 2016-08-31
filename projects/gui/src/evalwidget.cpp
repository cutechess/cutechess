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
#include <QTime>
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
	m_pv.clear();
	m_table->clearContents();
	m_table->setRowCount(0);
}

void EvalWidget::setPlayer(ChessPlayer* player)
{
	if (player != m_player || !player)
		clear();
	if (m_player)
		m_player->disconnect(this);
	m_player = player;
	if (!player)
		return;

	connect(player, SIGNAL(startedThinking(int)),
		this, SLOT(clear()));
	connect(player, SIGNAL(thinking(MoveEvaluation)),
		this, SLOT(onEval(MoveEvaluation)));
}

void EvalWidget::onEval(const MoveEvaluation& eval)
{
	QString depth;
	if (eval.depth())
	{
		depth = QString::number(eval.depth());
		if (eval.selectiveDepth())
			depth += "/" + QString::number(eval.selectiveDepth());
	}

	QString time;
	int ms = eval.time();
	if (!ms)
		time = "";
	else if (ms < 1000)
		time = QString("%1 ms").arg(ms);
	else if (ms < 1000 * 60 * 60)
		time = QTime(0, 0).addMSecs(eval.time()).toString("mm:ss");
	else
		time = QTime(0, 0).addMSecs(eval.time()).toString("hh:mm:ss");

	QString nodeCount;
	if (eval.nodeCount())
		nodeCount = QString::number(eval.nodeCount());

	QString score;
	if (eval.score() != MoveEvaluation::NULL_SCORE)
		score = QString::number(double(eval.score()) / 100.0, 'f', 2);

	QVector<QTableWidgetItem*> items;
	items << new QTableWidgetItem(depth)
	      << new QTableWidgetItem(time)
	      << new QTableWidgetItem(nodeCount)
	      << new QTableWidgetItem(score)
	      << new QTableWidgetItem(eval.pv());

	for (int i = 0; i < 4; i++)
		items[i]->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);

	if (eval.depth() != m_depth || (eval.pv() != m_pv && !m_pv.isEmpty()))
		m_table->insertRow(0);
	m_depth = eval.depth();
	m_pv = eval.pv();

	for (int i = 0; i < items.size(); i++)
		m_table->setItem(0, i, items.at(i));
}

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
	  m_statsTable(new QTableWidget(1, 5, this)),
	  m_pvTable(new QTableWidget(0, 5, this)),
	  m_depth(-1)
{
	m_statsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	auto hHeader = m_statsTable->horizontalHeader();
	auto vHeader = m_statsTable->verticalHeader();
	vHeader->hide();
	int maxHeight = hHeader->sizeHint().height() + vHeader->defaultSectionSize();
	m_statsTable->setMaximumHeight(maxHeight);

	QStringList statsHeaders;
	statsHeaders << tr("NPS") << tr("Hash")
		     << tr("Pondermove") << tr("Ponderhit") << tr("TB");
	m_statsTable->setHorizontalHeaderLabels(statsHeaders);
	hHeader->setSectionResizeMode(QHeaderView::Stretch);
	auto protoItem = new QTableWidgetItem;
	protoItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_statsTable->setItemPrototype(protoItem);

	m_pvTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pvTable->verticalHeader()->hide();

	QStringList pvHeaders;
	pvHeaders << tr("Depth") << tr("Time") << tr("Nodes")
		  << tr("Score") << tr("PV");
	m_pvTable->setHorizontalHeaderLabels(pvHeaders);
	m_pvTable->setColumnWidth(0, 60);
	m_pvTable->setColumnWidth(1, 60);
	m_pvTable->setColumnWidth(2, 100);
	m_pvTable->setColumnWidth(3, 60);
	m_pvTable->horizontalHeader()->setStretchLastSection(true);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_statsTable);
	layout->addWidget(m_pvTable);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
}

void EvalWidget::clear()
{
	m_statsTable->clearContents();
	m_depth = -1;
	m_pv.clear();
	m_pvTable->clearContents();
	m_pvTable->setRowCount(0);
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
	auto nps = eval.nps();
	if (nps)
	{
		QString npsStr = nps < 10000 ? QString("%1").arg(nps)
					     : QString("%1k").arg(nps / 1000);
		auto item = m_statsTable->itemPrototype()->clone();
		item->setText(npsStr);
		m_statsTable->setItem(0, NpsHeader, item);
	}
	if (eval.tbHits())
	{
		auto item = m_statsTable->itemPrototype()->clone();
		item->setText(QString::number(eval.tbHits()));
		m_statsTable->setItem(0, TbHeader, item);
	}
	if (eval.hashUsage())
	{
		double usage = double(eval.hashUsage()) / 10.0;
		auto item = m_statsTable->itemPrototype()->clone();
		item->setText(QString("%1%").arg(usage, 0, 'f', 1));
		m_statsTable->setItem(0, HashHeader, item);
	}
	auto ponderMove = eval.ponderMove();
	if (!ponderMove.isEmpty())
	{
		auto item = m_statsTable->itemPrototype()->clone();
		item->setText(ponderMove);
		m_statsTable->setItem(0, PonderMoveHeader, item);
	}
	if (eval.ponderhitRate())
	{
		double rate = double(eval.ponderhitRate() / 10.0);
		auto item = m_statsTable->itemPrototype()->clone();
		item->setText(QString("%1%").arg(rate, 0, 'f', 1));
		m_statsTable->setItem(0, PonderHitHeader, item);
	}

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

	QString score = eval.scoreText();

	QVector<QTableWidgetItem*> items;
	items << new QTableWidgetItem(depth)
	      << new QTableWidgetItem(time)
	      << new QTableWidgetItem(nodeCount)
	      << new QTableWidgetItem(score)
	      << new QTableWidgetItem(eval.pv());

	for (int i = 0; i < 4; i++)
		items[i]->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);

	if (eval.depth() != m_depth || (eval.pv() != m_pv && !m_pv.isEmpty()))
		m_pvTable->insertRow(0);
	m_depth = eval.depth();
	m_pv = eval.pv();

	for (int i = 0; i < items.size(); i++)
		m_pvTable->setItem(0, i, items.at(i));
}

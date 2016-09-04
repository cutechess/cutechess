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

#include "evalhistory.h"
#include <QVBoxLayout>
#include <QtGlobal>
#include <qcustomplot.h>
#include <chessgame.h>
#include <moveevaluation.h>

EvalHistory::EvalHistory(QWidget *parent)
	: QWidget(parent),
	  m_plot(new QCustomPlot(this)),
	  m_game(nullptr)
{
	auto x = m_plot->xAxis;
	auto y = m_plot->yAxis;

	x->setLabel(tr("move"));
	x->setAutoTickStep(false);
	x->setTickStep(1);
	x->setAutoSubTicks(false);
	x->setSubTickCount(0);
	x->setRangeLower(1);

	y->setLabel(tr("score"));
	y->setAutoSubTicks(false);
	y->setSubTickCount(0);
	y->setRange(-1, 1);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_plot);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);

	setMinimumHeight(120);
}

void EvalHistory::setGame(ChessGame* game)
{
	if (m_game)
		m_game->disconnect(this);
	m_game = game;
	m_plot->clearGraphs();
	if (!game)
	{
		replot(0);
		return;
	}

	connect(m_game, SIGNAL(scoreChanged(int,int)),
		this, SLOT(onScore(int,int)));

	m_plot->addGraph();
	m_plot->addGraph();

	auto cWhite = QColor("#ffce9e");
	auto cBlack = QColor("#d18b47");
	auto pWhite = QPen(cWhite.darker(150));
	pWhite.setWidth(2);
	auto pBlack = QPen(cBlack.darker());
	pBlack.setWidth(2);

	m_plot->graph(0)->setPen(pWhite);
	cWhite.setAlpha(200);
	m_plot->graph(0)->setBrush(QBrush(cWhite));
	m_plot->graph(1)->setPen(pBlack);
	cBlack.setAlpha(128);
	m_plot->graph(1)->setBrush(QBrush(cBlack));

	const auto& scores = game->scores();
	int ply = -1;

	for (auto it = scores.constBegin(); it != scores.constEnd(); ++it)
	{
		ply = it.key();
		addData(ply, it.value());
	}
	replot(ply);
}

void EvalHistory::addData(int ply, int score)
{
	if (score == MoveEvaluation::NULL_SCORE)
		return;

	int side = (ply % 2 == 0) ? 0 : 1;
	double x = double(ply + 2) / 2;
	double y = qBound(-15.0, double(score) / 100.0, 15.0);
	if (side == 1)
		y = -y;

	m_plot->graph(side)->addData(x, y);
}

void EvalHistory::replot(int maxPly)
{
	if (maxPly != -1)
	{
		int step = qMax(1, maxPly / 20);
		m_plot->xAxis->setTickStep(double(step));
		m_plot->rescaleAxes();
	}
	m_plot->replot();
}

void EvalHistory::onScore(int ply, int score)
{
	addData(ply, score);
	replot(ply);
}

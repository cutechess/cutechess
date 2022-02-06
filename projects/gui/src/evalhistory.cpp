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

#include "evalhistory.h"
#include "board/board.h"
#include <QVBoxLayout>
#include <QtGlobal>
#include <chessgame.h>
#include <moveevaluation.h>

using namespace QtCharts;

constexpr double cInitMin = +1.0e7;

EvalHistory::EvalHistory(QWidget *parent)
	: QWidget(parent),
	  m_chart(new QChart()),
	  m_min(cInitMin),
	  m_max(-cInitMin),
	  m_game(nullptr),
	  m_invertSides(false)
{
	QChartView *chartView = new QChartView(m_chart);
	chartView->setRenderHint(QPainter::Antialiasing);
	QLineSeries *series[2] {new QLineSeries(), new QLineSeries()};
	QLineSeries *base[2] {new QLineSeries(), new QLineSeries()};
	QAreaSeries *areaSeries[2] {new QAreaSeries(series[0], base[0]),
				    new QAreaSeries(series[1], base[1])};
	m_chart->legend()->hide();
	m_chart->setMargins(QMargins(0, 0, 0, 0));
	m_chart->addSeries(areaSeries[0]);
	m_chart->addSeries(areaSeries[1]);
	m_chart->createDefaultAxes();
	auto x = static_cast<QValueAxis *>(m_chart->axisX(areaSeries[0]));
	auto y = static_cast<QValueAxis *>(m_chart->axisY(areaSeries[0]));

	x->setTitleText(tr("move"));
	x->setLabelFormat("%i");
	x->setRange(1, 2);
	x->setTickCount(2);
	x->setLabelsColor(QApplication::palette().text().color());
	x->setGridLineColor(QApplication::palette().midlight().color());

	y->setTitleText(tr("score"));
	y->setLabelFormat("%.2f");
	y->setRange(-1, 1);
	y->setTickCount(5);
	y->setLabelsColor(QApplication::palette().text().color());
	y->setGridLineColor(QApplication::palette().midlight().color());

	m_chart->setBackgroundBrush(QApplication::palette().window());

	auto cWhite = QColor(0xff, 0xce, 0x9e);
	auto cBlack = QColor(0xd1, 0x8b, 0x47);
	auto pWhite = QPen(cWhite.darker(150));
	pWhite.setWidth(2);
	auto pBlack = QPen(cBlack.darker());
	pBlack.setWidth(2);

	areaSeries[0]->setPen(pWhite);
	cWhite.setAlpha(200);
	areaSeries[0]->setBrush(QBrush(cWhite));
	areaSeries[1]->setPen(pBlack);
	cBlack.setAlpha(128);
	areaSeries[1]->setBrush(QBrush(cBlack));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(chartView);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);

	setMinimumHeight(130);
}

void EvalHistory::setGame(ChessGame* game)
{
	if (m_game)
		m_game->disconnect(this);
	m_game = game;
	if (!game)
	{
		replot(0);
		return;
	}

	connect(m_game, SIGNAL(scoreChanged(int,int)),
		this, SLOT(onScore(int,int)));

	m_invertSides = (m_game->board()->startingSide() == Chess::Side::Black);
	setScores(game->scores());
}

void EvalHistory::setPgnGame(PgnGame* pgn)
{
	if (pgn == nullptr || pgn->isNull())
		return;

	m_invertSides = (pgn->startingSide() == Chess::Side::Black);
	setScores(pgn->extractScores());
}

void EvalHistory::setScores(const QMap< int, int >& scores)
{
	static_cast<QAreaSeries*>(m_chart->series()[0])->upperSeries()->clear();
	static_cast<QAreaSeries*>(m_chart->series()[0])->lowerSeries()->clear();
	static_cast<QAreaSeries*>(m_chart->series()[1])->upperSeries()->clear();
	static_cast<QAreaSeries*>(m_chart->series()[1])->lowerSeries()->clear();
	m_min = cInitMin;
	m_max = -cInitMin;

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

	int base = m_invertSides ? 1 : 0;
	int side = (ply % 2 == base) ? 0 : 1;
	double x = double(ply + 2) / 2;
	double y = qBound(-15.0, double(score) / 100.0, 15.0);
	if (side == 1)
		y = -y;

	static_cast<QAreaSeries *>(m_chart->series()[side])->upperSeries()->append(x, y);
	static_cast<QAreaSeries *>(m_chart->series()[side])->lowerSeries()->append(x, 0.);
	m_min = qMin(m_min, y);
	m_max = qMax(m_max, y);
}

void EvalHistory::replot(int maxPly)
{
	if (maxPly == -1)
	{
		m_chart->axisX()->setRange(1, 2);
		m_chart->axisY()->setRange(-1, 1);
	}
	else
	{
		m_chart->axisX()->setRange(1, 1 + maxPly / 2);
		int step = 1 + maxPly / 20;
		static_cast<QValueAxis *>(m_chart->axisX())->setTickCount(1 + maxPly / 2 / step);

		double scaler = 10.0;
		if (m_max - m_min > 5.0)
			scaler = 1.0;
		else if (m_max - m_min > 2.5)
			scaler = 2.0;
		else if (m_max - m_min > 1.0)
			scaler = 4.0;

		// Ordinate: Use integer units.
		int max = m_max * scaler + .99999;
		int min = m_min * scaler - .99999;

		// Zero must be in range.
		if (min > 0)
			min = 0;
		if (max < 0)
			max = 0;
		if (min == max)
			min--, max++;

		// Do not use too many ticks.
		if (height() >= 180)
			step = 1 + (max - min) / 6;
		else
			step = qMax(1, (max - min) / 2);

		// Align max and min with step size, calculate tick count.
		max = (max + step - 1) / step * step;
		min = (min - step + 1) / step * step;
		int ticks = 1 + (max - min) / step;
		static_cast<QValueAxis *>(m_chart->axisY())->setTickCount(ticks);
		m_chart->axisY()->setRange(min / scaler, max / scaler);
	}
}

void EvalHistory::onScore(int ply, int score)
{
	addData(ply, score);
	replot(ply);
}

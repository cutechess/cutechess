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

#include "tournamentresultsdlg.h"

#include <limits>

#include <QPlainTextEdit>
#include <QBoxLayout>
#include <QFont>
#include <QMenu>
#include "resultformatdlg.h"

#include <tournament.h>

TournamentResultsDialog::TournamentResultsDialog(QWidget* parent)
	: QDialog(parent),
	  m_tournament(0)
{
	setWindowTitle(tr("Tournament Results"));

	m_resultsEdit = new QPlainTextEdit(this);
	m_resultsEdit->setReadOnly(true);
	m_resultsEdit->setContextMenuPolicy(Qt::CustomContextMenu);

	QFont font("Courier New");
	font.setStyleHint(QFont::Monospace);
	font.setPointSize(font.pointSize() - 1);
	m_resultsEdit->document()->setDefaultFont(font);

	auto layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->addWidget(m_resultsEdit);
	layout->setContentsMargins(0, 0, 0, 0);

	setLayout(layout);
	resize(1100, 400);

	connect(m_resultsEdit, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(onContextMenuRequest()));

}

TournamentResultsDialog::~TournamentResultsDialog()
{
}

void TournamentResultsDialog::setTournament(Tournament* tournament)
{
	setWindowTitle(tournament->name());
	m_tournament = tournament;
	m_resultsEdit->setPlainText(tournament->results());
}

void TournamentResultsDialog::update()
{
	auto tournament = qobject_cast<Tournament*>(QObject::sender());
	Q_ASSERT(tournament != nullptr);

	m_tournament = tournament;
	setText();
}

void TournamentResultsDialog::setText()
{
	if (m_tournament.isNull())
		return;

	QString text;

	// A quick fix, copied from the CLI side.
	if (m_tournament->playerCount() == 2 && m_tournament->type() != "knockout")
	{
		TournamentPlayer fcp = m_tournament->playerAt(0);
		TournamentPlayer scp = m_tournament->playerAt(1);
		int totalResults = fcp.gamesFinished();
		double scoreRatio = std::numeric_limits<double>::quiet_NaN();
		if (totalResults > 0)
			scoreRatio = double(fcp.score()) / (totalResults * 2);
		text = tr("Score of %1 vs %2: %3 - %4 - %5 [%6]\n")
		       .arg(fcp.name())
		       .arg(scp.name())
		       .arg(fcp.wins())
		       .arg(scp.wins())
		       .arg(fcp.draws())
		       .arg(scoreRatio, 0, 'f', 3);
	}

	text += m_tournament->results();
	text += tr("\n%1 of %2 games finished.")
		.arg(m_tournament->finishedGameCount())
		.arg(m_tournament->finalGameCount());
	if (m_tournament->finishedGameCount() >= m_tournament->finalGameCount()
	|| m_tournament->isStopping())
		text.append("\n").append(m_tournament->outcomes());
	m_resultsEdit->setPlainText(text);
}

void TournamentResultsDialog::onContextMenuRequest()
{
	QMenu* menu = m_resultsEdit->createStandardContextMenu();
	if (!m_tournament.isNull())
	{
		auto editResultFormatlAct = menu->addAction(tr("Edit Result Format"));
		connect(editResultFormatlAct, &QAction::triggered, this, [=]()
		{
			ResultFormatDlg dlg;
			dlg.setFormats(m_tournament->namedResultFormats(),
				       m_tournament->resultFormat());
			if (dlg.exec() == QDialog::Accepted
			&& !m_tournament.isNull())
			{
				QString resultFormat(dlg.resultFormat());
				m_tournament->setResultFormat(resultFormat);
				setText();
			}
		});
	}
	menu->exec(QCursor::pos());
	delete menu;
}

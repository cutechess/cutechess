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

#include "tournamentresultsdlg.h"

#include <QPlainTextEdit>
#include <QBoxLayout>
#include <QFont>

TournamentResultsDialog::TournamentResultsDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Tournament Results"));

	m_resultsEdit = new QPlainTextEdit(this);
	m_resultsEdit->setReadOnly(true);

	QFont font("Courier New");
	font.setStyleHint(QFont::Monospace);
	m_resultsEdit->document()->setDefaultFont(font);

	auto layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->addWidget(m_resultsEdit);
	layout->setContentsMargins(0, 0, 0, 0);

	setLayout(layout);
	resize(400, 400);
}

TournamentResultsDialog::~TournamentResultsDialog()
{
}

void TournamentResultsDialog::setResults(const QString& results)
{
	m_resultsEdit->setPlainText(results);
}

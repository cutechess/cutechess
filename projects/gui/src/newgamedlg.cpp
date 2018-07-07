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

#include "newgamedlg.h"
#include "ui_newgamedlg.h"

#include <QAbstractItemView>
#include <QSettings>

#include <board/boardfactory.h>
#include <chessgame.h>
#include <enginebuilder.h>
#include <humanbuilder.h>
#include <enginemanager.h>
#include <openingsuite.h>

#include "cutechessapp.h"
#include "engineconfigurationmodel.h"
#include "engineconfigproxymodel.h"
#include "engineconfigurationdlg.h"
#include "timecontroldlg.h"
#include "stringvalidator.h"

#ifdef QT_DEBUG
#include <modeltest.h>
#endif

NewGameDialog::NewGameDialog(EngineManager* engineManager, QWidget* parent)
	: QDialog(parent),
	  m_engineManager(engineManager),
	  ui(new Ui::NewGameDialog)
{
	Q_ASSERT(engineManager != nullptr);
	ui->setupUi(this);

	m_engines = new EngineConfigurationModel(m_engineManager, this);
	#ifdef QT_DEBUG
	new ModelTest(m_engines, this);
	#endif

	connect(ui->m_configureWhiteEngineButton, SIGNAL(clicked()),
		this, SLOT(configureEngine()));
	connect(ui->m_configureBlackEngineButton, SIGNAL(clicked()),
		this, SLOT(configureEngine()));

	m_proxyModel = new EngineConfigurationProxyModel(this);
	m_proxyModel->setSourceModel(m_engines);
	m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_proxyModel->sort(0);
	m_proxyModel->setDynamicSortFilter(true);

	StringValidator* engineValidator = new StringValidator(this);
	engineValidator->setModel(m_proxyModel);

	connect(ui->m_whiteEngineComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onEngineChanged(int)));
	connect(ui->m_blackEngineComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onEngineChanged(int)));

	ui->m_whiteEngineComboBox->setModel(m_proxyModel);
	ui->m_whiteEngineComboBox->setValidator(engineValidator);
	ui->m_blackEngineComboBox->setModel(m_proxyModel);
	ui->m_blackEngineComboBox->setValidator(engineValidator);

	connect(ui->m_gameSettings, SIGNAL(variantChanged(QString)),
		this, SLOT(onVariantChanged(QString)));
	onVariantChanged(ui->m_gameSettings->chessVariant());

	connect(ui->m_whitePlayerCpuRadio, &QRadioButton::toggled, [=](bool checked)
	{
		auto type = checked ? CPU : Human;
		setPlayerType(Chess::Side::White, type);
	});
	connect(ui->m_blackPlayerCpuRadio, &QRadioButton::toggled, [=](bool checked)
	{
		auto type = checked ? CPU : Human;
		setPlayerType(Chess::Side::Black, type);
	});

	connect(ui->m_gameSettings, &GameSettingsWidget::statusChanged, [=](bool ok)
	{
		ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	});
}

NewGameDialog::~NewGameDialog()
{
	delete ui;
}

ChessGame* NewGameDialog::createGame() const
{
	bool ok = true;
	const QString variant = ui->m_gameSettings->chessVariant();
	auto board = Chess::BoardFactory::create(variant);
	auto pgn = new PgnGame();
	pgn->setSite(QSettings().value("pgn/site").toString());
	auto game = new ChessGame(board, pgn);

	game->setTimeControl(ui->m_gameSettings->timeControl());
	game->setAdjudicator(ui->m_gameSettings->adjudicator());

	auto suite = ui->m_gameSettings->openingSuite();
	if (suite)
	{
		int depth = ui->m_gameSettings->openingSuiteDepth();
		ok = game->setMoves(suite->nextGame(depth));
		delete suite;
	}

	auto book = ui->m_gameSettings->openingBook();
	if (book)
	{
		int depth = ui->m_gameSettings->bookDepth();
		game->setBookOwnership(true);

		for (int i = 0; i < 2; i++)
		{
			auto side = Chess::Side::Type(i);
			if (playerType(side) == CPU)
				game->setOpeningBook(book, side, depth);
		}
	}

	if (!ok)
	{
		delete game;
		return nullptr;
	}

	return game;
}

PlayerBuilder* NewGameDialog::createPlayerBuilder(Chess::Side side) const
{
	if (playerType(side) == CPU)
	{
		auto config = m_engineConfig[side];
		ui->m_gameSettings->applyEngineConfiguration(&config);

		return new EngineBuilder(config);
	}
	bool ignoreFlag = QSettings().value("games/human_can_play_after_timeout",
					   true).toBool();
	return new HumanBuilder(CuteChessApplication::userName(), ignoreFlag);
}

void NewGameDialog::setPlayerType(Chess::Side side, PlayerType type)
{
	if (side == Chess::Side::White)
	{
		ui->m_whiteEngineComboBox->setEnabled(type == CPU);
		ui->m_configureWhiteEngineButton->setEnabled(type == CPU);
	}
	else
	{
		ui->m_blackEngineComboBox->setEnabled(type == CPU);
		ui->m_configureBlackEngineButton->setEnabled(type == CPU);
	}

	int humanCount = 0;
	if (playerType(Chess::Side::White) == Human)
		humanCount++;
	if (playerType(Chess::Side::Black) == Human)
		humanCount++;
	ui->m_gameSettings->onHumanCountChanged(humanCount);
}

NewGameDialog::PlayerType NewGameDialog::playerType(Chess::Side side) const
{
	Q_ASSERT(!side.isNull());

	if (side == Chess::Side::White)
		return (ui->m_whitePlayerHumanRadio->isChecked()) ? Human : CPU;
	else
		return (ui->m_blackPlayerHumanRadio->isChecked()) ? Human : CPU;
}

void NewGameDialog::configureEngine()
{
	Chess::Side side;
	if (QObject::sender() == ui->m_configureWhiteEngineButton)
		side = Chess::Side::White;
	else
		side = Chess::Side::Black;

	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);
	dlg.applyEngineInformation(m_engineConfig[side]);

	if (dlg.exec() == QDialog::Accepted)
		m_engineConfig[side] = dlg.engineConfiguration();
}

void NewGameDialog::onVariantChanged(const QString& variant)
{
	m_proxyModel->setFilterVariant(variant);

	bool empty = m_proxyModel->rowCount() == 0;
	if (empty)
	{
		ui->m_whitePlayerHumanRadio->setChecked(true);
		ui->m_blackPlayerHumanRadio->setChecked(true);
	}

	ui->m_whitePlayerCpuRadio->setDisabled(empty);
	ui->m_blackPlayerCpuRadio->setDisabled(empty);
}

void NewGameDialog::onEngineChanged(int index, Chess::Side side)
{
	if (side == Chess::Side::NoSide)
	{
		if (QObject::sender() == ui->m_whiteEngineComboBox)
			side = Chess::Side::White;
		else
			side = Chess::Side::Black;
	}

	auto modelIndex = m_proxyModel->index(index, 0);
	if (modelIndex.isValid())
	{
		int i = m_proxyModel->mapToSource(modelIndex).row();
		m_engineConfig[side] = m_engineManager->engineAt(i);
	}
}

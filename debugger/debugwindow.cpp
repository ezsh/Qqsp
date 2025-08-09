#include "debugwindow.h"

#include "../qspstr.h"

#include "expressionsmodel.h"

#include <qsp_default.h>

#include <QClipboard>
#include <QGuiApplication>
#include <QItemSelectionModel>

#include "ui_debugwindow.h"

Debugger::DebugWindow::DebugWindow(QAction* toggleVisibilityAction, QWidget* parent)
	: base(parent)
	, ui_{new Ui::DebugWindow()}
	, toggleVisibilityAction_{toggleVisibilityAction}
	, expressionsModel_{new ExpressionsModel{this}}
	, expressionSelectionModel_{new QItemSelectionModel{expressionsModel_, this}}
{
	ui_->setupUi(this);
	ui_->tableViewExpressions->horizontalHeader()->setStretchLastSection(true);
	ui_->tableViewExpressions->setModel(expressionsModel_);
	ui_->tableViewExpressions->setSelectionModel(expressionSelectionModel_);

	ui_->tableViewExpressions->setContextMenu(ui_->menuExpressions);

	setWindowTitle(tr("QSP debug tools"));

	connect(toggleVisibilityAction, &QAction::triggered, this, &DebugWindow::setVisible);

	connect(ui_->actionLogClear, &QAction::triggered, ui_->logText, &QPlainTextEdit::clear);
	connect(ui_->actionLogCopyAll, &QAction::triggered, this, &DebugWindow::copyLogToClipboard);

	connect(ui_->pushButtonRunCode, &QAbstractButton::clicked, this, &DebugWindow::runCode);
	connect(ui_->lineEditCodeToRun, &QLineEdit::editingFinished, this, &DebugWindow::runCode);

	connect(ui_->actionRefreshExpressions, &QAction::triggered, this, &DebugWindow::refreshAllExpressions);
	connect(expressionSelectionModel_, &QItemSelectionModel::currentChanged, this, &DebugWindow::expressionsSelectionChanged);
	connect(expressionsModel_, &QAbstractItemModel::rowsInserted, this, &DebugWindow::expressionsRowsInserted);
	connect(expressionsModel_, &QAbstractItemModel::rowsRemoved, this, &DebugWindow::expressionsRowsRemoved);
	connect(expressionsModel_, &QAbstractItemModel::modelReset, this, &DebugWindow::expressionsModelReset);

	connect(ui_->actionExpressionAdd, &QAction::triggered, this, &DebugWindow::addNewExpression);
	connect(ui_->actionExpressionRemove, &QAction::triggered, this, &DebugWindow::removeSelectedExpression);
	connect(ui_->actionExpressionRemoveAll, &QAction::triggered, this, &DebugWindow::removeAllExpressions);
	connect(ui_->actionExpressionCopy, &QAction::triggered, this, &DebugWindow::copySelectedExpressionValue);
	connect(ui_->actionExpressionCopyAll, &QAction::triggered, this, &DebugWindow::copyAllExpression);
}

Debugger::DebugWindow::~DebugWindow()
{
	delete ui_;
}

void Debugger::DebugWindow::appendLogLine(QString line)
{
	ui_->logText->appendPlainText(line);
}

void Debugger::DebugWindow::scheduleUpdate()
{
	if (!ui_->actionActionAutoRefreshExpressions->isChecked()) {
		return;
	}

	if (isVisible()) {
		refreshAllExpressions();
	} else {
		updateScheduled_ = true;
	}
}

void Debugger::DebugWindow::setVisible(bool visible)
{
	base::setVisible(visible);
	toggleVisibilityAction_->setChecked(visible);
	QSPEnableDebugMode(visible);
	if (visible && updateScheduled_) {
		refreshAllExpressions();
	}
}

void Debugger::DebugWindow::copyLogToClipboard()
{
	QGuiApplication::clipboard()->setText(ui_->logText->toPlainText());
}

void Debugger::DebugWindow::addNewExpression()
{
	ui_->tableViewExpressions->edit(expressionsModel_->addExpression({}));
}

void Debugger::DebugWindow::removeSelectedExpression()
{
	if (ui_->tableViewExpressions->contextMenuIndex().isValid()) {
		expressionsModel_->removeRow(ui_->tableViewExpressions->contextMenuIndex().row());
	}
}

void Debugger::DebugWindow::removeAllExpressions()
{
	expressionsModel_->clear();
}

void Debugger::DebugWindow::copySelectedExpressionValue()
{
	if (ui_->tableViewExpressions->contextMenuIndex().isValid()) {
		QGuiApplication::clipboard()->setText(expressionsModel_->value(ui_->tableViewExpressions->contextMenuIndex().row()));
	}
}

void Debugger::DebugWindow::copyAllExpression()
{
	QString text;
	int rows = expressionsModel_->rowCount();
	for (int i = 0; i < rows; ++i) {
		text.append(expressionsModel_->expression(i));
		text.append(QChar('\t'));
		text.append(expressionsModel_->value(i));
		text.append(QChar('\n'));
	}
	QGuiApplication::clipboard()->setText(text);
}

void Debugger::DebugWindow::refreshAllExpressions()
{
	expressionsModel_->evaluateAll();
	updateScheduled_ = false;
}

void Debugger::DebugWindow::runCode()
{
	QString code{ui_->lineEditCodeToRun->text()};
	if (code.isEmpty()) {
		return;
	}

	QSPExecString(QSPStr(code), QSP_TRUE);
}

void Debugger::DebugWindow::expressionsSelectionChanged(const QModelIndex& current, [[maybe_unused]] const QModelIndex& previous)
{
	if (current.isValid()) {
		ui_->actionExpressionRemove->setEnabled(true);
		ui_->actionExpressionCopy->setEnabled(true);
	} else {
		ui_->actionExpressionRemove->setEnabled(false);
		ui_->actionExpressionCopy->setEnabled(false);
	}
}

void Debugger::DebugWindow::expressionsRowsInserted([[maybe_unused]] const QModelIndex& parent, [[maybe_unused]] int first, [[maybe_unused]] int last)
{
	ui_->actionExpressionCopyAll->setEnabled(true);
	ui_->actionExpressionRemoveAll->setEnabled(true);
}

void Debugger::DebugWindow::expressionsRowsRemoved(const QModelIndex& parent, [[maybe_unused]] int first, [[maybe_unused]] int last)
{
	bool modelIsEmpty = expressionsModel_->rowCount(parent) <= 0;
	if (modelIsEmpty) {
		expressionsModelReset();
	}
}

void Debugger::DebugWindow::expressionsModelReset()
{
	ui_->actionExpressionCopyAll->setEnabled(false);
	ui_->actionExpressionRemoveAll->setEnabled(false);
}

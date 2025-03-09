#include "debuglogwindow.h"

#include <QClipboard>
#include <QGuiApplication>
#include <qsp_default.h>

#include "ui_debuglogwindow.h"

DebugLogWindow::DebugLogWindow(QAction* toggleVisibilityAction, QWidget* parent)
    : base(parent)
    , _ui{new Ui::DebugLogWindow()}
    , _toggleVisibilityAction{toggleVisibilityAction}
{
    _ui->setupUi(this);
    _ui->retranslateUi(this);
    setWindowTitle(tr("QSP debug log"));

    connect(toggleVisibilityAction, &QAction::triggered, this, &DebugLogWindow::setVisible);

    connect(_ui->actionClear, &QAction::triggered, _ui->logText, &QPlainTextEdit::clear);
    connect(_ui->actionCopy_all, &QAction::triggered, this, &DebugLogWindow::copyAllToClipboard);
}

DebugLogWindow::~DebugLogWindow()
{
    delete _ui;
}

void DebugLogWindow::appendLine(QString line)
{
    _ui->logText->appendPlainText(line);
}

void DebugLogWindow::setVisible(bool visible)
{
    base::setVisible(visible);
    _toggleVisibilityAction->setChecked(visible);
    QSPEnableDebugMode(visible);
}

void DebugLogWindow::copyAllToClipboard()
{
    QGuiApplication::clipboard()->setText(_ui->logText->toPlainText());
}

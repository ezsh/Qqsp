#ifndef QQSP_DEBUGLOGWINDOW_H
#define QQSP_DEBUGLOGWINDOW_H

#include <QMainWindow>

namespace Ui
{
class DebugLogWindow;
}

class DebugLogWindow : public QMainWindow
{
    using base = QMainWindow;

public:
    DebugLogWindow(QAction* toggleVisibilityAction, QWidget* parent = nullptr);
    ~DebugLogWindow();

    void appendLine(QString line);

    // QWidget interface
    void setVisible(bool visible) override;

private:
    void copyAllToClipboard();

    Ui::DebugLogWindow* _ui;
    QAction* _toggleVisibilityAction;
};

#endif

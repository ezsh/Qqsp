#ifndef QQSP_DEBUGGER_DEBUGWINDOW_H
#define QQSP_DEBUGGER_DEBUGWINDOW_H

#include <QMainWindow>

class QItemSelectionModel;

namespace Ui {
	class DebugWindow;
}

namespace Debugger {
	class ExpressionsModel;

	class DebugWindow: public QMainWindow {
		using base = QMainWindow;

	public:
		DebugWindow(QAction* toggleVisibilityAction, QWidget* parent = nullptr);
		~DebugWindow();

		void appendLogLine(QString line);
		void scheduleUpdate();

		// QWidget interface
		void setVisible(bool visible) override;

	private:
		void copyLogToClipboard();
		void refreshAllExpressions();
		void runCode();
		void expressionsSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
		void expressionsRowsInserted(const QModelIndex& parent, int first, int last);
		void expressionsRowsRemoved(const QModelIndex& parent, int first, int last);
		void expressionsModelReset();
		void addNewExpression();
		void removeSelectedExpression();
		void removeAllExpressions();
		void copySelectedExpressionValue();
		void copyAllExpression();

		Ui::DebugWindow* ui_;
		QAction* toggleVisibilityAction_;
		ExpressionsModel* expressionsModel_;
		QItemSelectionModel* expressionSelectionModel_;
		bool updateScheduled_{};
	};
} // namespace Debugger
#endif

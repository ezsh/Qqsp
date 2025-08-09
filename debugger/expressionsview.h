#ifndef QQSP_DEBUGGER_EXPRESSIONVIEW_H
#define QQSP_DEBUGGER_EXPRESSIONVIEW_H

#include <QTableView>

namespace Debugger {
	class ExpressionsView: public QTableView {
		using Base = QTableView;

	public:
		using Base::Base;

		void setContextMenu(QMenu* menu);

		const QModelIndex contextMenuIndex() const { return contextMenuIndex_; }

	protected:
		// QWidget interface
		void contextMenuEvent(QContextMenuEvent* event) override;

	private:
		QMenu* contextMenu_{};
		QModelIndex contextMenuIndex_;
	};
} // namespace Debugger
#endif

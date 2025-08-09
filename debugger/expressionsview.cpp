#include "expressionsview.h"

#include <QClipboard>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>

void Debugger::ExpressionsView::setContextMenu(QMenu* menu)
{
	contextMenu_ = menu;
}

void Debugger::ExpressionsView::contextMenuEvent(QContextMenuEvent* event)
{
	if (!contextMenu_) {
		return;
	}

	contextMenuIndex_ = indexAt(event->pos());
	contextMenu_->popup(event->globalPos());

	/*
	QModelIndex index = ;
	QMenu* menu = new QMenu(this);
	QAction* actionAdd = menu->addAction(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd), tr("Add"), QKeyCombination{Qt::Key::Key_Insert});
	QAction* actionRemove = menu->addAction(QIcon::fromTheme(QIcon::ThemeIcon::ListRemove), tr("Remove"), QKeyCombination{Qt::Key::Key_Delete});
	QAction* actionRemoveAll = menu->addAction(tr("Remove all"));
	menu->addSeparator();
	QAction* actionCopyValue =
	    menu->addAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), tr("Copy value"), QKeyCombination{Qt::Modifier::CTRL, Qt::Key::Key_C});
	QAction* actionCopyAll = menu->addAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), tr("Copy All"), QKeyCombination{Qt::Modifier::CTRL, Qt::Key::Key_C});

	connect(actionAdd, &QAction::triggered, this, [this, index]() { model()->insertRow(index.isValid() ? index.row() : model()->rowCount()); });
	if (index.isValid()) {
	    connect(actionRemove, &QAction::triggered, this, [this, index]() { model()->removeRow(index.row()); });
	    connect(actionCopyValue, &QAction::triggered, this, [this, index]() {
	        QGuiApplication::clipboard()->setText(model()->data(model()->index(index.row(), 1)).toString());
	    });
	} else {
	    actionRemove->setEnabled(false);
	    actionCopyValue->setEnabled(false);
	}

	if (model()->rowCount()) {
	    connect(actionRemoveAll, &QAction::triggered, this, [this]() { model()->removeRows(0, model()->rowCount()); });
	    connect(actionCopyAll, &QAction::triggered, this, &ExpressionsView::copyAll);
	} else {
	    actionRemoveAll->setEnabled(false);
	    actionCopyAll->setEnabled(false);
	}

	menu->popup(event->globalPos());
	*/
}

#include "webinspectorwindow.h"

#include <QApplication>
#include <QScreen>
#include <QSettings>

#include "ui_webinspectorwindow.h"

WebInspectorWindow::WebInspectorWindow(QWidget* parent)
	: _ui{new Ui::WebInspectorWindow()}
{
	setAttribute(Qt::WA_DeleteOnClose);
	_ui->setupUi(this);
	loadSettings();
}

WebInspectorWindow::~WebInspectorWindow()
{
	saveSettings();
	delete _ui;
}

void WebInspectorWindow::setViews(QspWebBox* mainView, QspWebBox* descView)
{
	_ui->tabMain->setView(mainView);
	_ui->tabDesc->setView(descView);
}

void WebInspectorWindow::loadSettings()
{
	QSettings settings;
	settings.beginGroup(QLatin1StringView("UI"));
	settings.beginGroup(QLatin1StringView("Web-Inspector"));
	restoreGeometry(settings.value("geometry").toByteArray());
	if (isMaximized()) {
		setGeometry(QApplication::screenAt(geometry().center())->availableGeometry());
	}

	if (settings.value("isMaximized", isMaximized()).toBool()) {
		showMaximized();
	}
	if (settings.value("isFullScreen", isFullScreen()).toBool()) {
		showFullScreen();
	}
}

void WebInspectorWindow::saveSettings()
{
	QSettings settings;
	settings.beginGroup(QLatin1StringView("UI"));
	settings.beginGroup(QLatin1StringView("Web-Inspector"));

	settings.setValue("geometry", saveGeometry());
	settings.setValue("isMaximized", isMaximized());
	settings.setValue("isFullScreen", isFullScreen());
}

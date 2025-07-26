#ifndef QQSP_WEBINSPECTORWINDOW_H
#define QQSP_WEBINSPECTORWINDOW_H

#include <QMainWindow>

class QspWebBox;

namespace Ui {
	class WebInspectorWindow;
}

class WebInspectorWindow: public QMainWindow {
public:
	explicit WebInspectorWindow(QWidget* parent);
	~WebInspectorWindow() override;

	void setViews(QspWebBox* mainDescView, QspWebBox* additionalDescView);

private:
	void loadSettings();
	void saveSettings();

	Ui::WebInspectorWindow* _ui;
};

#endif

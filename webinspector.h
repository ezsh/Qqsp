#ifndef QQSP_WEBINSPECTOR_H
#define QQSP_WEBINSPECTOR_H

#include <QWebEngineView>

class QspWebBox;

class WebInspector: public QWebEngineView {
	Q_OBJECT

public:
	explicit WebInspector(QWidget* parent = nullptr);
	~WebInspector() override;

	void setView(QspWebBox* view);
	void inspectElement();

	QSize sizeHint() const override;

private Q_SLOTS:
	void loadFinished();

private:
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

	int m_height;
	QSize m_windowSize;
	bool m_inspectElement = false;
	QspWebBox* m_view;
};

#endif

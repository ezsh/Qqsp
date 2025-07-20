#include "webinspector.h"

#include "qspwebbox.h"

WebInspector::WebInspector(QWidget* parent)
	: QWebEngineView(parent)
	, m_view(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1StringView("web-inspector"));
	setMinimumHeight(100);

	connect(page(), &QWebEnginePage::windowCloseRequested, this, &WebInspector::deleteLater);
	connect(page(), &QWebEnginePage::loadFinished, this, &WebInspector::loadFinished);
}

WebInspector::~WebInspector()
{
	if (m_view && hasFocus()) {
		m_view->setFocus();
	}
}

void WebInspector::setView(QspWebBox* view)
{
	m_view = view;
	Q_ASSERT(isEnabled());

	page()->setInspectedPage(m_view->page());
	connect(m_view, &QspWebBox::pageChanged, this, &WebInspector::deleteLater);
}

void WebInspector::inspectElement()
{
	m_inspectElement = true;
}

void WebInspector::loadFinished()
{
	// Inspect element
	if (m_inspectElement) {
		m_view->triggerPageAction(QWebEnginePage::InspectElement);
		m_inspectElement = false;
	}
}

QSize WebInspector::sizeHint() const
{
	if (isWindow()) {
		return m_windowSize;
	}
	QSize s = QWebEngineView::sizeHint();
	s.setHeight(m_height);
	return s;
}

void WebInspector::keyPressEvent(QKeyEvent* event)
{
	Q_UNUSED(event)
}

void WebInspector::keyReleaseEvent(QKeyEvent* event)
{
	Q_UNUSED(event)
}

#include "qspexecwebengineurlschemehandler.h"

#include <QString>
#include <QBuffer>
#include <QUrl>
#include <QMessageBox>

#include <qsp_default.h>
#include "callbacks_gui.h"
#include "comtools.h"
#include "qspstr.h"

QspExecWebEngineUrlSchemeHandler::QspExecWebEngineUrlSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{

}

void QspExecWebEngineUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    url = request->requestUrl();
    QTimer::singleShot(0, this, &QspExecWebEngineUrlSchemeHandler::QspLinkClicked);
}

void QspExecWebEngineUrlSchemeHandler::QspLinkClicked()
{
    emit qspLinkClicked(url);
}

void QspExecWebEngineUrlSchemeHandler::legacyLinkClicked(QWebEngineUrlRequestJob *request)
{
    url = request->requestUrl();
    QString href;
    href = QByteArray::fromPercentEncoding(url.toString().toUtf8());
    QString string = href.mid(5);
    if (!QSPExecString(QSPStr(string), QSP_TRUE))
    {
        QString errorMessage;
        const QSPErrorInfo errorInfo{QSPGetLastErrorData()};
        QString desc = QSPTools::qspStrToQt(errorInfo.ErrorDesc);
        if (errorInfo.LocName.Str)
            errorMessage = QString("Location: %1\nArea: %2\nLine: %3\nCode: %4\nDesc: %5")
                               .arg(QSPTools::qspStrToQt(errorInfo.LocName))
                               .arg(errorInfo.ActIndex < 0 ? QString("on visit") : QString("on action"))
                               .arg(errorInfo.IntLineNum)
                               .arg(errorInfo.ErrorNum)
                               .arg(desc);
        else
            errorMessage = QString("Code: %1\nDesc: %2").arg(errorInfo.ErrorNum).arg(desc);
        QMessageBox dialog(QMessageBox::Critical, tr("Error"), errorMessage, QMessageBox::Ok);
        dialog.exec();
        QSPCallBacks::RefreshInt(QSP_FALSE, QSP_TRUE);
    }
    //request->redirect(QUrl("qsp:/"));
}

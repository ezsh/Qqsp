#include "qspwebchannel.h"

#include <QMessageBox>

#include <qsp_default.h>
#include "callbacks_gui.h"
#include "comtools.h"
#include "qspstr.h"

QspWebChannel::QspWebChannel(QObject *parent) : QObject(parent)
{

}

void QspWebChannel::ExecString(const QString &string)
{
    if (!QSPExecString(QSPStr(string), QSP_TRUE))
        ShowError();
}

void QspWebChannel::ShowError()
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

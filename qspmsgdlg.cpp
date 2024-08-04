#include "qspmsgdlg.h"

#include <QDesktopServices>
#include <QPalette>
#include <QPushButton>
#include <QRect>

#include "mainwindow.h"

#include "qspstr.h"

QspMsgDlg::QspMsgDlg(QWidget *parent) : QDialog(parent)
{

}

QspMsgDlg::~QspMsgDlg()
{

}

QspMsgDlg::QspMsgDlg(const QString &caption, const QString &text, QWidget *parent) : QDialog(parent) {
    setWindowTitle(caption);
    sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy().setVerticalPolicy(QSizePolicy::Expanding);
    connect(&m_desc, &QspTextBox::anchorClicked, this, &QspMsgDlg::OnLinkClicked);
    m_desc.setHtml(text);
    m_desc.sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    m_desc.sizePolicy().setVerticalPolicy(QSizePolicy::Expanding);
    dialogButtons.setStandardButtons({QDialogButtonBox::StandardButton::Ok});
    connect(&dialogButtons, &QDialogButtonBox::accepted, this, &QspMsgDlg::close);
    m_desc.document()->setTextWidth(400);
    resize(450, m_desc.document()->size().height() + 65);
    layout.addWidget(&m_desc);
    layout.addWidget(&dialogButtons);
    setLayout(&layout);
    setModal(true);
    //exec();
}

QspMsgDlg::QspMsgDlg(const QColor& backColor,
                     const QColor& fontColor,
                     const QFont& new_font,
                     const QString& caption,
                     const QString& text,
                     bool isHtml,
                     const QString& gamePath,
                     QWidget *parent) : QDialog(parent)
{
    m_desc.SetGamePath(gamePath);
    m_desc.SetIsHtml(isHtml);
    m_desc.SetBackgroundColor(backColor);
    m_desc.SetForegroundColor(fontColor);
    m_desc.SetTextFont(new_font);
    m_desc.SetText(text);
    m_desc.setBackgroundRole(QPalette::NoRole);
    QPalette p = palette();
    p.setColor(QPalette::Base, backColor);
    //p.setColor(QPalette::Window, backColor);
    setPalette(p);
    // ----------
    setWindowTitle(caption);
    sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy().setVerticalPolicy(QSizePolicy::Expanding);
    m_desc.sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    m_desc.sizePolicy().setVerticalPolicy(QSizePolicy::Expanding);
    dialogButtons.setStandardButtons({QDialogButtonBox::StandardButton::Ok});
    connect(&dialogButtons, &QDialogButtonBox::accepted, this, &QspMsgDlg::close);
    dialogButtons.button(QDialogButtonBox::StandardButton::Ok)->setDefault(true);
    dialogButtons.button(QDialogButtonBox::StandardButton::Ok)->setFont(new_font);
    dialogButtons.button(QDialogButtonBox::StandardButton::Ok)->setFocus(Qt::PopupFocusReason);

    m_desc.document()->setTextWidth(400);
    resize(450, m_desc.document()->size().height() + 65);

    layout.addWidget(&m_desc);
    layout.addWidget(&dialogButtons);
    setLayout(&layout);
    setModal(true);
    //exec();
}

void QspMsgDlg::OnLinkClicked(const QUrl &url)
{
    QString href;
    href = QByteArray::fromPercentEncoding(url.toString().toUtf8());

    if (href.startsWith("#"))
    {
        m_desc.setSource(url);
    }
    else if (href.startsWith("EXEC:", Qt::CaseInsensitive)) //NOTE: was not part of original player
    {
        QString string = href.mid(5);
        if (!QSPExecString(QSPStr(string), QSP_TRUE))
            if(this->parent() != 0)
                if(this->parent()->objectName() == QStringLiteral("MainWindow"))
                    qobject_cast<MainWindow*>(this->parent())->ShowError(); //TODO: replace with signal
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}

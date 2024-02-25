#include "callbacks_gui.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QThread>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QTimer>
#include <QEventLoop>

#include "comtools.h"
#include "qspmsgdlg.h"
#include "qspinputdlg.h"
#ifdef _WEBBOX
#include "qspwebbox.h"
#endif
#ifdef _WEBBOX_WEBKIT
#include "qspwebbox_webkit.h"
#endif

QString QSPCallBacks::m_gamePath;
MainWindow *QSPCallBacks::m_frame;
bool QSPCallBacks::m_isHtml;
QSPSounds QSPCallBacks::m_sounds;
float QSPCallBacks::m_volumeCoeff;
bool QSPCallBacks::m_isAllowHTML5Extras;

void QSPCallBacks::Init(MainWindow *frame)
{
	m_frame = frame;
    m_volumeCoeff = 1.0f;

    m_isAllowHTML5Extras = false;

	QSPSetCallBack(QSP_CALL_SETTIMER, (QSP_CALLBACK)&SetTimer);
	QSPSetCallBack(QSP_CALL_REFRESHINT, (QSP_CALLBACK)&RefreshInt);
	QSPSetCallBack(QSP_CALL_SETINPUTSTRTEXT, (QSP_CALLBACK)&SetInputStrText);
	QSPSetCallBack(QSP_CALL_ISPLAYINGFILE, (QSP_CALLBACK)&IsPlay);
	QSPSetCallBack(QSP_CALL_PLAYFILE, (QSP_CALLBACK)&PlayFile);
	QSPSetCallBack(QSP_CALL_CLOSEFILE, (QSP_CALLBACK)&CloseFile);
	QSPSetCallBack(QSP_CALL_SHOWMSGSTR, (QSP_CALLBACK)&Msg);
	QSPSetCallBack(QSP_CALL_SLEEP, (QSP_CALLBACK)&Sleep);
	QSPSetCallBack(QSP_CALL_GETMSCOUNT, (QSP_CALLBACK)&GetMSCount);
	QSPSetCallBack(QSP_CALL_SHOWMENU, (QSP_CALLBACK)&ShowMenu);
	QSPSetCallBack(QSP_CALL_INPUTBOX, (QSP_CALLBACK)&Input);
	QSPSetCallBack(QSP_CALL_SHOWIMAGE, (QSP_CALLBACK)&ShowImage);
	QSPSetCallBack(QSP_CALL_SHOWWINDOW, (QSP_CALLBACK)&ShowPane);
    //QSPSetCallBack(QSP_CALL_OPENGAME, (QSP_CALLBACK)&OpenGame); //replace
	QSPSetCallBack(QSP_CALL_OPENGAMESTATUS, (QSP_CALLBACK)&OpenGameStatus);
	QSPSetCallBack(QSP_CALL_SAVEGAMESTATUS, (QSP_CALLBACK)&SaveGameStatus);
    //TODO: implement this?
    //QSP_CALL_DEBUG, /* void func(QSPString str) */
}

void QSPCallBacks::DeInit()
{
    CloseFile({nullptr, nullptr});
}

void QSPCallBacks::SetTimer(int msecs)
{
	if (m_frame->IsQuit()) return;
	if (msecs)
        m_frame->GetTimer()->start(msecs);
	else
        m_frame->GetTimer()->stop();
}

void QSPCallBacks::RefreshInt(QSP_BOOL isRedraw)
{
	static int oldFullRefreshCount = 0;
	int numVal;
	bool isScroll, isCanSave;
    QSPString strVal;
	if (m_frame->IsQuit()) return;
    // -------------------------------
    UpdateGamePath(m_frame->gameFilePath());
    // -------------------------------
    const QSPString mainDesc = QSPGetMainDesc();
    const QSPString varsDesc = QSPGetVarsDesc();
    // -------------------------------
    isScroll = !(QSPGetVarValues(L"DISABLESCROLL"_qsp, 0, &numVal, &strVal) && numVal);
    isCanSave = !(QSPGetVarValues(L"NOSAVE"_qsp, 0, &numVal, &strVal) && numVal);
    m_isHtml = QSPGetVarValues(L"USEHTML"_qsp, 0, &numVal, &strVal) && numVal;
    // -------------------------------
	m_frame->GetVars()->SetIsHtml(m_isHtml);
	if (QSPIsVarsDescChanged())
	{
        m_frame->EnableControls(false, true);
        if(m_isAllowHTML5Extras)
        {
            if (QSPGetVarValues(L"SETSTATHEAD"_qsp, 0, &numVal, &strVal) && strVal.Str)
                m_frame->GetVars()->SetHead(QSPTools::qspStrToQt(strVal));
            else
                 m_frame->GetVars()->SetHead(QString(""));
        }
        m_frame->GetVars()->SetText(QSPTools::qspStrToQt(varsDesc), isScroll);
        m_frame->EnableControls(true, true);
	}
	// -------------------------------
	int fullRefreshCount = QSPGetFullRefreshCount();
	if (oldFullRefreshCount != fullRefreshCount)
	{
		isScroll = false;
		oldFullRefreshCount = fullRefreshCount;
	}
	m_frame->GetDesc()->SetIsHtml(m_isHtml);
    if (QSPIsMainDescChanged())
    {
        m_frame->EnableControls(false, true);
        if(m_isAllowHTML5Extras)
        {
            if (QSPGetVarValues(L"SETMAINDESCHEAD"_qsp, 0, &numVal, &strVal) && strVal.Str)
                m_frame->GetDesc()->SetHead(QSPTools::qspStrToQt(strVal));
            else
                 m_frame->GetDesc()->SetHead(QString(""));
        }
        m_frame->GetDesc()->SetText(QSPTools::qspStrToQt(mainDesc), isScroll);
        m_frame->EnableControls(true, true);
	}
	// -------------------------------
	m_frame->GetActions()->SetIsHtml(m_isHtml);
	m_frame->GetActions()->SetIsShowNums(m_frame->IsShowHotkeys());
    if (QSPIsActionsChanged())
    {
        std::vector<QSPListItem> actions = QSPTools::qspActions();
        m_frame->GetActions()->BeginItems();
        for (const QSPListItem& li: actions)
        {
            m_frame->GetActions()->AddItem(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(li.Image)), QSPTools::qspStrToQt(li.Name));
        }
        m_frame->GetActions()->EndItems();
    }
    m_frame->GetActions()->SetSelection(QSPGetSelActionIndex());
	m_frame->GetObjects()->SetIsHtml(m_isHtml);
    if (QSPIsObjectsChanged())
    {
        std::vector<QSPListItem> objects = QSPTools::qspObjects();
        m_frame->GetObjects()->BeginItems();
        for (const QSPListItem& li: objects)
        {
            m_frame->GetObjects()->AddItem(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(li.Image)), QSPTools::qspStrToQt(li.Name));
        }
        m_frame->GetObjects()->EndItems();
    }
	m_frame->GetObjects()->SetSelection(QSPGetSelObjectIndex());
	// -------------------------------
    if (QSPGetVarValues(L"BACKIMAGE"_qsp, 0, &numVal, &strVal) && strVal.Str)
        m_frame->GetDesc()->LoadBackImage(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(strVal)));
	else
        m_frame->GetDesc()->LoadBackImage(QString(""));
    // -------------------------------
    m_frame->ApplyParams();
	if (isRedraw)
	{
		m_frame->EnableControls(false, true);
        //m_frame->Update();
        //QCoreApplication::processEvents();
		if (m_frame->IsQuit()) return;
		m_frame->EnableControls(true, true);
	}
    m_frame->GetGameMenu()->setEnabled(isCanSave);
}

void QSPCallBacks::SetInputStrText(QSPString text)
{
	if (m_frame->IsQuit()) return;
    m_frame->GetInput()->SetText(QSPTools::qspStrToQt(text));
}

QSP_BOOL QSPCallBacks::IsPlay(QSPString file)
{
    QSP_BOOL playing = QSP_FALSE;
    QSPSounds::iterator elem = m_sounds.find(QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath());
    if (elem != m_sounds.end())
        if(elem.value()->state() == QMediaPlayer::PlayingState)
            playing = QSP_TRUE;
    return playing;
}

void QSPCallBacks::CloseFile(QSPString file)
{
    if (file.Str) {
        QSPSounds::iterator elem = m_sounds.find(QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath());
		if (elem != m_sounds.end()) {
            delete elem.value();
			m_sounds.erase(elem);
		}
	} else {
        for (QSPSounds::iterator i = m_sounds.begin(); i != m_sounds.end(); ++i)
            delete i.value();
		m_sounds.clear();
	}
}

void QSPCallBacks::PlayFile(QSPString file, int volume)
{
    if (SetVolume(file, volume)) return;
    CloseFile(file);
    QString strFile(QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath());
    QMediaPlayer *snd = new QMediaPlayer();
    snd->setMedia(QUrl::fromLocalFile(strFile));
    snd->setVolume(volume*m_volumeCoeff);
    snd->play();
    m_sounds.insert(strFile, snd);
    UpdateSounds();
}

void QSPCallBacks::ShowPane(int type, QSP_BOOL isShow)
{
	if (m_frame->IsQuit()) return;
	switch (type)
	{
	case QSP_WIN_ACTS:
        m_frame->GetActionsDock()->setVisible(isShow != QSP_FALSE);
		break;
	case QSP_WIN_OBJS:
        m_frame->GetObjectsDock()->setVisible(isShow != QSP_FALSE);
		break;
	case QSP_WIN_VARS:
        m_frame->GetVarsDock()->setVisible(isShow != QSP_FALSE);
		break;
	case QSP_WIN_INPUT:
        m_frame->GetInputDock()->setVisible(isShow != QSP_FALSE);
		break;
	}
}

void QSPCallBacks::Sleep(int msecs)
{
    QTimer wtimer;
    wtimer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&wtimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    wtimer.start(50);
    loop.exec();
    //RefreshInt(QSP_TRUE);
	if (m_frame->IsQuit()) return;
    bool isSave = m_frame->GetGameMenu()->isEnabled();
	bool isBreak = false;
    m_frame->EnableControls(false, true);
	int i, count = msecs / 50;
	for (i = 0; i < count; ++i)
    {
        //QThread::msleep(50);
        wtimer.start(50);
        loop.exec();
        //qDebug() << QSPTools::qspStrToQt(QSPGetMainDesc());
        //m_frame->Update();
        //QCoreApplication::processEvents();
		if (m_frame->IsQuit() ||
            m_frame->IsKeyPressedWhileDisabled()) //TODO: implement
		{
			isBreak = true;
			break;
		}
	}
    if (!isBreak) //NOTE: no check in old code
    {
        //QThread::msleep(msecs % 50);
        wtimer.start(msecs % 50);
        loop.exec();
        //m_frame->Update();
        //QCoreApplication::processEvents();
    }
	m_frame->EnableControls(true, true);
    m_frame->GetGameMenu()->setEnabled(isSave);
}

int QSPCallBacks::GetMSCount()
{
    static QElapsedTimer stopWatch;
    if(stopWatch.isValid() == false)
        stopWatch.start();
    int ret = stopWatch.restart();
	return ret;
}

void QSPCallBacks::Msg(QSPString str)
{
	if (m_frame->IsQuit()) return;
	RefreshInt(QSP_FALSE);
    QspMsgDlg dialog(m_frame->GetDesc()->GetBackgroundColor(),
        m_frame->GetDesc()->GetForegroundColor(),
		m_frame->GetDesc()->GetTextFont(),
        MainWindow::tr("Info"), //caption
        QSPTools::qspStrToQt(str),
		m_isHtml,
        m_gamePath,
        m_frame
	);
	m_frame->EnableControls(false);
    dialog.exec();
	m_frame->EnableControls(true);
}

void QSPCallBacks::DeleteMenu()
{
    if (m_frame->IsQuit()) return;
    m_frame->DeleteMenu();
}

void QSPCallBacks::AddMenuItem(const QSPString& name, const QSPString& imgPath)
{
    if (m_frame->IsQuit()) return;
    m_frame->AddMenuItem(QSPTools::qspStrToQt(name), QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(imgPath)));
}

int QSPCallBacks::ShowMenu(const QSPListItem* items, int count)
{
    if (m_frame->IsQuit()) return -1;
    m_frame->EnableControls(false);
    DeleteMenu();
    if (!count) {
        return 0;
    }
    for (int i = 0; i < count; ++i) {
        AddMenuItem(items[i].Name, items[i].Image);
    }
    int index = m_frame->ShowMenu();
    m_frame->EnableControls(true);
    return index;
}

void QSPCallBacks::Input(QSPString text, QSP_CHAR *buffer, int maxLen)
{
	if (m_frame->IsQuit()) return;
    RefreshInt(QSP_FALSE);
//	QSPInputDlg dialog(m_frame,
//		wxID_ANY,
//		m_frame->GetDesc()->GetBackgroundColor(),
//		m_frame->GetDesc()->GetForegroundColor(),
//		m_frame->GetDesc()->GetTextFont(),
//		_("Input data"),
//		wxString(text.Str, text.End),
//		m_isHtml,
//		m_gamePath
//	);
//	m_frame->EnableControls(false);
//	dialog.ShowModal();
//	m_frame->EnableControls(true);
    //#ifdef _UNICODE
	// 	wcsncpy(buffer, inputText.data(), maxLen);
	// #else
	// 	strncpy(buffer, dialog.GetText().c_str(), maxLen);
	// #endif
    //QString inputText = QInputDialog::getMultiLineText(m_frame, MainWindow::tr("Input data"), QSPTools::qspStrToQt(text));

    QString inputText = QInputDialog::getText(m_frame, MainWindow::tr("Input data"), QSPTools::qspStrToQt(text), QLineEdit::Normal);
    QSPTools::qtStrToQspBuffer(inputText, buffer, maxLen);
}

void QSPCallBacks::ShowImage(QSPString file)
{
	if (m_frame->IsQuit()) return;
    m_frame->GetImgView()->OpenFile(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))); //NOTE: will not display image if file is not found
    if(QSPTools::qspStrToQt(file) == "")
    {
        m_frame->GetImageDock()->setVisible(false);
    }
    else
    {
        m_frame->GetImageDock()->setVisible(true);
    }

    //m_frame->GetImgView()->setVisible(true);
}

//void QSPCallBacks::OpenGame(const QSP_CHAR *file, QSP_BOOL isNewGame)
//{
//	if (m_frame->IsQuit()) return;
//	if (QSPLoadGameWorld(file, isNewGame) && isNewGame)
//	{
//        QFileInfo fileName(QSPTools::qspStrToQt(file));
//        m_gamePath = fileName.canonicalPath();
//        if(!m_gamePath.endsWith('/')) m_gamePath+="/";
//		m_frame->UpdateGamePath(m_gamePath);
//	}
//}

bool QSPCallBacks::OpenGameStatusEx(const QSPString& file, bool isReferesh)
{
	if (m_frame->IsQuit()) return false;
    const auto openSave = [isReferesh](QString path) -> bool {
        QFileInfo fileInfo(path);
        if (fileInfo.exists() && fileInfo.isFile()) {
            QFile inp{fileInfo.path()};
            if (inp.open(QFile::ReadOnly)) {
                QByteArray data = inp.readAll();
                return QSPOpenSavedGameFromData(data.data(), data.size(), isReferesh ? QSP_TRUE : QSP_FALSE);
            }
        }
        return false;
    };

    if (file.Str) {
        return openSave(QSPTools::qspStrToQt(file));
	} else {
        m_frame->EnableControls(false);
        QString path = QFileDialog::getOpenFileName(m_frame, MainWindow::tr("Select saved game file"), m_frame->GetLastPath(), MainWindow::tr("Saved game files (*.sav)"));
        m_frame->EnableControls(true);
        if (!path.isEmpty()) {
            m_frame->SetLastPath(QFileInfo(path).canonicalPath());
            return openSave(path);
		}
		return false;
	}
}

void QSPCallBacks::OpenGameStatus(QSPString file)
{
    OpenGameStatusEx(file, false);
}

bool QSPCallBacks::SaveGameStatusEx(const QSPString& file, bool isRefresh)
{
	if (m_frame->IsQuit()) return false;

    const auto makeSaveFile = [isRefresh](QString file) -> bool {
        int requiredBufSize;
        QSPSaveGameAsData(nullptr, &requiredBufSize, isRefresh ? QSP_TRUE : QSP_FALSE);
        QByteArray buf{requiredBufSize, Qt::Uninitialized};
        if (QSPSaveGameAsData(buf.data(), &requiredBufSize, isRefresh ? QSP_TRUE : QSP_FALSE)) {
            QFile out{file};
            if (!out.open(QIODevice::WriteOnly)) {return false;}
            return out.write(buf) == buf.size();
        }
        return false;
    };
    if (file.Str) {
        return makeSaveFile(QSPTools::qspStrToQt(file));
    } else {
        m_frame->EnableControls(false);
        QString path = QFileDialog::getSaveFileName(m_frame, MainWindow::tr("Select file to save"), m_frame->GetLastPath(), MainWindow::tr("Saved game files (*.sav)"));
		m_frame->EnableControls(true);
        if (!path.isEmpty()) {
            m_frame->SetLastPath(QFileInfo(path).canonicalPath());
            return makeSaveFile(path);
		}
		return false;
	}
}

void QSPCallBacks::SaveGameStatus(QSPString file)
{
    SaveGameStatusEx(file, false);
}

void QSPCallBacks::UpdateGamePath(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    m_gamePath = fileInfo.canonicalPath();
    if(!m_gamePath.endsWith("/")) m_gamePath+="/";
    //m_frame->UpdateGamePath(m_gamePath);
    m_frame->GetDesc()->SetGamePath(m_gamePath);
    m_frame->GetObjects()->SetGamePath(m_gamePath);
    m_frame->GetActions()->SetGamePath(m_gamePath);
    m_frame->GetVars()->SetGamePath(m_gamePath);
    m_frame->GetImgView()->SetGamePath(m_gamePath);
}

bool QSPCallBacks::SetVolume(const QSPString& file, int volume)
{
    if (!IsPlay(file)) return false;
    QSPSounds::iterator elem = m_sounds.find(QString(QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath()));
    QMediaPlayer *snd = elem.value();
    snd->setVolume(volume*m_volumeCoeff);
	return true;
}

void QSPCallBacks::SetOverallVolume(float coeff)
{
    QMediaPlayer *snd;
    if (coeff < 0.0)
        coeff = 0.0;
    else if (coeff > 1.0)
        coeff = 1.0;
    m_volumeCoeff = coeff;
    for (QSPSounds::iterator i = m_sounds.begin(); i != m_sounds.end(); ++i)
    {
        snd = i.value();
        if (snd->state() == QMediaPlayer::PlayingState)
            snd->setVolume(snd->volume()*m_volumeCoeff);
    }
}

void QSPCallBacks::SetAllowHTML5Extras(bool HTML5Extras)
{
    m_isAllowHTML5Extras = HTML5Extras;
}

void QSPCallBacks::UpdateSounds()
{
    QMediaPlayer *snd;
    QSPSounds::iterator i = m_sounds.begin();
    while (i != m_sounds.end())
    {
        snd = i.value();
        if(snd->state() == QMediaPlayer::PlayingState)
            ++i;
        else
        {
            delete snd;
            i = m_sounds.erase(i);
        }
    }
}

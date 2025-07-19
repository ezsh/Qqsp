#ifndef CALLBACKS_GUI_H
#define CALLBACKS_GUI_H

#include "mainwindow.h"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QString>
#include <qsp_default.h>

#include <map>

class DebugLogWindow;

struct QSPSound
{
    QSPSound();
    QSPSound(QSPSound &&other);
    ~QSPSound();

    QMediaPlayer *player;
    QAudioOutput *output;
};

typedef std::map<QString, QSPSound> QSPSounds;

//static QSPString qspStringFromPair(const QSP_CHAR *start, const QSP_CHAR *end)
//{
//    QSPString string;
//    string.Str = (QSP_CHAR *)start;
//    string.End = (QSP_CHAR *)end;
//    return string;
//}

//static QSPString qspStringFromLen(const QSP_CHAR *s, int len)
//{
//    QSPString string;
//    string.Str = (QSP_CHAR *)s;
//    string.End = (QSP_CHAR *)s + len;
//    return string;
//}

// static const QSPString qspStringFromQString(const QString &s)
// {
//     //QSPString string;
//     //string.Str = (QSP_CHAR *)s.utf16();
//     //string.End = (QSP_CHAR *)s.utf16() + s.length();
//    // return (QSP_CHAR *)s.utf16();
//     return {(QSP_CHAR *)s.utf16(), (QSP_CHAR *)s.utf16() + s.length()};
// }

class QSPCallBacks
{
public:
    // Methods
    static void Init(MainWindow* frame, DebugLogWindow* debugLogWindow);
    static void DeInit();
    static void SetOverallVolume(float coeff);
    static void SetAllowHTML5Extras(bool HTML5Extras);

    // CallBacks
    static void RefreshInt(QSP_BOOL isForced, QSP_BOOL isNewDesc);
    static void SetTimer(int msecs);
    static void SetInputStrText(QSPString text);
    static QSP_BOOL IsPlay(QSPString file);
    static void CloseFile(QSPString file);
    static void PlayFile(QSPString file, int volume);
    static void ShowPane(int type, QSP_BOOL isShow);
    static void Sleep(int msecs);
    static int GetMSCount();
    static void Msg(QSPString str);
    static void DeleteMenu();
    static void AddMenuItem(const QSPString& name, const QSPString& imgPath);
    static int ShowMenu(const QSPListItem* items, int count);
    static void Input(QSPString text, QSP_CHAR *buffer, int maxLen);
    static void ShowImage(QSPString file);
    static void OpenGame(QSPString file, QSP_BOOL isNewGame);
    static void OpenGameStatus(QSPString file);
    static bool OpenGameStatusEx(const QSPString& file, bool isRefresh);
    static void SaveGameStatus(QSPString file);
    static bool SaveGameStatusEx(const QSPString& file, bool isRefresh);
    static void Debug(QSPString str);

    static QString m_gamePath;
private:
    // Internal methods
    static void UpdateGamePath(const QString& fileName);
    static bool SetVolume(const QSPString& file, int volume);
    static void UpdateSounds();

    // Fields
    static MainWindow *m_frame;
    static DebugLogWindow* m_debugLogWindow;
    static bool m_isHtml;
    static QSPSounds m_sounds;
    static float m_volumeCoeff;
    static bool m_isAllowHTML5Extras;
};

#endif

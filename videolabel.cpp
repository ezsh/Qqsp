#include "videolabel.h"

#include <QMediaMetaData>
#include <QUrl>

// #include <QCoreApplication>
// #include <QThread>

VideoLabel::VideoLabel(QString path, QString filename, QWidget* parent)
	: QVideoWidget(parent)
{
	m_path = path;
	m_filename = filename;
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	resolution_set = false;
	m_medialLoaded = false;

	mediaPlayer.setSource(QUrl::fromLocalFile(m_path + m_filename));
	mediaPlayer.setLoops(QMediaPlayer::Infinite);
	mediaPlayer.setVideoOutput(this);
	mediaPlayer.play();
	//    while(!vfp.hasFrame && mediaPlayer.error() != QMediaPlayer::InvalidMedia && vfp.error() == QAbstractVideoSurface::NoError)
	//    {
	//        QCoreApplication::processEvents();
	//        //QThread::msleep(4);
	//    }

	//    if(mediaPlayer.error() != QMediaPlayer::InvalidMedia && vfp.error() == QAbstractVideoSurface::NoError)
	//    {
	m_videoError = false;
}

VideoLabel::~VideoLabel() {}

bool VideoLabel::videoError()
{
	return mediaPlayer.error() != QMediaPlayer::NoError;
}

QSize VideoLabel::getResolution()
{
	return mediaPlayer.videoTracks().front().value(QMediaMetaData::Resolution).toSize();
}

#ifndef PREVIEWPLAYER_H
#define PREVIEWPLAYER_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#ifdef LIBVLC
#include "vlc/vlc.h"
#else
#include "VLCQtCore/Common.h"
#include "VLCQtCore/Instance.h"
#include "VLCQtCore/Media.h"
#include "VLCQtCore/MediaPlayer.h"
#endif

class PreviewPlayer : public QObject
{
    Q_OBJECT

#ifdef LIBVLC
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *mediaPlayer;
    libvlc_media_t *media;
    bool isPlaying;
#else
    VlcInstance *vlcInstance;
    VlcMedia *media;
#endif
public:
#ifdef LIBVLC
    explicit PreviewPlayer(QObject *parent = nullptr);
#else
    explicit PreviewPlayer(VlcVideoDelegate *videoArea, QObject *parent = nullptr);
#endif
    VlcMediaPlayer *mediaPlayer;
    ~PreviewPlayer();
    //float GetTimelinePosition();
    //int GetVolume();

    bool isPlaying;
    void ResetCurrentMedia();
signals:

public slots:
#ifdef LIBVLC
    void playFile(QString file, WId windowId);
#else
    void playFile(QString file);
    void playPause();
#endif
    //void changeVolume(int newVolume);
    //void changeTimelinePosition(int newTimelinePosition, unsigned int positionResolution);
};

#endif // PREVIEWPLAYER_H

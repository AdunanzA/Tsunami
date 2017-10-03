#include "previewplayer.h"

#ifdef LIBVLC
PreviewPlayer::PreviewPlayer(QObject *parent) : QObject(parent)
{
    const char * const vlc_args[] = {
                  "--verbose=2", //be much more verbose then normal for debugging purpose
     };

    vlcInstance = libvlc_new(1, vlc_args);
    mediaPlayer = libvlc_media_player_new(vlcInstance);
#else
PreviewPlayer::PreviewPlayer(VlcVideoDelegate *videoArea, QObject *parent) : QObject(parent)
{
    vlcInstance = new VlcInstance(VlcCommon::args(), this);
    mediaPlayer = new VlcMediaPlayer(vlcInstance);
    mediaPlayer->setVideoWidget(videoArea);
    media = 0;
    isPlaying = false;
#endif
}

PreviewPlayer::~PreviewPlayer()
{
#ifdef LIBVLC
    libvlc_media_player_stop(mediaPlayer);
    libvlc_media_player_release(mediaPlayer);
    libvlc_release(vlcInstance);
#else
    delete mediaPlayer;
    if(media != 0)
    {
        delete media;
    }
    delete vlcInstance;
#endif
}
#ifdef LIBVLC
void PreviewPlayer::playFile(QString file, WId windowId)
{

    media = libvlc_media_new_path(vlcInstance, file.toLocal8Bit().constData());
    libvlc_media_player_set_media(mediaPlayer, media);

#if defined(Q_OS_WIN)
        libvlc_media_player_set_nsobject(mediaPlayer, &windowId);
        //libvlc_media_player_set_drawable(_mp, reinterpret_cast<unsigned int>(_videoWidget->winId()), &_vlcexcep ); // [20101215 JG] Used for versions prior to VLC 1.2.0.
        //libvlc_media_player_set_hwnd(_mp, _videoWidget->winId(), &_vlcexcep ); // for vlc 1.0
    #elif defined(Q_OS_MAC)
        libvlc_media_player_set_drawable(mediaPlayer, windowId);
        //libvlc_media_player_set_drawable(_mp, _videoWidget->winId(), &_vlcexcep ); // [20101215 JG] Used for versions prior to VLC 1.2.0.
        //libvlc_media_player_set_agl (_mp, _videoWidget->winId(), &_vlcexcep); // for vlc 1.0
    #else //Linux
        //[20101201 Ondrej Spilka] obsolete call on libVLC >=1.1.5
        //libvlc_media_player_set_drawable(_mp, _videoWidget->winId(), &_vlcexcep );
        //libvlc_media_player_set_xwindow(_mp, _videoWidget->winId(), &_vlcexcep ); // for vlc 1.0

     //again note X11 handle on Linux is needed
       // winID() returns X11 handle when QX11EmbedContainer us used

        //int windid = _videoWidget->winId();
        libvlc_media_player_set_xwindow (mediaPlayer, windowId );

    #endif

    libvlc_media_player_play(mediaPlayer);
    isPlaying = true;
#else
void PreviewPlayer::playFile(QString file)
{
    if(file.isEmpty())
        return;
    media = new VlcMedia(file, true, vlcInstance);
    mediaPlayer->open(media);
    isPlaying = true;
#endif
}

/*void PreviewPlayer::changeVolume(int newVolume)
{
//    libvlc_audio_set_volume (mediaPlayer, newVolume);
}

void PreviewPlayer::changeTimelinePosition(int newPosition, unsigned int positionResolution)
{

//    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    // It's possible that the vlc doesn't play anything
//    // so check before
//    libvlc_media_t *curMedia = libvlc_media_player_get_media (mediaPlayer);
//    //libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    if (curMedia == NULL)
//        return;

//    float pos=(float)(newPosition)/(float)positionResolution;
//    libvlc_media_player_set_position (mediaPlayer, pos);
//    //libvlc_media_player_set_position (_mp, pos, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    //raise(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
}

int PreviewPlayer::GetVolume()
{
//    if(!isPlaying)
//        return -1;

//    // It's possible that the vlc doesn't play anything
//    // so check before
//    libvlc_media_t *curMedia = libvlc_media_player_get_media (mediaPlayer);
//    //libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    if (curMedia == NULL)
//        return -1;

//    return libvlc_audio_get_volume(mediaPlayer);

    return 0;
}

float PreviewPlayer::GetTimelinePosition()
{
//    if(!isPlaying)
//        return -1.0;

//    // It's possible that the vlc doesn't play anything
//    // so check before
//    libvlc_media_t *curMedia = libvlc_media_player_get_media (mediaPlayer);
//    //libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
//    if (curMedia == NULL)
//        return -1.0;

//    return libvlc_media_player_get_position (mediaPlayer);

    return 0;
}*/

void PreviewPlayer::playPause()
{
    if(media != 0)
    {
        if(isPlaying)
        {
            mediaPlayer->pause();
        }
        else
        {
            mediaPlayer->play();
        }
        isPlaying = !isPlaying;
    }
}

void PreviewPlayer::ResetCurrentMedia()
{
    mediaPlayer->openOnly(media);
}

#include "previewplayerwindow.h"
#include "ui_previewplayerwindow.h"
#include <QFileDialog>
#include <QKeyEvent>

#define PLAY_STRING "Play"
#define PAUSE_STRING "Pause"

#define PLAY_IMAGE_RESOURCE ":/images/tsuplayer/Play.png"
#define PAUSE_IMAGE_RESOURCE ":/images/tsuplayer/Pause.png"

PreviewPlayerWindow::PreviewPlayerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewPlayerWindow)
{
    ui->setupUi(this);

    previewPlayer = new PreviewPlayer(ui->videoAreaWidget);
    ui->videoAreaWidget->setMediaPlayer(previewPlayer->mediaPlayer);
    ui->volumeWidget->setMediaPlayer(previewPlayer->mediaPlayer);
    ui->volumeWidget->setVolume(50);
    ui->volumeWidget->setOrientation(Qt::Orientation::Horizontal);              //Set audio slider horizontal
    //ui->volumeWidget->setTracking(true);
    ui->seekWidget->setMediaPlayer(previewPlayer->mediaPlayer);
    ui->seekWidget->setStyleSheet("color:white");                               //allows the time to be shown on a black background

    connect(previewPlayer->mediaPlayer, SIGNAL(end()), this, SLOT(mediaStopped()));
    connect(previewPlayer->mediaPlayer, SIGNAL(error()), this, SLOT(mediaStopped()));

    //previewPlayer->playFile("C:/Users/Riccardo/Desktop/test.mkv");

    ui->pushButtonOpenFile->setStyleSheet("background-color:white");
    ui->pushButtonPlayPause->setStyleSheet("background-color:white");

    QIcon tempIcon;
    tempIcon.addFile(":/images/tsuplayer/Full_Screen.png", QSize(), QIcon::Normal, QIcon::Off);
    ui->pushButtonFullscreen->setIcon(tempIcon);
    ui->pushButtonFullscreen->setIconSize(QSize(30,30));
    ui->pushButtonFullscreen->setText("");

    tempIcon.addFile(":/images/tsuplayer/OpenFile.png", QSize(), QIcon::Normal, QIcon::Off);
    ui->pushButtonOpenFile->setIcon(tempIcon);
    ui->pushButtonOpenFile->setIconSize(QSize(30,30));
    ui->pushButtonOpenFile->setText("");

    updatePlayPauseButton();

    seekSlider = new QSlider(Qt::Orientation::Horizontal);
    seekSlider->setStyleSheet("QSlider::groove:horizontal { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6B6A68, stop: 0.5 #212121, stop: 1 #000000); height: 8px; border-top-right-radius: 4px; border-bottom-right-radius: 4px; border-top-left-radius: 4px; border-bottom-left-radius: 4px; } QSlider::handle:horizontal {  } QSlider::add-page:horizontal { } QSlider::sub-page:horizontal { background: qlineargradient(x1: 0.2, y1: 0, x2: 0.2, y2: 1, stop: 0 #6BEAFD, stop: 0.5 #125292, stop: 1 #0F1014); border-top-right-radius: 4px; border-bottom-right-radius: 4px; border-top-left-radius: 4px; border-bottom-left-radius: 4px; margin: 0px 2px 0px 1px; }");
    ui->seekWidget->setSliderWidget(seekSlider, true);

    ui->volumeWidget->setStyleSheet("QSlider::groove:horizontal { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6B6A68, stop: 0.5 #212121, stop: 1 #000000); height: 8px; border-top-right-radius: 4px; border-bottom-right-radius: 4px; border-top-left-radius: 4px; border-bottom-left-radius: 4px; } QSlider::handle:horizontal {  } QSlider::add-page:horizontal { } QSlider::sub-page:horizontal { background: qlineargradient(x1: 0.2, y1: 0, x2: 0.2, y2: 1, stop: 0 #C2934D, stop: 0.5 #A07028, stop: 1 #895D1C); border-top-right-radius: 4px; border-bottom-right-radius: 4px; border-top-left-radius: 4px; border-bottom-left-radius: 4px; margin: 0px 2px 0px 1px; }");

    qApp->installEventFilter(this);
    //this->setStyleSheet("background-color:white");
}

PreviewPlayerWindow::~PreviewPlayerWindow()
{
    delete seekSlider;
    delete previewPlayer;
    delete ui;
}

void PreviewPlayerWindow::on_pushButtonOpenFile_clicked()
{
    // Propose download folder as starting path

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video Stream"),
                                                    (!m_downloadPath.isEmpty() ? m_downloadPath : QDir::currentPath()),
                                                    QString("%1 (*.mkv *.avi *.mp4 *.mov *.wmv)").arg(tr("Video Files")));
    if (!fileName.isEmpty()) {
        previewPlayer->playFile(fileName);
        updatePlayPauseButton();
    }
}

void PreviewPlayerWindow::on_pushButtonPlayPause_clicked()
{
    previewPlayer->playPause();
    this->updatePlayPauseButton();
}

void PreviewPlayerWindow::updatePlayPauseButton()
{
    QIcon playPauseIcon;
    if(previewPlayer->isPlaying)
    {
        playPauseIcon.addFile(PAUSE_IMAGE_RESOURCE, QSize(), QIcon::Normal, QIcon::Off);

    }
    else
    {
        playPauseIcon.addFile(PLAY_IMAGE_RESOURCE, QSize(), QIcon::Normal, QIcon::Off);
    }
    ui->pushButtonPlayPause->setIcon(playPauseIcon);
    ui->pushButtonPlayPause->setIconSize(QSize(30,30));
    ui->pushButtonPlayPause->setText("");
}

void PreviewPlayerWindow::mediaStopped()
{
    previewPlayer->isPlaying = false;
    updatePlayPauseButton();
    previewPlayer->ResetCurrentMedia();
}

void PreviewPlayerWindow::PauseForHidden()
{
    if(previewPlayer->isPlaying)
    {
        on_pushButtonPlayPause_clicked();
        previewPlayer->rewindForHidden();
    }
}

void PreviewPlayerWindow::on_pushButtonFullscreen_clicked()
{
    //ui->videoAreaWidget->showFullScreen();
    ui->videoAreaWidget->setWindowFlags(Qt::Window);
    ui->videoAreaWidget->setWindowState(Qt::WindowFullScreen);
    ui->videoAreaWidget->show();
}

bool PreviewPlayerWindow::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent;
    QEvent::Type actualEvent = event->type();
    if (actualEvent == QEvent::KeyPress)
    {
        keyEvent = static_cast<QKeyEvent *>(event);
    }
    switch(actualEvent)
    {
        case QEvent::KeyPress:
            if(keyEvent->key() == Qt::Key_Escape && obj == ui->videoAreaWidget)
            {
                if(ui->videoAreaWidget->isFullScreen())
                {
                   ui->videoAreaWidget->setWindowFlags(Qt::Widget);
                   ui->videoAreaWidget->showNormal();
                }
            }
        break;
        case QEvent::MouseButtonDblClick:
            if(ui->videoAreaWidget->underMouse())
            {
                if(ui->videoAreaWidget->isFullScreen())
                {
                    ui->videoAreaWidget->setWindowFlags(Qt::Widget);
                    ui->videoAreaWidget->showNormal();
                }
                else
                {
                    on_pushButtonFullscreen_clicked();
                }
            }
        break;
        default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

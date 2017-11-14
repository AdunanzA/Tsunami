#include "previewplayerwindow.h"
#include "ui_previewplayerwindow.h"
#include <QFileDialog>

#define PLAY_STRING "Play"
#define PAUSE_STRING "Pause"

PreviewPlayerWindow::PreviewPlayerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewPlayerWindow)
{
    ui->setupUi(this);


    previewPlayer = new PreviewPlayer(ui->videoAreaWidget);
    ui->videoAreaWidget->setMediaPlayer(previewPlayer->mediaPlayer);
    ui->volumeWidget->setMediaPlayer(previewPlayer->mediaPlayer);
    ui->volumeWidget->setVolume(50);
    //ui->volumeWidget->setTracking(true);
    ui->seekWidget->setMediaPlayer(previewPlayer->mediaPlayer);
    ui->seekWidget->setStyleSheet("color:white");                               //allows the time to be shown on a black background

    connect(previewPlayer->mediaPlayer, SIGNAL(end()), this, SLOT(mediaStopped()));
    connect(previewPlayer->mediaPlayer, SIGNAL(error()), this, SLOT(mediaStopped()));

    //previewPlayer->playFile("C:/Users/Riccardo/Desktop/test.mkv");

    ui->pushButtonOpenFile->setStyleSheet("background-color:white");
    ui->pushButtonPlayPause->setStyleSheet("background-color:white");
    //this->setStyleSheet("background-color:white");
}

PreviewPlayerWindow::~PreviewPlayerWindow()
{
    delete previewPlayer;
    delete ui;
}

void PreviewPlayerWindow::on_pushButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Video Stream", QDir::currentPath(), "Video Files (*.mkv *.avi *.mp4 *.mov *.wmv)");
    if(fileName != "")
    {
        previewPlayer->playFile(fileName);
        this->updatePlayPauseButton();
    }
}

void PreviewPlayerWindow::on_pushButtonPlayPause_clicked()
{
    previewPlayer->playPause();
    this->updatePlayPauseButton();
}

void PreviewPlayerWindow::updatePlayPauseButton()
{
    if(previewPlayer->isPlaying)
    {
        ui->pushButtonPlayPause->setText(PAUSE_STRING);
    }
    else
    {
        ui->pushButtonPlayPause->setText(PLAY_STRING);
    }
}

void PreviewPlayerWindow::mediaStopped()
{
    previewPlayer->isPlaying = false;
    updatePlayPauseButton();
    previewPlayer->ResetCurrentMedia();
}

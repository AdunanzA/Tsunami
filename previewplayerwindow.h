#ifndef PREVIEWPLAYERWINDOW_H
#define PREVIEWPLAYERWINDOW_H

#include <QWidget>
#include "VLCQtWidgets/WidgetVideo.h"
#include "previewplayer.h"

namespace Ui {
class PreviewPlayerWindow;
}

class PreviewPlayerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewPlayerWindow(QWidget *parent = 0);
    ~PreviewPlayerWindow();

private slots:
    void on_pushButtonOpenFile_clicked();

    void on_pushButtonPlayPause_clicked();
    void mediaStopped();

private:
    Ui::PreviewPlayerWindow *ui;
    PreviewPlayer *previewPlayer;
    void updatePlayPauseButton();
};

#endif // PREVIEWPLAYERWINDOW_H

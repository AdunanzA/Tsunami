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

    void PauseForHidden();
protected:
    //void keyPressEvent(QKeyEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void on_pushButtonOpenFile_clicked();

    void on_pushButtonPlayPause_clicked();
    void mediaStopped();

    void on_pushButtonFullscreen_clicked();

private:
    Ui::PreviewPlayerWindow *ui;
    PreviewPlayer *previewPlayer;
    void updatePlayPauseButton();
};

#endif // PREVIEWPLAYERWINDOW_H

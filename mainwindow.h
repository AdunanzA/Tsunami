#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPointer>
#include <QtGui>
#include <QWidget>
#include <QScrollArea>
#include <QLabel>

#include "downloadwindow.h"
#include "searchwindow.h"
#include "settingswindow.h"
#include "statisticswindow.h"

#include "QScale/qscale.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    typedef std::shared_ptr<libtorrent::torrent_info> NativePtr;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPointer<QSystemTrayIcon> m_systrayIcon;

public slots:
    void updateStatusBar(const QString & msg);
    void updateGauge(const double & value);
    void popupInfo(const QString & msg);

private slots:
    void toggleVisibility(QSystemTrayIcon::ActivationReason e = QSystemTrayIcon::Trigger);
    void balloonClicked();
    void btnMenuClick();

    void btnAddClick();

//    void timerTick();

private:
    Ui::MainWindow *ui;
    settingswindow *settingsPage;
    searchwindow *searchPage;
    downloadwindow *downloadPage;
    statisticswindow *statisticsPage;
    QScrollArea *qsa;
    QLabel *statusLabel;

    QScale *p_scaleGauge;

//    int count = 0;
//    TsuProgress *tp;
//    QTimer *timer;

    void createTrayIcon();
    void initializeScreen();
};


#endif // MAINWINDOW_H

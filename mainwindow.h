#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPointer>
#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>
#include <QFileDialog>

#include "tsumanager.h"
#include "downloadwindow.h"
#include "searchwindow.h"
#include "settingswindow.h"
#include "statisticswindow.h"

#include "libtorrent/add_torrent_params.hpp"
#include "libtorrent/torrent_handle.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"

#include "QScale/qscale.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    tsuManager *sessionManager;
    QPointer<QSystemTrayIcon> m_systrayIcon;

signals:
    void stopSessionManager();

public slots:
    void updateStatusBar(const QString & msg);
    void updateGauge(const double & value);
    void popupInfo(const QString & msg);

private slots:
    void toggleVisibility(QSystemTrayIcon::ActivationReason e = QSystemTrayIcon::Trigger);
    void balloonClicked();

    void btnMenuClick();
    void btnAddClick();

private:
    Ui::MainWindow *ui;
    settingswindow *settingsPage;
    searchwindow *searchPage;
    downloadwindow *downloadPage;
    statisticswindow *statisticsPage;
    QLabel *statusLabel;

    QThread *p_session_thread;

    QScale *p_scaleGauge;

    void createTrayIcon();
    void initializeScreen();

    void readSettings();
    void writeSettings();
    void updateTsunami();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

};


#endif // MAINWINDOW_H

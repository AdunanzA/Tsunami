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
#include <QTranslator>

#include "previewplayerwindow.h"
#include "tsumanager.h"
#include "downloadwindow.h"
#include "searchwindow.h"
#include "settingswindow.h"
#include "statisticswindow.h"
#include "archivewindow.h"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_info.hpp>

#include <src/changelog.h>
#include <src/QcGaugeWidget/qcgaugewidget.h>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

    tsuManager *sessionManager;
    QPointer<QSystemTrayIcon> m_systrayIcon;

signals:
    void stopSessionManager();

public slots:
    void updateStatusBarStatistics(const QString & msg);
    void updateStatusBarLabel(const QString & msg);
    void popupInfo(const QString & msg);

    void externalIpAssigned();
    void dhtBootstrapExecuted();
    void listenerUpdate(QString type, bool success);

    void sessionStatisticUpdate(const quint64 &sent, const quint64 &received,
                                const quint64 &downloading, const quint64 &uploading,
                                const quint64 &checking, const quint64 &stopped,
                                const quint64 &error, const quint64 &queuedDown,
                                const quint64 &queuedSeed);

private slots:
    void toggleVisibility(QSystemTrayIcon::ActivationReason e = QSystemTrayIcon::Trigger);
    void balloonClicked();

    void btnMenuClick();
    void btnAddClick();
    void loadLanguage();

    void on_btnStatDown_toggled(bool checked);
    void on_btnStatUp_toggled(bool checked);
    void on_btnStatCheck_toggled(bool checked);
    void on_btnStatPause_toggled(bool checked);
    void on_btnStatError_toggled(bool checked);

    void fileDropped(QString fileName);
    void clipboardChanged();

    void downloadFromSearchPage(const QString magnet);

    void on_btnPatreon_released();

private:
    Ui::MainWindow *ui;
    settingswindow *settingsPage;
    searchwindow *searchPage;
    downloadwindow *downloadPage;
    statisticswindow *statisticsPage;
    PreviewPlayerWindow *previewPlayerPage;
    archivewindow *archivePage;

    QLabel *statusLabel;

    QTranslator p_translator;

    QThread *p_session_thread;

    QcGaugeWidget *p_gauge;
    QcDegreesItem *p_gaugeDegreesItem;
    QcValuesItem *p_gaugeValuesItem;
    QcNeedleItem *p_gaugeDownNeedle;
    QcNeedleItem *p_gaugeUpNeedle;
    QcLabelItem *p_gaugeUnitLabel;
    QcLabelItem *p_gaugeSpeedLabel;

    void createTrayIcon();
    void initializeScreen();

    void readSettings();
    void writeSettings();

    void updateGauge(const float &downValue, const float &upValue);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    // for some reason we need a little trick to adjust the window title on Mac and Linux
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    void showEvent( QShowEvent * event ) override;
#endif
};

#endif // MAINWINDOW_H

#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "searchwindow.h"
#include "downloadwindow.h"

#define TIME_TRAY_BALLOON 5000
#define PROJECT "Tsunami++"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settingsPage(new settingswindow),
    searchPage(new searchwindow),
    downloadPage(new downloadwindow),
    fakeList(new QVector<QWidget*>)
{
    ui->setupUi(this);
    settingsPage->hide();
    searchPage->hide();

    initializeScreen();
    statusBar()->showMessage("Welcome!");

    fakeList->append(settingsPage);
    fakeList->append(searchPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeScreen() {
    setWindowIcon(QIcon(":/icons/logo_tsunami_tiny.ico"));
    setWindowTitle(PROJECT " " VERSION);
    //setWindowFlags(Qt::FramelessWindowHint);
    createTrayIcon();

    connect(ui->btnSettings, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnUser, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnSearch, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnDownload, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnArchive, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnStreaming, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnStatistics, SIGNAL(released()), this, SLOT(btnMenuClick()));

    ui->btnSettings->show();
    ui->btnUser->show();
    ui->btnSearch->show();
    ui->btnDownload->show();
    ui->btnArchive->show();
    ui->btnStreaming->show();
    ui->btnStatistics->show();

    ui->btnSettings_on->hide();
    ui->btnUser_on->hide();
    ui->btnSearch_on->hide();
    ui->btnDownload_on->hide();
    ui->btnArchive_on->hide();
    ui->btnStreaming_on->hide();
    ui->btnStatistics_on->hide();

    ui->content->addWidget(settingsPage);
    ui->content->addWidget(searchPage);
    ui->content->addWidget(downloadPage);

    QWidget *defaultPage = new QWidget(this);
    defaultPage->setWindowOpacity(0.5);
    defaultPage->setStyleSheet("background-image: url(:/images/logo_tsunami_big.ico); background-repeat: no-repeat; background-position:center;");
    ui->content->addWidget(defaultPage);

}


void MainWindow::createTrayIcon()
{
    // Tray icon
    m_systrayIcon = new QSystemTrayIcon(QIcon(":/icons/logo_tsunami_tiny.ico"), this);

    //m_systrayIcon->setContextMenu(trayIconMenu());
    //connect(m_systrayIcon, SIGNAL(messageClicked()), this, SLOT(balloonClicked()));
    // End of Icon Menu
    connect(m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(toggleVisibility(QSystemTrayIcon::ActivationReason)));
    m_systrayIcon->setToolTip(PROJECT " " VERSION);
    m_systrayIcon->show();
    m_systrayIcon->showMessage(PROJECT, "Welcome to the future!", QSystemTrayIcon::Information, TIME_TRAY_BALLOON);
    qDebug("started");
}

// Toggle Main window visibility
void MainWindow::toggleVisibility(QSystemTrayIcon::ActivationReason e)
{
    qDebug("entered toggleVisibility");
    if ((e == QSystemTrayIcon::Trigger) || (e == QSystemTrayIcon::DoubleClick)) {
        if (isHidden()) {
            /*if (m_uiLocked) {
                // Ask for UI lock password
                if (!unlockUI())
                    return;
            }*/
            // Make sure the window is not minimized
            setWindowState(windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
            // Then show it
            show();
            raise();
            activateWindow();
        }
        else {
            hide();
        }
    }
}

void MainWindow::btnMenuClick() {
    QString btn = sender()->objectName();

    ui->btnSettings->setHidden(btn == ui->btnSettings->objectName());
    ui->btnUser->setHidden(btn == ui->btnUser->objectName());
    ui->btnSearch->setHidden(btn == ui->btnSearch->objectName());
    ui->btnDownload->setHidden(btn == ui->btnDownload->objectName());
    ui->btnArchive->setHidden(btn == ui->btnArchive->objectName());
    ui->btnStreaming->setHidden(btn == ui->btnStreaming->objectName());
    ui->btnStatistics->setHidden(btn == ui->btnStatistics->objectName());

    ui->btnSettings_on->setHidden(btn != ui->btnSettings->objectName());
    ui->btnUser_on->setHidden(btn != ui->btnUser->objectName());
    ui->btnSearch_on->setHidden(btn != ui->btnSearch->objectName());
    ui->btnDownload_on->setHidden(btn != ui->btnDownload->objectName());
    ui->btnArchive_on->setHidden(btn != ui->btnArchive->objectName());
    ui->btnStreaming_on->setHidden(btn != ui->btnStreaming->objectName());
    ui->btnStatistics_on->setHidden(btn != ui->btnStatistics->objectName());

    settingsPage->setHidden(btn != ui->btnSettings->objectName());
    searchPage->setHidden(btn != ui->btnSearch->objectName());
    downloadPage->setHidden(btn != ui->btnDownload->objectName());
}

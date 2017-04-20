#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPointer>
#include <QtGui>
#include <QWidget>
#include <QScrollArea>

#include "downloadwindow.h"
#include "searchwindow.h"
#include "settingswindow.h"

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

    QPointer<QSystemTrayIcon> m_systrayIcon;

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
    QScrollArea *qsa;

    int count;

    void createTrayIcon();
    void initializeScreen();
};


#endif // MAINWINDOW_H

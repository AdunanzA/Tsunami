#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPointer>
#include <QtGui>
#include <QWidget>

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
    void btnMenuClick();

private:
    Ui::MainWindow *ui;
    QWidget *settingsPage;
    QWidget *searchPage;
    QWidget *downloadPage;
    QVector<QWidget*> *fakeList;

    void createTrayIcon();
    void initializeScreen();
};


#endif // MAINWINDOW_H

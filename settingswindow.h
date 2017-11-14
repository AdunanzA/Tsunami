#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>

#include <QApplication>
#include <QProcess>

#include <changelog.h>

namespace Ui {
class settingswindow;
}

class settingswindow : public QWidget
{
    Q_OBJECT

public:
    explicit settingswindow(QWidget *parent = 0);
    ~settingswindow();

    void loadSettings();
    void saveSettings();

    void setLastBrowsedDir(const QString &path);
    QString getLastBrowsedDir();

    static QString settingsFileName;
    static QSettings::Format settingsFileFormat;

    QString getDownloadPath() const;

    int getDebugLevelIndex() const;
    int getCurrentLanguageIndex() const;

signals:
    void sendMessageToStatusBar(const QString & msg);
    void sendRefreshSettings();

private slots:
    void on_btnSave_released();
    void on_btnCancel_released();
    void on_btnFolder_released();

    void on_commandLinkButton_released();

    void on_pushButton_released();

private:
    Ui::settingswindow *ui;

    QSettings settings;

    void changeEvent(QEvent *e);

//    bool needRestart = false;
};

#endif // SETTINGSWINDOW_H

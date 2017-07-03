#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QFileDialog>

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

    const QString settingsFileName = "tsunami.ini";
    const QSettings::Format settingsFileFormat = QSettings::IniFormat;

    QString getDownloadPath() const;
    void setDownloadPath(const QString &value);

private slots:
    void on_btnSave_released();
    void on_btnCancel_released();

private:
    Ui::settingswindow *ui;
    QString p_downloadPath;

    bool eventFilter(QObject* object, QEvent* event);
};

#endif // SETTINGSWINDOW_H

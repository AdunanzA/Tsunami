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

    static QString settingsFileName;
    static QSettings::Format settingsFileFormat;

    QString getDownloadPath() const;
    void setDownloadPath(const QString &value);

    QString getDebugLevel() const;
    void setDebugLevel(const QString &value);

    QString getCurrentLanguage() const;

private slots:
    void on_btnSave_released();
    void on_btnCancel_released();
    void setCurrentLanguage(int value);

private:
    Ui::settingswindow *ui;
    QString p_downloadPath;
    QString p_debugLevel;
    QString p_appLanguage;

    bool eventFilter(QObject* object, QEvent* event);
    void changeEvent(QEvent *e);
};

#endif // SETTINGSWINDOW_H

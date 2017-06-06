#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>

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

private:
    Ui::settingswindow *ui;
    QString p_settingsFile;
};

#endif // SETTINGSWINDOW_H

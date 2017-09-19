#ifndef CHANGELOG_H
#define CHANGELOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

namespace Ui {
class changelog;
}

class changelog : public QDialog
{
    Q_OBJECT

public:
    explicit changelog(QWidget *parent = 0);
    ~changelog();

private slots:
    void on_btnClose_released();

private:
    Ui::changelog *ui;
    void onCmbChange(const QString &selectedText);
};

#endif // CHANGELOG_H

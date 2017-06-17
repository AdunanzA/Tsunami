#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QGraphicsScene>
#include <QDateTime>

#include "tsumanager.h"
#include "tsuitem.h"

#include <cmath> // floor

namespace Ui {
    class downloadwindow;
}

class downloadwindow : public QWidget
{
    Q_OBJECT

public:
    explicit downloadwindow(QWidget *parent = 0);
    ~downloadwindow();

public slots:
    void updateFromSession(const QVector<tsuEvents::tsuEvent> &events);
    void torrentDeletedFromSession(const std::string &hash);

private:
    void redrawItemsPosition();

    Ui::downloadwindow *ui;
    QGraphicsScene *p_scene;
    QVector<tsuItem *> p_tsulist;
    int p_downRate = 0;
    int p_upRate = 0;
    int p_totalDownload = 0;
    int p_totalUpload = 0;
    QDateTime p_lastStatisticsSent = QDateTime::currentDateTime();

    int p_itemsPerRow = 0;

    QTimer *p_sessionStatisticTimer;

private slots:
    void requestedCancel(const std::string &hash, const bool &deleteFilesToo);
    void requestedPause(const std::string &hash);
    void requestedResume(const std::string &hash);
    void updateSessionStatistics();

signals:
    void sendUpdateToStatusBar(const QString &msg);
    void sendUpdateGauge(const double &value);
    void sendStatisticsUpdate(const QPair<int, int> &values);
    void sendPopupInfo(const QString &msg);
    void sendRequestedCancelToSession(const std::string &hash, const bool &deleteFilesToo);
    void sendRequestedPauseToSession(const std::string &hash);
    void sendRequestedResumeToSession(const std::string &hash);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // DOWNLOADWINDOW_H

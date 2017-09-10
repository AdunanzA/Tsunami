#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QGraphicsScene>
#include <QDateTime>
#include <qmath.h> // qFabs

#include "tsumanager.h"
#include "tsuitem.h"
#include "itemdetails.h"
#include "tsugraphicsscene.h"

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
    void addFromSession(const tsuEvents::tsuEvent &event);
    void torrentDeletedFromSession(const std::string &hash);

private:
    void redrawItemsPosition();

    Ui::downloadwindow *ui;
    tsugraphicsscene *p_scene;
    QVector<tsuItem *> p_tsulist;
    int p_downRate = 0;
    int p_upRate = 0;
    int p_totalDownload = 0;
    int p_totalUpload = 0;
//    QDateTime p_lastStatisticsSent = QDateTime::currentDateTime();

    int p_itemsPerRow = 0;

    QTimer *p_sessionStatisticTimer;

    itemDetails *p_itemDetails;

    void changeEvent(QEvent *e);

private slots:
    void requestedCancel(const std::string &hash, const bool &deleteFilesToo);
    void requestedPause(const std::string &hash);
    void requestedResume(const std::string &hash);
    void requestedDetails(const std::string &hash);
    void showContextMenu(const QPoint &pos);
    void selectAll();
    void deselectAll();
    void pauseSelected();
    void resumeSelected();
    void deleteSelected();
    void updateSessionStatistics();

signals:
    void sendUpdateToStatusBar(const QString &msg);
    void sendUpdateGauge(const double &value);
    void sendStatisticsUpdate(const QPair<int, int> &values);
    void sendPopupInfo(const QString &msg);
    void sendMessageToStatusBar(const QString & msg);
    void sendRequestedCancelToSession(const std::string &hash, const bool &deleteFilesToo);
    void sendRequestedPauseToSession(const std::string &hash);
    void sendRequestedResumeToSession(const std::string &hash);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // DOWNLOADWINDOW_H

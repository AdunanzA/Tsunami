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

    QPair<float, float> getRate();
    void updateVisibility();

public slots:
    void updateFromSession(const QVector<tsuEvents::tsuEvent> &events);
    void addFromSession(const tsuEvents::tsuEvent &event);
    void torrentDeletedFromSession(const std::string &hash);

private:
    Ui::downloadwindow *ui;
    tsugraphicsscene *p_scene;
    QVector<tsuItem *> p_tsulist;

    int p_itemsPerRow = 0;
    qreal p_transformFactor = 1;
    itemDetails *p_itemDetails;

    QMenu *p_contextMenu;

    void redrawItemsPosition();
    void changeEvent(QEvent *e);

private slots:
    void requestedCancel(const std::string &hash, const bool &deleteFilesToo);
    void requestedPause(const std::string &hash);
    void requestedResume(const std::string &hash);
    void requestedDetails(const std::string &hash);
    void downloadFinished(tsuItem *item);

    void showContextMenu(const QPoint &pos);
    void selectAll();
    void deselectAll();
    void pauseSelected();
    void resumeSelected();
    void deleteSelected();

    void on_btnZoomIn_released();
    void on_btnZoomOut_released();

    void fileDroppedFromGraphicScene(QString fileName);

signals:
    void sendPopupInfo(const QString &msg);
    void sendMessageToStatusBar(const QString & msg);
    void sendRequestedCancelToSession(const std::string &hash, const bool &deleteFilesToo);
    void sendRequestedPauseToSession(const std::string &hash);
    void sendRequestedResumeToSession(const std::string &hash);

    void fileDropped(QString fileName);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

};

#endif // DOWNLOADWINDOW_H

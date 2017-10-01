#ifndef TSUITEM_H
#define TSUITEM_H

#include <QDebug>
#include <QColor>
#include <QtWidgets>
#include <QGraphicsItem>
#include <QMessageBox>
#include <qmath.h>

#include "tsuevents.h"

#include <cmath>       /* fmod */

enum statusEnum {
    undefined,
    checking_files,
    downloading_metadata,
    downloading,
    finished,
    seeding,
    allocating,
    checking_resume_data,
    paused
};

class tsuItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    tsuItem();
    tsuItem(const std::string &head);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void set_FactorTransform(const qreal & value);
    qreal get_FactorTransform();
    QString get_Head();

    void set_Size(const int & value);
    void set_Downloaded(const int & value);
    void set_Uploaded(const int & value);
    int get_Downloaded();
    int get_Uploaded();

    statusEnum get_Status() const;
    void set_Status(const int &value);

    int get_RateDownload() const;
    void set_RateDownload(const int &value);

    int get_RateUpload() const;
    void set_RateUpload(const int &value);

    bool get_Visibility() const;
    void set_Visibility(const bool &visible);

    static const qreal ItemWidth;
    static const qreal ItemHeight;
    static const qreal ItemGlowRadius;

    std::string get_Hash() const;
    void set_Hash(const std::string &value);

    void executeItemRemove(bool const &filesToo);
    void executePause();
    void executeResume();

    QString convertSize(const int &size);
    QString convertSizeUnit(const int &size);


public slots:
    void setValue(const tsuEvents::tsuEvent &event);
    void emitCancel();

private:
    int p_progressValue = 0;
    QString p_head = QString("");
    qreal p_factorTransform = 1;
    bool p_isFadingOut = false;
    bool p_isFadingIn = false;

    bool p_isMoving = false;

    QPixmap p_imageBkg = QPixmap(":/images/tsuitem/bkg.png");
    QPixmap p_imageBkg2 = QPixmap(":/images/tsuitem/bkg2.png");
    QPointF p_imageBkg2Position = QPointF(1, 158);
    QPixmap p_imageExit = QPixmap(":/images/tsuitem/X.png");
    QPointF p_imageExitPosition = QPointF(122, 4);
    QPixmap p_imagePause = QPixmap(":/images/tsuitem/pause.png");
    QPointF p_imagePausePosition = QPointF(31, 167);
    QPixmap p_imageResume = QPixmap(":/images/tsuitem/resume.png");
    QPointF p_imageResumePosition = QPointF(46, 167);

    QPixmap p_imageStateD = QPixmap(":/images/tsuitem/state_download.png");
    QPixmap p_imageStateL = QPixmap(":/images/tsuitem/state_loading.png");
    QPixmap p_imageStateP = QPixmap(":/images/tsuitem/state_pause.png");
    QPixmap p_imageStateW = QPixmap(":/images/tsuitem/state_warning.png");
    QPixmap p_imageStateF = QPixmap(":/images/tsuitem/finished.png");
    QPixmap p_imageSelected = QPixmap(":/images/tsuitem/selected.png");

    QPointF p_imageStatePosition = QPointF(1, 158);
    QPointF p_imageStateFinishedPosition = QPointF(16, 140);
    QPointF p_imageSelectedPosition = QPointF(99, 148);

    int p_textFontHeight = 12;
    int p_symbolFontHeight = 7;
    int p_progress_width = 10;
    qreal p_shadowDepth = 1.5;
    qreal p_fontBorder = 0.5;
    QColor p_colorText = QColor(255, 255, 255);
    QColor p_colorShadow = QColor(58, 58, 58);
    QFont p_fontText = QFont("Bebas", p_textFontHeight);
    QFont p_fontSymbol = QFont("Bebas", p_symbolFontHeight);
    QFont p_indicatorFont = QFont("Tahoma", 8, QFont::Thin);
    QFont p_indicatorUnitFont = QFont("Tahoma", 6, QFont::Thin);

    QRectF p_rectProgress = QRectF(45.5, 69, 49.0, 49.0);
    QPen p_penProgress;
    QConicalGradient p_gradientProgress;
    int p_arcLengthApproximation = p_progress_width + p_progress_width / 3;

    QColor p_colorLabel = QColor(127,127,127);
    QFont p_fontLabel = QFont("Tahoma", 7.5);

    QGraphicsDropShadowEffect *p_itemShadow;
    QPropertyAnimation *p_itemShadowAnimation;

    std::string p_hash;
    int p_size = 0;
    int p_downloaded = 0;
    int p_uploaded = 0;
    int p_rateDownload = 0;
    int p_rateUpload = 0;
    int p_numSeeds = 0;
    int p_numPeers = 0;
    statusEnum p_status = statusEnum::undefined;
    QDateTime p_eta;

    bool p_cancelFilesOnDelete = false;

    bool p_visible = true;

private slots:
    void fadeInFinished();
    void fadeOutFinished();

protected:
    void createItem();
    QString remainingTime();
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void cancelRequested(const std::string &hash, const bool &canelFiles);
    void pauseRequested(const std::string &hash);
    void resumeRequested(const std::string &hash);
    void detailsRequested(const std::string &hash);
    void downloadFinished(tsuItem *item);

};

#endif // TSUITEM_H

#ifndef TSUITEM_H
#define TSUITEM_H

#include <cmath>       // needed for fmod

#include <QDebug>
#include <QColor>
#include <QtWidgets>
#include <QGraphicsItem>
#include <QMessageBox>
#include <qmath.h>

#include "tsuevents.h"

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


class tsuItem : public QGraphicsObject // QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    tsuItem();
    tsuItem(const std::string &head);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void set_FactorTransform(const qreal & value);
    qreal get_FactorTransform() const;
    const QString& get_Head() const;

    void set_Size(const uint64_t & value);
    void set_Downloaded(const uint64_t & value);
    void set_Uploaded(const uint64_t & value);
    uint64_t get_Downloaded() const;
    uint64_t get_Uploaded() const;

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

    const std::string& get_Hash() const;
    void set_Hash(const std::string &value);

    void executeItemRemove(bool const &filesToo);
    void executePause();
    void executeResume();

public slots:
    void setValue(const tsuEvents::tsuEvent &event);
    void emitCancel();

private:
    int p_progressValue {};
    QString p_head;
    qreal p_factorTransform {1};
    bool p_isFadingOut {};
    bool p_isFadingIn {};

    bool p_isMoving {};

    QPixmap p_imageBkg {":/images/tsuitem/bkg.png"};
    QPixmap p_imageBkg2 {":/images/tsuitem/bkg2.png"};
    QPointF p_imageBkg2Position {1, 158};
    QPixmap p_imageExit {":/images/tsuitem/X.png"};
    QPointF p_imageExitPosition = QPointF(122, 4);
    QPixmap p_imagePause = QPixmap(":/images/tsuitem/pause.png");
    QPointF p_imagePausePosition = QPointF(31, 167);
    QPixmap p_imageResume = QPixmap(":/images/tsuitem/resume.png");
    QPointF p_imageResumePosition = QPointF(46, 167);

    QPixmap p_imageStateD {":/images/tsuitem/state_download.png"};
    QPixmap p_imageStateL {":/images/tsuitem/state_loading.png"};
    QPixmap p_imageStateP {":/images/tsuitem/state_pause.png"};
    QPixmap p_imageStateW {":/images/tsuitem/state_warning.png"};
    QPixmap p_imageStateF {":/images/tsuitem/finished.png"};
    QPixmap p_imageSelected {":/images/tsuitem/selected.png"};

    QPointF p_imageStatePosition {1, 158};
    QPointF p_imageStateFinishedPosition {16, 140};
    QPointF p_imageSelectedPosition {99, 148};

    int p_textFontHeight {12};
    int p_symbolFontHeight {7};
    int p_progress_width {10};
    qreal p_shadowDepth {1.5};
    qreal p_fontBorder {0.5};
    QColor p_colorText {255, 255, 255};
    QColor p_colorShadow {58, 58, 58};
    QFont p_fontText {"Bebas", p_textFontHeight};
    QFont p_fontSymbol {"Bebas", p_symbolFontHeight};
    QFont p_indicatorFont {"Tahoma", 8, QFont::Thin};
    QFont p_indicatorUnitFont {"Tahoma", 6, QFont::Thin};

    QRectF p_rectProgress {45.5, 69, 49.0, 49.0};
    QPen p_penProgress;
    QConicalGradient p_gradientProgress;
    int p_arcLengthApproximation {p_progress_width + p_progress_width / 3};

    QColor p_colorLabel {127,127,127};
    QFont p_fontLabel {"Tahoma", 7};

    QGraphicsDropShadowEffect *p_itemShadow{};
    QPropertyAnimation *p_itemShadowAnimation{};

    std::string p_hash;
    uint64_t p_size {};
    uint64_t p_downloaded{};
    uint64_t p_uploaded{};
    int p_rateDownload{};
    int p_rateUpload{};
    int p_numSeeds{};
    int p_numPeers{};
    statusEnum p_status {statusEnum::undefined};
    QDateTime p_eta;

    bool p_cancelFilesOnDelete {};

    bool p_visible {true};

private slots:
    void fadeInFinished();
    void fadeOutFinished();

protected:
    void createItem();
    QString remainingTime() const;
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

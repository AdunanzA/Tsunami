#include "tsuitem.h"

const qreal tsuItem::ItemWidth = 140;
const qreal tsuItem::ItemHeight = 190;
const qreal tsuItem::ItemGlowRadius = 20;

tsuItem::tsuItem()
{
    createItem();
}

tsuItem::tsuItem(const std::string &hash)
{
    p_hash = hash;
    createItem();
}

void tsuItem::createItem()
{
//    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    // fadeIn effect
    p_isFadingIn = true;
    QGraphicsOpacityEffect *fadeInEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(fadeInEffect);

    QPropertyAnimation *fadeInAnimation = new QPropertyAnimation(fadeInEffect, "opacity");
    fadeInAnimation->setDuration(500);
    fadeInAnimation->setStartValue(0);
    fadeInAnimation->setEndValue(1);
    fadeInAnimation->setEasingCurve(QEasingCurve::InBack);
    fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    connect(fadeInAnimation,SIGNAL(finished()),this,SLOT(fadeInFinished()));

    p_gradientProgress.setCenter(p_rectProgress.center());
    p_gradientProgress.setAngle(90);

    // will be populated with first date during fast resume
    p_eta = QDateTime::currentDateTime();

}

QRectF tsuItem::boundingRect() const
{
    return QRectF(0 - tsuItem::ItemGlowRadius,
                  0 - tsuItem::ItemGlowRadius,
                  tsuItem::ItemWidth + tsuItem::ItemGlowRadius,
                  tsuItem::ItemHeight + tsuItem::ItemGlowRadius);
}

void tsuItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // http://thesmithfam.org/blog/2007/02/03/qt-improving-qgraphicsview-performance/
    painter->setClipRect( option->exposedRect );

//    painter->setRenderHint(QPainter::Antialiasing);
//    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QString text = QString::number(p_progressValue);
    QString symbol("%");
    double progress = (double)p_progressValue/100;

    // BACKGROUND IMAGE
    painter->drawPixmap(0, 0, tsuItem::ItemWidth, tsuItem::ItemHeight, p_imageBkg);

    // SELECTED
    if (!p_isFadingOut && !p_isFadingIn && !p_isMoving && this->isSelected() ) {
        painter->drawPixmap(p_imageSelectedPosition, p_imageSelected);
    } else {
    }

    // CANCEL IMAGE
    painter->drawPixmap(p_imageExitPosition, p_imageExit);

    if (this->p_status != statusEnum::finished) {
        painter->drawPixmap(p_imageBkg2Position, p_imageBkg2);
        painter->drawPixmap(p_imagePausePosition, p_imagePause);
        painter->drawPixmap(p_imageResumePosition, p_imageResume);
    }

    switch (p_status) {
    case undefined:
    case checking_files:
    case checking_resume_data:
        painter->drawPixmap(p_imageStatePosition, p_imageStateW);
        break;
    case downloading_metadata:
    case downloading:
        painter->drawPixmap(p_imageStatePosition, p_imageStateD);
        break;
    case seeding:
    case allocating:
        painter->drawPixmap(p_imageStatePosition, p_imageStateL);
        break;
    case paused:
        painter->drawPixmap(p_imageStatePosition, p_imageStateP);
        break;
    case finished:
        painter->drawPixmap(p_imageStateFinishedPosition, p_imageStateF);
//        painter->drawPixmap(p_imageStatePosition, p_imageStateL);
        break;
    default:
        break;
    }

    QPen pen;

    painter->setFont(p_fontText);
    int lenText = painter->fontMetrics().width(text);
    painter->setFont(p_fontSymbol);
    int lenSymb = painter->fontMetrics().width(symbol);

    qreal textX = (tsuItem::ItemWidth/2)-((lenText+lenSymb)/2);
    qreal textY = (tsuItem::ItemHeight/2)+(p_textFontHeight/2);

    // CENTRAL PERCENTAGE TEXT SHADOW
    QPainterPath pathShadow;
    painter->setPen(p_colorShadow);
    painter->setBrush(p_colorShadow);
    pathShadow.addText(textX+p_shadowDepth, textY+p_shadowDepth, p_fontText, text);
    pathShadow.addText(textX+lenText+p_shadowDepth, textY+p_shadowDepth, p_fontSymbol, symbol);
    painter->drawPath(pathShadow);

    int red;
    int green;
    int blu;

    if (p_progressValue < 50) {
        red   = 255;
        green = 255 - (90  * p_progressValue / 50);
        blu   = 255 - (255 * p_progressValue / 50);
    } else if (p_progressValue == 50) {
        red   = 255;
        green = 165;
        blu   = 0;
    } else {
        red   = 255 - (248 * ( p_progressValue - 50 ) / 50);
        green = 165 + (87  * ( p_progressValue - 50 ) / 50);
        blu   = 0   + (18  * ( p_progressValue - 50 ) / 50);
    }
//    qDebug() << QString("%1 -> r %2, g %3, b %4").arg(p_value).arg(red).arg(green).arg(blu);

    // CENTRAL PERCENTAGE TEXT
    QPainterPath pathText;
    pen.setColor(QColor(red, green, blu).darker(200));
    pen.setWidthF(p_fontBorder);
    painter->setBrush(QColor(red, green, blu));
    painter->setPen(pen);
    pathText.addText(textX, textY, p_fontText, text);
    pathText.addText(textX+lenText, textY, p_fontSymbol, symbol);
    painter->drawPath(pathText);

    // HEAD
    QPainterPath pathTitle;
    pen.setColor(p_colorText);
    pen.setWidthF(p_fontBorder/2);
    painter->setBrush(p_colorText);
    painter->setPen(pen);
    pathTitle.addText(8, 16, QFont("Tahoma", 8), p_head);
    painter->drawPath(pathTitle);

    // CIRCULAR PROGRESS BAR
    if ( p_progressValue < 100 ) {
        p_gradientProgress.setColorAt(0, QColor(7, 252, 18, 200));
        p_gradientProgress.setColorAt(0.5, QColor(255, 165, 0, 200));
        p_gradientProgress.setColorAt(1, QColor(252, 0, 0, 200));
        p_penProgress = QPen(QBrush(p_gradientProgress), p_progress_width);
    } else {
        p_penProgress = QPen(QColor(red, green, blu, 150), p_progress_width);
    }
    p_penProgress.setCapStyle(Qt::FlatCap);
    painter->setPen(p_penProgress);
    painter->drawArc(p_rectProgress, 90 * 16 - p_arcLengthApproximation, -(progress) * 360 * 16);

    // FILE INFO
    // Label
    painter->setFont(p_fontLabel);
    int lenTot = painter->fontMetrics().width(tr("Total"));
    int lenDow = painter->fontMetrics().width(tr("Downloaded"));
    int lenRem = painter->fontMetrics().width(tr("Remaining"));

    QPainterPath pathLabels;
    pen.setColor(Qt::transparent);
    pen.setWidthF(0.5);
    painter->setBrush(p_colorLabel);
    painter->setPen(pen);
    pathLabels.addText(100 - lenTot, 34, p_fontLabel, tr("Total"));
    pathLabels.addText(100 - lenDow, 46, p_fontLabel, tr("Downloaded"));
    pathLabels.addText(100 - lenRem, 58, p_fontLabel, tr("Remaining"));
    painter->drawPath(pathLabels);

    // Indicators
    QString sSize = convertSize(p_size);
    QString sDown = convertSize(p_downloaded);
    QString sRema = convertSize((p_size - p_downloaded));

    painter->setFont(p_indicatorFont);
    int lenSize = painter->fontMetrics().width(sSize);
    int lenDown = painter->fontMetrics().width(sDown);
    int lenRema = painter->fontMetrics().width(sRema);

    QPainterPath pathGreenIndicators;
    painter->setBrush(QColor(7,252,18));
    pathGreenIndicators.addText(124-lenSize, 34, p_indicatorFont, sSize);
    pathGreenIndicators.addText(126, 34, p_indicatorUnitFont, convertSizeUnit(p_size));
    painter->drawPath(pathGreenIndicators);

    QPainterPath pathOrangeIndicators;
    painter->setBrush(QColor(255,196,0));
    pathOrangeIndicators.addText(124-lenDown, 46, p_indicatorFont, sDown);
    pathOrangeIndicators.addText(126, 46, p_indicatorUnitFont, convertSizeUnit(p_downloaded));
    painter->drawPath(pathOrangeIndicators);

    QPainterPath pathBluIndicators;
    painter->setBrush(QColor(0,144,255));
    pathBluIndicators.addText(124-lenRema, 58, p_indicatorFont, sRema);
    pathBluIndicators.addText(126, 58, p_indicatorUnitFont, convertSizeUnit((p_size - p_downloaded)));
    painter->drawPath(pathBluIndicators);

    // LOAD INDICATORS
    QFont loadUnitFont = QFont("Tahoma", 8, QFont::Thin);
    QPainterPath loadUnitIndicators;
    painter->setBrush(QColor(0,144,255));
    painter->setFont(p_indicatorFont);
    int ruuSize = painter->fontMetrics().width(convertSizeUnit(p_rateUpload));
    int rduSize = painter->fontMetrics().width(convertSizeUnit(p_rateDownload));
    loadUnitIndicators.addText(10-(ruuSize/2), 90, loadUnitFont, convertSizeUnit(p_rateUpload));
    loadUnitIndicators.addText(127-(rduSize/2), 90, loadUnitFont, convertSizeUnit(p_rateDownload));
    painter->drawPath(loadUnitIndicators);

    QFont loadFont = QFont("Tahoma", 8, QFont::Thin);
    QPainterPath loadIndicatorUp;
    QPainterPath loadIndicatorDown;
    painter->setFont(loadFont);
    int ruSize = painter->fontMetrics().width(convertSize(p_rateUpload));
    int rdSize = painter->fontMetrics().width(convertSize(p_rateDownload));
    painter->setBrush(QColor(131,131,131));
    loadIndicatorUp.addText(18-(ruSize/2), 103, loadFont, convertSize(p_rateUpload));
    painter->drawPath(loadIndicatorUp);
    painter->setBrush(QColor(255,144,0));
    loadIndicatorDown.addText(120-(rdSize/2), 103, loadFont, convertSize(p_rateDownload));
    painter->drawPath(loadIndicatorDown);

    if (this->p_status != statusEnum::finished) {
        // REMAINING TIME
        QString remaining = remainingTime();
        QPainterPath pathReminingTime;
        QFont remainingFont = QFont("Tahoma", 8, QFont::DemiBold);
        painter->setFont(remainingFont);
        painter->setBrush(QColor(255,255,255));
        int remSize = painter->fontMetrics().width(remaining);
        pathReminingTime.addText(70-(remSize/2), 148, remainingFont, remaining);
        painter->drawPath(pathReminingTime);

        // ETA
        QPainterPath pathEta;
        QFont etaFont = QFont("Tahoma", 7, QFont::Thin);
        painter->setFont(etaFont);
        painter->setBrush(QColor(131, 131, 131));
        int etaSize = painter->fontMetrics().width(p_eta.toString("dd/MM/yyyy hh:mm"));
        pathEta.addText(70-(etaSize/2), 158, etaFont, p_eta.toString("dd/MM/yyyy hh:mm"));
        painter->drawPath(pathEta);
    }

}

void tsuItem::set_FactorTransform(const qreal &value)
{
    p_factorTransform = value;
    this->setScale(value);
    update();
}

qreal tsuItem::get_FactorTransform()
{
    return p_factorTransform;
}

QString tsuItem::get_Head()
{
    return p_head;
}

void tsuItem::set_Size(const int &value)
{
    p_size = value;
}

void tsuItem::set_Downloaded(const int &value)
{
    p_downloaded = value;
}

void tsuItem::set_Uploaded(const int &value)
{
    p_uploaded = value;
}

int tsuItem::get_Downloaded()
{
    return p_downloaded;
}

int tsuItem::get_Uploaded()
{
    return p_uploaded;
}

void tsuItem::setValue(const tsuEvents::tsuEvent &event)
{
//    qDebug() << QString("setValue event.percentage %1").arg(event.percentage);
    p_progressValue = event.percentage/10000;
    p_downloaded = (event.downloaded != 0) ? qFabs(event.downloaded) : 0;
    p_rateDownload = (event.downloadRate != 0) ? qFabs(event.downloadRate) : 0;
    p_head = event.name;
    p_size = (event.total_size != 0) ? qFabs(event.total_size) : 0;
    p_uploaded = (event.uploaded != 0) ? qFabs(event.uploaded) : 0;
    p_rateUpload = (event.uploadRate != 0) ? qFabs(event.uploadRate) : 0;
    set_Status(event.state);
    update();
}

void tsuItem::emitCancel()
{
    emit cancelRequested(p_hash, p_cancelFilesOnDelete);
}

std::string tsuItem::get_Hash() const
{
    return p_hash;
}

void tsuItem::set_Hash(const std::string &value)
{
    p_hash = value;
}

void tsuItem::executeItemRemove(const bool &filesToo)
{
    p_cancelFilesOnDelete = filesToo;
    p_isFadingOut = true;
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(500);
    a->setStartValue(1);
    a->setEndValue(0.5);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a,SIGNAL(finished()),this,SLOT(emitCancel()));
}

void tsuItem::executePause()
{
    if (p_status != statusEnum::finished) {
        emit pauseRequested(this->p_hash);
        update();
    }
}

void tsuItem::executeResume()
{
    if (p_status != statusEnum::finished) {
        emit resumeRequested(this->p_hash);
        update();
    }
}

int tsuItem::get_RateUpload() const
{
    return p_rateUpload;
}

void tsuItem::set_RateUpload(int value)
{
    p_rateUpload = value;
}

int tsuItem::get_RateDownload() const
{
    return p_rateDownload;
}

void tsuItem::set_RateDownload(int value)
{
    p_rateDownload = value;
}

statusEnum tsuItem::get_Status() const
{
    return p_status;
}

void tsuItem::set_Status(const int &value)
{
    p_status = static_cast<statusEnum>(value);
}

void tsuItem::fadeInFinished()
{
    // set glow (activated on mouse over) after fadeIn finished
    p_itemShadow = new QGraphicsDropShadowEffect(this);
    p_itemShadow->setBlurRadius(tsuItem::ItemGlowRadius);
    p_itemShadow->setOffset(0,0);
    p_itemShadow->setColor(QColor(0, 0, 0));
    this->setGraphicsEffect(p_itemShadow);

//    p_itemShadowAnimation = new QPropertyAnimation(p_itemShadow, "blurRadius");
//    p_itemShadowAnimation->setDuration(300);
//    p_itemShadowAnimation->setStartValue(0);
//    p_itemShadowAnimation->setEndValue(20);
    p_itemShadowAnimation = new QPropertyAnimation(p_itemShadow, "color");
    p_itemShadowAnimation->setDuration(250);
    p_itemShadowAnimation->setStartValue(QColor(0, 0, 0));
    p_itemShadowAnimation->setEndValue(QColor(255, 162, 0));

    p_isFadingIn = false;
}

void tsuItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // bypass this event to let mouseReleaseEvent choose
    Q_UNUSED(event);
//    QGraphicsItem::mousePressEvent(event);
}

void tsuItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // shift + mouse click and move
//    if (event->modifiers() & Qt::ShiftModifier) {
//        update();
//        return;
//    }
//    p_isMoving = true;
//    setOpacity(0.8);
    QGraphicsItem::mouseMoveEvent(event);
}

void tsuItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << QString("release y %0").arg(QString::number(event->lastScenePos().y()));
//    p_isMoving = false;
//    if (event->lastScenePos().x() < 0) this->setX(0);
//    if (event->lastScenePos().y() < 0) this->setY(0);
//    setOpacity(1);

    if (p_isFadingOut || p_isFadingIn) return;

    int x = event->lastPos().x();
    int y = event->lastPos().y();

    if ( (x >= p_imageExitPosition.x() && x <= (p_imageExitPosition.x() + p_imageExit.width()) ) &&
         (y >= p_imageExitPosition.y() && y <= (p_imageExitPosition.y() + p_imageExit.height()) ) ) {

        // CANCEL BUTTON
        QMessageBox mbox;
        QString msg = QString("<center>Do you really want to cancel<br/><b>%0</b><br/>from download?</center>").arg(this->get_Head());
        mbox.setText(msg);
        mbox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );

        mbox.addButton("Yes and delete files too", QMessageBox::YesRole);

        bool proceedWithCancel = false;

        switch (mbox.exec()) {
            case QMessageBox::Yes:
                proceedWithCancel = true;
                break;
            case QMessageBox::No:
                proceedWithCancel = false;
                break;
            default:
                proceedWithCancel = true;
                p_cancelFilesOnDelete = true;
                break;
        }

        if (proceedWithCancel) {
            executeItemRemove(p_cancelFilesOnDelete);
        }

    } else  if ( (x >= 8 && x <= 120) && (y >= 6 && y <= 17) ) {

        // TSUCARD TITLE
        emit detailsRequested(this->get_Hash());

    } else if ( (x >= p_imagePausePosition.x() && x <= (p_imagePausePosition.x() + p_imagePause.width())) &&
                (y >= p_imagePausePosition.y() && y <= (p_imagePausePosition.y() + p_imagePause.height()) ) ) {

        // PAUSE BUTTON
        executePause();

    } else if ( (x >= p_imageResumePosition.x() && x <= (p_imageResumePosition.x() + p_imageResume.width())) &&
                (y >= p_imageResumePosition.y() && y <= (p_imageResumePosition.y() + p_imageResume.height()) ) ) {

        // RESUME BUTTON
        executeResume();

    } else {
        // moving?
//        p_isMoving = true;
//        setOpacity(0.8);

        // WHEN MOVE ITEM IS ACTIVATED, THIS BRING THE CLICKED ITEM ON TOP OF OVERLAPPED ITEM
//        int z =0;
//        foreach (QGraphicsItem *item, this->collidingItems(Qt::IntersectsItemBoundingRect))
//            item->topLevelItem()->setZValue(--z);
//        this->setZValue(0);

        // deselect if selected (not done oob by qt) else let qt manage event
        if (this->isSelected()) {
            this->setSelected(false);
        } else {
            QGraphicsItem::mouseReleaseEvent(event);
        }
    }
}

void tsuItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    if (!p_isFadingOut && !p_isFadingIn && !p_isMoving ) {
        p_itemShadowAnimation->stop();
        p_itemShadowAnimation->setDirection(QAbstractAnimation::Forward);
        p_itemShadowAnimation->start();
    }
}

void tsuItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    if (!p_isFadingOut && !p_isFadingIn && !p_isMoving ) {
        p_itemShadowAnimation->stop();
        p_itemShadowAnimation->setDirection(QAbstractAnimation::Backward);
        p_itemShadowAnimation->start();
    }
}

void tsuItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverMoveEvent(event);

    setToolTip("");
    setCursor(Qt::ArrowCursor);

    // TOOLTIP ON CLICKABLE ITEMS
    int x = event->lastPos().x();
    int y = event->lastPos().y();
    if ( (x >= p_imageExitPosition.x() && x <= (p_imageExitPosition.x() + p_imageExit.width()) ) &&
         (y >= p_imageExitPosition.y() && y <= (p_imageExitPosition.y() + p_imageExit.height()) ) ) {
        setToolTip("Remove");
        setCursor(Qt::PointingHandCursor);
    } else {
        if (p_status == statusEnum::finished) {
            if ( (x >= p_imageStateFinishedPosition.x() && x <= (p_imageStateFinishedPosition.x() + p_imageStateF.width())) &&
                 (y >= p_imageStateFinishedPosition.y() && y <= (p_imageStateFinishedPosition.y() + p_imageStateF.height()) ) ) {
                setToolTip("Finished, YAY!");
            }
        } else {
            if ( (x >= p_imagePausePosition.x() && x <= (p_imagePausePosition.x() + p_imagePause.width())) &&
                 (y >= p_imagePausePosition.y() && y <= (p_imagePausePosition.y() + p_imagePause.height()) ) ) {
                setToolTip("Pause");
                setCursor(Qt::PointingHandCursor);
            } else
            if ( (x >= p_imageResumePosition.x() && x <= (p_imageResumePosition.x() + p_imageResume.width())) &&
                 (y >= p_imageResumePosition.y() && y <= (p_imageResumePosition.y() + p_imageResume.height()) ) ) {
                setToolTip("Resume");
                setCursor(Qt::PointingHandCursor);
            }
        }
        if ( (x >= 8 && x <= 120) && (y >= 6 && y <= 17) ) {
             setToolTip("Click for torrent details");
             setCursor(Qt::PointingHandCursor);
        }
    }
}

QVariant tsuItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    // SNAP TO GRID
//    if (change == ItemPositionChange && scene()) {
//        QPointF newPos = value.toPointF();
//            int gridSizeX = this->boundingRect().width() + ItemGlowRadius;
//            int gridSizeY = this->boundingRect().height() + ItemGlowRadius;
//            qreal xV = round(newPos.x()/gridSizeX)*gridSizeX;
//            qreal yV = round(newPos.y()/gridSizeY)*gridSizeY;
//            return QPointF(xV, yV);
//    }
//    else
        return QGraphicsItem::itemChange(change, value);
}

QString tsuItem::convertSize(const int &size)
{
    if (size==0) return "0";
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("b");

    while(num >= 1000.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1000.0;
    }

    int length = 1;
    int x = num;
    while ( x /= 10 )
       length++;

    return QString().setNum(num,'f',3-length);

//    if (unit == "GB" || unit == "TB") {
//        return QString().setNum(num,'f',2);//+" "+unit;
//    } else {
//        return QString().setNum(num,'f',0);//+" "+unit;
//    }
}

QString tsuItem::convertSizeUnit(const int &size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("b");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return unit;
}

QString tsuItem::remainingTime()
{
    if (p_rateDownload <= 0) return "∞";
    double remaining = (p_size-p_downloaded)/p_rateDownload;
    if (remaining <= 0) return "∞";

    double seconds = std::fmod(remaining, 60);
    remaining /= 60;
    double minutes = std::fmod(remaining, 60);
    remaining /= 60;
    double hours = std::fmod(remaining, 24);

    QString sHou = QString::number(std::floor(hours));
    QString sMin = QString::number(std::floor(minutes));
    QString sSec = QString::number(std::floor(seconds));

    return QString("%1:%2:%3").arg(sHou.rightJustified(2, '0'))
                              .arg(sMin.rightJustified(2, '0'))
                              .arg(sSec.rightJustified(2, '0'));
}

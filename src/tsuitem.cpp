#include "tsuitem.h"

#include "bytevalue.h"

const qreal tsuItem::ItemWidth = 140;
const qreal tsuItem::ItemHeight = 190;
const qreal tsuItem::ItemGlowRadius = 20;


tsuItem::tsuItem(const std::string &hash)
{
    p_hash = hash;
    createItem();
}


tsuItem::tsuItem() : tsuItem(std::string())
{
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
    fadeInAnimation->setEasingCurve(QEasingCurve::OutQuad);
    fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    connect(fadeInAnimation,SIGNAL(finished()),this,SLOT(fadeInFinished()));

    p_gradientProgress.setCenter(p_rectProgress.center());
    p_gradientProgress.setAngle(90);

    // will be populated with first date during fast resume
    p_eta = QDateTime::currentDateTime();

}

QRectF tsuItem::boundingRect() const
{
    // see example of "QRectF CircleItem::boundingRect()" at https://doc.qt.io/qt-5/qgraphicsitem.html#boundingRect
    // I used tsuItem::ItemGlowRadius as a sort of "pen width"

    static const qreal half_radius = (0.5 * tsuItem::ItemGlowRadius);

    auto tot_width = (tsuItem::ItemWidth + tsuItem::ItemGlowRadius);
    auto tot_height = (tsuItem::ItemHeight + tsuItem::ItemGlowRadius);

    return QRectF(-half_radius, -half_radius,
                  tot_width, tot_height);

}

void tsuItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // The following will be used as "starting point" to draw the head text (filename, truncated if needed)
    static const QPointF head_start_position{8, 16};

    // http://thesmithfam.org/blog/2007/02/03/qt-improving-qgraphicsview-performance/
    painter->setClipRect( option->exposedRect );

//    painter->setRenderHint(QPainter::Antialiasing);
//    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QString text = QString::number(p_progressValue);
    QString symbol("%");
    double progress = 0.01*p_progressValue;

    // BACKGROUND IMAGE
    painter->drawPixmap(0, 0, tsuItem::ItemWidth, tsuItem::ItemHeight, p_imageBkg);

    // SELECTED
    if (!p_isFadingOut && !p_isFadingIn && !p_isMoving && this->isSelected() ) {
        painter->drawPixmap(p_imageSelectedPosition, p_imageSelected);
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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int lenText = painter->fontMetrics().horizontalAdvance(text);
#else
    int lenText = painter->fontMetrics().width(text);
#endif

    painter->setFont(p_fontSymbol);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int lenSymb = painter->fontMetrics().horizontalAdvance(symbol);
#else
    int lenSymb = painter->fontMetrics().width(symbol);
#endif

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

    // Try to avoid to overlap with the exit symbol
    // Compute available width space
    QFont head_font("Tahoma", 8);
    QFontMetrics head_font_metric(head_font);
    qreal estimated_width = (p_imageExitPosition.x() - head_start_position.x());

    // Obtain a text to display in place of original head text:
    // - if available width is enough to display the original text then "head_to_use" will be just a copy of the original text
    // - otherwise head_to_use will be a string beginning and ending as the original text but shorter as needed to be printed in the available width, and will display "..." in the middle
    //   For example, instead of "Supercalifragilisticexpialidocious.mp3" the displayed string will be something like "Super....mp3"
    QString head_to_use = head_font_metric.elidedText(p_head, Qt::TextElideMode::ElideMiddle, static_cast<int>(estimated_width));

    pathTitle.addText(head_start_position, head_font, head_to_use);
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
    // obtain strings to use for file size, file remaining, uprate and downrate
    QString sSize_inU;
    QString sSize_ULabel;
    CByteValue::convertToRankValueAndGetStrings_size(static_cast<uint64_t>(p_size), sSize_inU, sSize_ULabel);

    QString sDown_inU;
    QString sDown_ULabel;
    CByteValue::convertToRankValueAndGetStrings_size(static_cast<uint64_t>(p_downloaded), sDown_inU, sDown_ULabel);


    decltype(p_size) remaining = (p_size > p_downloaded) ? (p_size - p_downloaded) : 0;

    QString sRema_inU;
    QString sRema_ULabel;
    CByteValue::convertToRankValueAndGetStrings_size(static_cast<uint64_t>(remaining), sRema_inU, sRema_ULabel);

    QString sRateUp_inU;
    QString sRateUp_ULabel;
    CByteValue::convertToRankValueAndGetStrings_rate(static_cast<uint64_t>(p_rateUpload), sRateUp_inU, sRateUp_ULabel);

    QString sRateDown_inU;
    QString sRateDown_ULabel;
    CByteValue::convertToRankValueAndGetStrings_rate(static_cast<uint64_t>(p_rateDownload), sRateDown_inU, sRateDown_ULabel);


    /////////////////////////////////////////////////////////////////////
    //
    //
    //      |DESCR| |VAL| |UM|
    //   012345678901234567890
    //
    // TOT = 30
    // LEN_UM = 30 - 27 +1 = 4
    // X_UM = TOT - LEN_UM + 1
    // LEN_VAL = 6
    // X_VAL = TOT - (LEN_VAL + LEN_UM) = X_UM - LEN_VAL
    // xfilesizeLABEL = x for FILESIZE LABEL = tsuItem::ItemWidth - spaceOf("MiB  ")
    // xfilesizeVALUE = x for FILESIZE VALUE = xfilesizeLABEL - spaceOf("999.9 ")
    // xdescription   = x for DESCRIPTION = xfilesizeVALE - spaceOf(description)
    //
    int estimated_unit_label_length = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
        estimated_unit_label_length = QFontMetrics(p_indicatorUnitFont).horizontalAdvance("MiB  "); // 2 trailing space to leave some space from right edge
#else
        estimated_unit_label_length = QFontMetrics(p_indicatorUnitFont).horizontalAdvance("MiB  "); // 2 trailing space to leave some space from right edge
#endif

    int estimated_value_length = 0;
    {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
            estimated_value_length = QFontMetrics(p_indicatorFont).horizontalAdvance("999.9 "); // Probably nobody will try to download files with size > of 999.9 TiB!
        #else
            estimated_value_length = QFontMetrics(p_indicatorFont).width().horizontalAdvance("999.9 ");
        #endif
    }

    int x_size_UM    = tsuItem::ItemWidth - estimated_unit_label_length;
    int x_size_value = x_size_UM - estimated_value_length;



    // Label
    painter->setFont(p_fontLabel);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int lenTot = painter->fontMetrics().horizontalAdvance(tr("Total") + QString("  "));      // Trailing spaces to have some space between description and value
    int lenDow = painter->fontMetrics().horizontalAdvance(tr("Downloaded") + QString("  "));
    int lenRem = painter->fontMetrics().horizontalAdvance(tr("Remaining") + QString("  "));
#else
    int lenTot = painter->fontMetrics().width(tr("Total") + QString("  "));
    int lenDow = painter->fontMetrics().width(tr("Downloaded") + QString("  "));
    int lenRem = painter->fontMetrics().width(tr("Remaining") + QString("  "));
#endif
    QPainterPath pathLabels;
    pen.setColor(Qt::transparent);
    pen.setWidthF(0.5);
    painter->setBrush(p_colorLabel);
    painter->setPen(pen);

    pathLabels.addText(x_size_value - lenTot, 34, p_fontLabel, tr("Total"));
    pathLabels.addText(x_size_value - lenDow, 46, p_fontLabel, tr("Downloaded"));
    pathLabels.addText(x_size_value - lenRem, 58, p_fontLabel, tr("Remaining"));
    painter->drawPath(pathLabels);

    // Indicators

    painter->setFont(p_indicatorFont);
    QPainterPath pathGreenIndicators;
    painter->setBrush(QColor(7,252,18));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int precise_size_value_length = painter->fontMetrics().horizontalAdvance(sSize_inU + QString(" "));
    int precise_down_value_length = painter->fontMetrics().horizontalAdvance(sDown_inU + QString(" "));
    int precise_rema_value_length = painter->fontMetrics().horizontalAdvance(sRema_inU + QString(" "));
#else
    int precise_size_value_length = painter->fontMetrics().width().horizontalAdvance(sSize_inU + QString(" "));
    int precise_down_value_length = painter->fontMetrics().width().horizontalAdvance(sDown_inU + QString(" "));
    int precise_rema_value_length = painter->fontMetrics().width().horizontalAdvance(sRema_inU + QString(" "));
#endif
    int precise_x_size_value = x_size_UM - precise_size_value_length;
    int precise_x_down_value = x_size_UM - precise_down_value_length;
    int precise_x_rema_value = x_size_UM - precise_rema_value_length;

    pathGreenIndicators.addText(precise_x_size_value, 34, p_indicatorFont, sSize_inU);
    pathGreenIndicators.addText(x_size_UM, 34, p_indicatorUnitFont, sSize_ULabel);
    painter->drawPath(pathGreenIndicators);

    QPainterPath pathOrangeIndicators;
    painter->setBrush(QColor(255,196,0));
    pathOrangeIndicators.addText(precise_x_down_value, 46, p_indicatorFont, sDown_inU);
    pathOrangeIndicators.addText(x_size_UM, 46, p_indicatorUnitFont, sDown_ULabel);
    painter->drawPath(pathOrangeIndicators);

    QPainterPath pathBluIndicators;
    painter->setBrush(QColor(0,144,255));
    pathBluIndicators.addText(precise_x_rema_value, 58, p_indicatorFont, sRema_inU);
    pathBluIndicators.addText(x_size_UM, 58, p_indicatorUnitFont, sRema_ULabel);
    painter->drawPath(pathBluIndicators);

    // LOAD INDICATORS
    QFont loadUnitFont = QFont("Tahoma", 8, QFont::Thin);
    QPainterPath loadUnitIndicators;
    painter->setBrush(QColor(0,144,255));
    painter->setFont(p_indicatorFont);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int ruuSize = painter->fontMetrics().horizontalAdvance(sRateUp_ULabel);
    int rduSize = painter->fontMetrics().horizontalAdvance(sRateDown_ULabel);
#else
    int ruuSize = painter->fontMetrics().width(sRateUp_ULabel);
    int rduSize = painter->fontMetrics().width(sRateDown_ULabel);
#endif
    loadUnitIndicators.addText(10-(ruuSize/2), 90, loadUnitFont, sRateUp_ULabel);
    loadUnitIndicators.addText(127-(rduSize/2), 90, loadUnitFont, sRateDown_ULabel);
    painter->drawPath(loadUnitIndicators);

    QFont loadFont = QFont("Tahoma", 8, QFont::Thin);
    QPainterPath loadIndicatorUp;
    QPainterPath loadIndicatorDown;
    painter->setFont(loadFont);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int ruSize = painter->fontMetrics().horizontalAdvance(sRateUp_inU);
    int rdSize = painter->fontMetrics().horizontalAdvance(sRateDown_inU);
#else
    int ruSize = painter->fontMetrics().width(sRateUp_inU);
    int rdSize = painter->fontMetrics().width(sRateDown_inU);
#endif
    painter->setBrush(QColor(131,131,131));
    loadIndicatorUp.addText(18-(ruSize/2), 103, loadFont, sRateUp_inU);
    painter->drawPath(loadIndicatorUp);
    painter->setBrush(QColor(255,144,0));
    loadIndicatorDown.addText(120-(rdSize/2), 103, loadFont, sRateDown_inU);
    painter->drawPath(loadIndicatorDown);

    if (this->p_status != statusEnum::finished) {
        // REMAINING TIME
        QString remaining = remainingTime();
        QPainterPath pathReminingTime;
        QFont remainingFont = QFont("Tahoma", 8, QFont::DemiBold);
        painter->setFont(remainingFont);
        painter->setBrush(QColor(255,255,255));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
        int remSize = painter->fontMetrics().horizontalAdvance(remaining);
#else
        int remSize = painter->fontMetrics().width(remaining);
#endif
        pathReminingTime.addText(70-(remSize/2), 148, remainingFont, remaining);
        painter->drawPath(pathReminingTime);

        // ETA
        QPainterPath pathEta;
        QFont etaFont = QFont("Tahoma", 7, QFont::Thin);
        painter->setFont(etaFont);
        painter->setBrush(QColor(131, 131, 131));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
        int etaSize = painter->fontMetrics().horizontalAdvance(p_eta.toString("dd/MM/yyyy hh:mm"));
#else
        int etaSize = painter->fontMetrics().width(p_eta.toString("dd/MM/yyyy hh:mm"));
#endif
        pathEta.addText(70-(etaSize/2), 158, etaFont, p_eta.toString("dd/MM/yyyy hh:mm"));
        painter->drawPath(pathEta);
    }

    // SEEDS
    QPainterPath seedsPath;
    QFont seedsFont = QFont("Tahoma", 8, QFont::Thin);
    painter->setFont(seedsFont);
    painter->setBrush(QColor(131, 131, 131));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int seedsSize = painter->fontMetrics().horizontalAdvance(p_numSeeds);
#else
    int seedsSize = painter->fontMetrics().width(p_numSeeds);
#endif
    seedsPath.addText(10-(seedsSize/2), 135, seedsFont, QString::number(p_numSeeds));
    painter->drawPath(seedsPath);

    // PEERS
    QPainterPath peersPath;
    QFont peersFont = QFont("Tahoma", 8, QFont::Thin);
    painter->setFont(peersFont);
    painter->setBrush(QColor(131, 131, 131));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11 , 0))
    int peersSize = painter->fontMetrics().horizontalAdvance(p_numPeers);
#else
    int peersSize = painter->fontMetrics().width(p_numPeers);
#endif
    peersPath.addText(122-(peersSize/2), 135, peersFont, QString::number(p_numPeers));
    painter->drawPath(peersPath);

}

void tsuItem::set_FactorTransform(const qreal &value)
{
    p_factorTransform = value;
    this->setScale(value);
    update();
}

qreal tsuItem::get_FactorTransform() const
{
    return p_factorTransform;
}

const QString& tsuItem::get_Head() const
{
    return p_head;
}

void tsuItem::set_Size(const uint64_t &value)
{
    p_size = value;
}

void tsuItem::set_Downloaded(const uint64_t &value)
{
    p_downloaded = value;
}

void tsuItem::set_Uploaded(const uint64_t &value)
{
    p_uploaded = value;
}

uint64_t tsuItem::get_Downloaded() const
{
    return p_downloaded;
}

uint64_t tsuItem::get_Uploaded() const
{
    return p_uploaded;
}

void tsuItem::setValue(const tsuEvents::tsuEvent &event)
{
//    qDebug() << QString("setValue event.percentage %1").arg(event.percentage);
    p_progressValue = event.percentage/10000;
    p_downloaded = qFabs(event.downloaded);
    p_rateDownload = qFabs(event.downloadRate);
    p_head = event.name;
    p_size = qFabs(event.total_size);
    p_uploaded = qFabs(event.uploaded);
    p_rateUpload = qFabs(event.uploadRate);
    p_numSeeds = event.numSeeds;
    p_numPeers = event.numPeers;
    set_Status(event.state);
    update();
}

void tsuItem::emitCancel()
{
    emit cancelRequested(p_hash, p_cancelFilesOnDelete);
}

const std::string& tsuItem::get_Hash() const
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
    a->setEasingCurve(QEasingCurve::OutQuad);
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

void tsuItem::set_RateUpload(const int &value)
{
    p_rateUpload = value;
}

bool tsuItem::get_Visibility() const
{
    return p_visible;
}

void tsuItem::set_Visibility(const bool &visible)
{
    if (p_visible == true && visible == false) {
        // hiding, fade out
        p_isFadingOut = true;
        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(eff);
        QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
        a->setDuration(500);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutQuad);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        connect(a,SIGNAL(finished()),this,SLOT(fadeOutFinished()));
    }

    if (p_visible == false && visible == true) {
        // showing, fade in
        p_isFadingIn = true;
        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(eff);
        QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
        a->setDuration(500);
        a->setStartValue(0);
        a->setEndValue(1);
        a->setEasingCurve(QEasingCurve::OutQuad);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        connect(a,SIGNAL(finished()),this,SLOT(fadeInFinished()));
    }
    p_visible = visible;
    update();
}

int tsuItem::get_RateDownload() const
{
    return p_rateDownload;
}

void tsuItem::set_RateDownload(const int &value)
{
    p_rateDownload = value;
}

statusEnum tsuItem::get_Status() const
{
    return p_status;
}

void tsuItem::set_Status(const int &value)
{
    statusEnum newStatus = static_cast<statusEnum>(value);
    if ( p_status == statusEnum::downloading && newStatus == statusEnum::seeding ) {
        qInfo() << QString("download finished for %0").arg(this->get_Head());
        emit downloadFinished(this);
    }
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

    p_itemShadowAnimation = new QPropertyAnimation(p_itemShadow, "color");
    p_itemShadowAnimation->setDuration(250);
    p_itemShadowAnimation->setStartValue(QColor(0, 0, 0));
    p_itemShadowAnimation->setEndValue(QColor(255, 162, 0));

    p_isFadingIn = false;
}

void tsuItem::fadeOutFinished()
{
    p_isFadingOut = false;
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

//    qDebug("mouseReleaseEvent");

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
    if (!p_isFadingOut && !p_isFadingIn && !p_isMoving && p_visible ) {
        p_itemShadowAnimation->stop();
        p_itemShadowAnimation->setDirection(QAbstractAnimation::Forward);
        p_itemShadowAnimation->start();
    }
}

void tsuItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    if (!p_isFadingOut && !p_isFadingIn && !p_isMoving && p_visible ) {
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
             //setToolTip("Click for torrent details");
            setToolTip(QString("%0\n%1").arg(p_head).arg("Click for torrent details"));
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

QString tsuItem::remainingTime() const
{
    if (p_rateDownload <= 0) return "∞";
    double remaining = static_cast<double>(p_size-p_downloaded)/p_rateDownload;
    if (remaining <= 0.0) return "∞";

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

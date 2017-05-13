#ifndef TSUPROGRESS_H
#define TSUPROGRESS_H

#include "QSegment/qsegmentwidget.h"
#include <QtMath>

class TsuProgress : public QSegmentWidget
{
    Q_OBJECT

public:
    TsuProgress(QWidget *parent = 0);
    void setValue(qreal qty);

protected:
    void createProgress();
//    bool SetCylinder(int cyl, qreal angle, int segments, int mode, qreal m_distance);
//    void Segment(int cyl, qreal a1, qreal a2);
//    void Update();

//    typedef QMap<int, SegmentList> SMap;
//    SMap m_cylindersIn, m_cylindersOut;

    qreal m_oldValue = 0;

    qreal m_numSegment = 36;
    qreal m_ratio = (360.0 / m_numSegment);
    qreal m_distance = 0.3;
    qreal m_dAngle = m_ratio * m_distance; // distance
    qreal m_eAngle = m_ratio - m_dAngle; // element angle

    QColor m_colorOn = QColor(255, 208, 0);
    QColor m_colorOff = QColor(100, 100, 100);

};

#endif // TSUPROGRESS_H

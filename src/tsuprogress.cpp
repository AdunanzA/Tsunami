#include "tsuprogress.h"

#include <QDebug>

TsuProgress::TsuProgress(QWidget *parent) : QSegmentWidget(parent)
{
//    qDebug() << QString("segment %1 ; ratio %2").arg(numSegment).arg(ratio);
    createProgress();
}

void TsuProgress::setValue(qreal qty)
{
    if (qty<0 || qty>100) return;

    SetText(qty);

    m_oldValue = qty;
    qty = (qRound(qty * (m_numSegment/100))*m_ratio);
    int repetition = 1;
    int tempvalue = m_oldValue;
    while ( (qRound(tempvalue * (m_numSegment/100))*m_ratio) == qty ) {
        tempvalue++;
        repetition++;
    }
//    qDebug() << QString("oldValue %1 ; qty %2 ; repetation %3").arg(m_oldValue).arg(qty).arg(repetition);

    qreal cAngle = 0;
    int current = 0;
    for (int i = 0; i<361;i=i+m_ratio) {
        current++;
        cAngle = 360.0 + 90.0 - i; // current
        if (cAngle >= 360.0)
          cAngle -= 360.0;
        if (qty == 0 || qty > 360) {
            addSegment(1, cAngle, cAngle + m_eAngle, m_colorOff);
        } else if (qty == 360) {
            addSegment(1, cAngle, cAngle + m_eAngle, m_colorOn);
        } else if (i < qty) {
            addSegment(1, cAngle, cAngle + m_eAngle, m_colorOn);
//            qDebug() << QString("seg %1 rou on").arg(current);
        } else if (i == qty) {
            int newGreen = m_colorOff.green() + qRound((qreal)((m_colorOn.green()-m_colorOff.green())/repetition));
            int newRed = m_colorOff.red() + qRound((qreal)((m_colorOn.red()-m_colorOff.red())/repetition));
            int newBlu = m_colorOff.blue() + qRound((qreal)((m_colorOn.blue()-m_colorOff.blue())/repetition));
            addSegment(1, cAngle, cAngle + m_eAngle, QColor(newRed, newGreen, newBlu));
//            int newH = m_colorOff.hue() + qRound((qreal)((m_colorOn.hue()-m_colorOff.hue())/repetition));
//            int newS = m_colorOff.saturation() + qRound((qreal)((m_colorOn.saturation()-m_colorOff.saturation())/repetition));
//            int newV = m_colorOff.value() + qRound((qreal)((m_colorOn.value()-m_colorOff.value())/repetition));
//            addSegment(1, cAngle, cAngle + m_eAngle, QColor::fromHsv(newH, newS, newV));
//            qDebug() << QString("seg %1 newGreen %2 newRed %3").arg(current).arg(newGreen).arg(newRed);
        } else {
            addSegment(1, cAngle, cAngle + m_eAngle, m_colorOff);
//            qDebug() << QString("seg %1 rou off").arg(current);
        }
    }

}

void TsuProgress::createProgress()
{
    addCylinder(33); // cyl 0 -> empty
    addCylinder(20); // cyl 1 -> our progress meter

    qreal cAngle = 0;
    for (int i = 0; i<361;i=i+m_ratio) {
        cAngle = 360.0 + 90.0 - i; // current
        if (cAngle >= 360.0)
          cAngle -= 360.0;
        addSegment(1, cAngle, cAngle + m_eAngle, m_colorOff);
    }
}


//void TsuProgress::Segment(int cyl, qreal a1, qreal a2)
//{
//  QSegmentWidget::Segment s;//(a1, a2, Qt::transparent);
//  s.first = a1;
//  s.second = a2;
//  s.color = Qt::transparent;
//  m_cylindersIn[cyl].push_back(s);
//}

//void TsuProgress::Update()
//{
//    for (int cyl = 0; cyl != cylinders(); cyl++) {
//      SegmentList &si = m_cylindersIn[cyl];
//      SegmentList &so = m_cylindersOut[cyl];
//      SegmentList ss, sc;
//      /*for (SegmentList::iterator i = si.begin(); i != si.end(); i++) {
//        if (!so.contains(*i)) {
//          ss.append(*i);
//        }
//      }
//      for (SegmentList::iterator i = so.begin(); i != so.end(); i++) {
//        if (!si.contains(*i)) {
//          sc.append(*i);
//        }
//      }*/
//      if (!sc.empty())
//        clearSegments(cyl, sc);
//      if (!ss.empty())
//        addSegments(cyl, ss);
//      so = si;
//    }
//}

//bool TsuProgress::SetCylinder(int cyl, qreal angle, int segments, int mode, qreal distance)
//{
//    if (segments == 0)
//      return false;
//    m_cylindersIn[cyl].clear();
//    qreal sAngle = 360.0 / segments;
//    qreal dAngle = sAngle * distance; // distance
//    qreal eAngle = sAngle - dAngle; // element angle
//    qreal cAngle = 360.0 + 90.0 - angle; // current
//    if (cAngle >= 360.0)
//      cAngle -= 360.0;
//    cAngle -= eAngle / 2.0;

//    switch (mode) {
//      case 0:
//      {
//        // single
//        Segment(cyl, cAngle, cAngle + eAngle);
//        break;
//      }
//      case 1:
//      {
//        // inverse
//        cAngle += sAngle;
//        for (int i = 1; i < segments; i++) {
//          Segment(cyl, cAngle, cAngle + eAngle);
//          cAngle += sAngle;
//        }
//        break;
//      }
//      case 2:
//      {
//        // band
//        if (fabs(angle) < dAngle) {
//          Segment(cyl, cAngle, cAngle + eAngle);
//        } else while (angle > eAngle / 2.0) {
//          Segment(cyl, cAngle, cAngle + eAngle);
//          cAngle += sAngle;
//          angle -= sAngle;
//        }
//        break;
//      }
//    }
//    return true;
//}



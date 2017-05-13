/*******************************************************************************
**
**  Copyright (C) 2009 J?rg Dentler
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this library.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include <QtGui>
#include "qsegmentwidget.h"

#include <QDebug>

QSegmentWidget::QSegmentWidget(QWidget *parent):
  QWidget(parent),
  m_pixmap(0)
{
//  m_color = palette().color(foregroundRole());
    m_color = QColor(Qt::transparent);
}

void QSegmentWidget::InitPixmap()
{
  m_pixmap = new QPixmap(BM_OSIZE, BM_OSIZE);
//  m_pixmap->fill(palette().color(backgroundRole()));
  m_pixmap->fill(Qt::transparent);
  QPainter painter(m_pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setWindow( -(BM_OSIZE / 2), -(BM_OSIZE / 2),
    BM_OSIZE, BM_OSIZE);

  DrawBevel(&painter);
  DrawBackground(&painter);
}

QSegmentWidget::~QSegmentWidget()
{
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::paintEvent(QPaintEvent *)
{
  if (!m_pixmap)
    InitPixmap();
  QPainter painter(this);
  int side = qMin(width(), height());
  painter.setViewport((width() - side) / 2, (height() - side) / 2,
    side, side);
  painter.setWindow(0, 0, BM_OSIZE, BM_OSIZE);
  painter.drawPixmap(0, 0, *m_pixmap);
}

void QSegmentWidget::DrawBevel(QPainter *painter)
{
  painter->save();
//  QRadialGradient g(0, 0, BM_OSIZE / 2, 0, 0);
//  QPalette p = palette();
//  g.setColorAt(0.0, p.color(QPalette::Normal, QPalette::Midlight));
//  g.setColorAt(0.8, p.color(QPalette::Normal, QPalette::Dark));
//  g.setColorAt(0.9, p.color(QPalette::Normal, QPalette::Light));
//  g.setColorAt(1.0, p.color(QPalette::Normal, QPalette::Shadow));
//  painter->setBrush(g);
  painter->setBrush(Qt::NoBrush);
  painter->setPen(Qt::NoPen);
  painter->drawEllipse(
    -((BM_SIZE + BM_BEVEL) / 2),
    -((BM_SIZE + BM_BEVEL) / 2),
    BM_SIZE + BM_BEVEL,
    BM_SIZE + BM_BEVEL);
  painter->restore();
}

void QSegmentWidget::DrawCylinder(const Cylinder &cyl,
  const SegmentList &s, const bool erase)
{
  if (!m_pixmap)
    InitPixmap();
  qreal border = 5;
  QPainter painter(m_pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setWindow( -(BM_OSIZE / 2), -(BM_OSIZE / 2),
    BM_OSIZE, BM_OSIZE);
  QRectF rco(-cyl.outerRadius, -cyl.outerRadius, (cyl.outerRadius * 2.0)+border,
      (cyl.outerRadius * 2.0)+border);
  QRectF rci(-cyl.innerRadius, -cyl.innerRadius, (cyl.innerRadius * 2.0)+border,
      (cyl.innerRadius * 2.0)+border);
  QRegion ro(rco.toRect(), QRegion::Rectangle);
  QRegion ri(rci.toRect(), QRegion::Ellipse);
  QRegion region = ro.subtracted(ri);
  painter.setClipRegion(region);
  if (erase) {
    QBrush b(m_color.darker(2 * 200));
//    painter.setBrush(b);
//    painter.setPen(QPen(b, border));
      painter.setBrush(Qt::NoBrush);
      painter.setPen(Qt::NoPen);
  } else {
//    QBrush b(cyl.color);
//    painter.setBrush(b);
//    painter.setPen(Qt::NoPen);
      QBrush b(QColor(255,208,0));
      painter.setBrush(b);
      QBrush c(QColor(204,127,0));
      painter.setPen(QPen(c, border));
  }
//  qDebug() << QString("processing DrawCylinder for %1,%2").arg(s[0].first).arg(s[0].second);
  DrawSegments(&painter, s, cyl.outerRadius);
}

void QSegmentWidget::DrawSegments(QPainter *painter,
  const SegmentList &s, qreal radius)
{
  for (SegmentList::const_iterator i = s.begin(); i != s.end(); i++) {
    int startAngle = qRound(i->first * 16.0);
    qreal dAngle = i->second - i->first;
    if (dAngle < 0.0)
      dAngle += 360.0;
    int spanAngle = qRound(dAngle * 16.0);
    QRectF r(-radius, -radius, radius * 2.0, radius * 2.0);
    if (spanAngle >= 360 * 16)
      painter->drawEllipse(r);
    else
      painter->drawPie(r, startAngle, spanAngle);
  }
}

void QSegmentWidget::DrawBackground(QPainter *painter)
{
  painter->save();
//  QBrush face(m_color.darker(2 * 200));
  QBrush face(Qt::NoBrush);
  painter->setBrush(face);
  painter->drawEllipse(-BM_RADIUS, -BM_RADIUS, BM_SIZE, BM_SIZE);
  painter->restore();
}

void QSegmentWidget::setColor(const QColor &c)
{
  m_color = c;
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::addCylinder(qreal size)
{
  Q_ASSERT(size > 0.0);
  Cylinder cyl;
  cyl.size = size;
  cyl.color = m_color;
  m_cylinders.push_back(cyl);
  CalculateRadians();
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::addCylinder(const QColor &c, qreal size)
{
  Q_ASSERT(size > 0.0);
  Cylinder cyl;
  cyl.size = size;
  cyl.color = c;
  m_cylinders.push_back(cyl);
  CalculateRadians();
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::clear()
{
  m_cylinders.clear();
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::CalculateRadians()
{
  qreal ssum = 0.0;
  for (QList<Cylinder>::iterator i = m_cylinders.begin();
      i != m_cylinders.end(); i++) {
    ssum += i->size;
  }
  qreal r = 0.0;
  for (QList<Cylinder>::iterator i = m_cylinders.begin();
      i != m_cylinders.end(); i++) {
    qreal s = (i->size / ssum) * BM_RADIUS;
    i->innerRadius = r;
    r += s;
    i->outerRadius = r;
  }
}

void QSegmentWidget::addSegment(int cyl, qreal startAngle, qreal stopAngle)
{
  Segment s(startAngle, stopAngle);
  SegmentList sl;
  sl.push_back(s);
  addSegments(cyl, sl);
}

void QSegmentWidget::addSegments(int cyl, const SegmentList &s)
{
  Q_ASSERT(cyl >= 0 && cyl < m_cylinders.size());
  Cylinder &cylinder = m_cylinders[cyl];
  DrawCylinder(cylinder, s, false);
  update();
}

void QSegmentWidget::clearSegment(int cyl, qreal startAngle, qreal stopAngle)
{
  Segment s(startAngle, stopAngle);
  SegmentList sl;
  sl.push_back(s);
  clearSegments(cyl, sl);
}

void QSegmentWidget::clearSegments(int cyl, const SegmentList &s)
{
  Q_ASSERT(cyl >= 0 && cyl < m_cylinders.size());
  Cylinder &cylinder = m_cylinders[cyl];
  DrawCylinder(cylinder, s, true);
  update();
}

qreal QSegmentWidget::TrimAngle(const qreal &a)
{
  qreal r = a;
  while (r > 360.0)
    r -= 360.0;
  while (r < 0.0)
    r += 360.0;
  return r;
}



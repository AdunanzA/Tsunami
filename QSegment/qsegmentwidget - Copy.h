/*******************************************************************************
**
**  Copyright (C) 2009 Jörg Dentler 
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

#ifndef _QSEGMENTWIDGET_H_
#define _QSEGMENTWIDGET_H_

#include <Qt>
#include <QWidget>
#include <QList>

/**
  @brief QSegmentWidget class provides a rounded segment control.

  QSegmentWidget is intended as base class for segment widgets like
  progress controls, status displays, clocks ...

  @image html SegmentClock1.png

  A QSegmentWidget contains a number of cylinders with a specified width. 
  Every cylinder can show a number of arcs with a defined start and stop
  angle. The segments can be modified by appropriate slots.

  @authors Joerg Dentler

  @note sclock/sclock  example shows a segment clock.
*/
class QSegmentWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * Defines a segment with start and stop angle.
   */
  typedef QPair<qreal, qreal> Segment;
  /**
   * A list of segments
   */
  typedef QList<Segment> SegmentList;

public: 
  /**
   * Constructor
   *
   * @param parent Parent widget
   */
  QSegmentWidget(QWidget *parent = 0);
  /**
   * @return The number of cylinders in the widget
   */
  int cylinders() const { return m_cylinders.size(); }
  /**
   * @return The current widget color
   */
  QColor color() const { return m_color; }

  ~QSegmentWidget();

public slots:
  /**
   * This slot is used to set the default segment color.
   */
	void setColor(const QColor &c);
  /**
   * This slot is used to add a new cylinder at the outer ring.
   * @param size Relative cylinder size
   */
  void addCylinder(qreal size = 1.0);
  /**
   * This slot is used to add a new cylinder at the outer ring.
   * @param c The cylinder color to be used
   * @param size Relative cylinder size
   */
  void addCylinder(const QColor &c, qreal size = 1.0);
  /**
   * This slot is used to draw a new segment.
   * 
   * A segment with a specified start and stop angle will be drawn.
   * The angles should be specified in degrees and can be > 360° or < 0°.
   * @param cyl The index of the cylinder for drawing.
   * @param startAngle The start angle of the current segment
   * @param stopAngle The stop angle of the current segment
   */
  void addSegment(int cyl, qreal startAngle, qreal stopAngle);
  /**
   * This slot is used to draw new segments.
   * 
   * A list of segments will be drawn.
   * @param cyl The index of the cylinder for drawing
   * @param s The list of segments to draw
   */
  void addSegments(int cyl, const SegmentList &s);
  /**
   * This slot is used to clear a segment from a cylinder.
   *
   * The specified segment will be drawn with the specified background 
   * color. With this method highlighted segments can be erased.
   * @param cyl The index of the cylinder for clearing
   * @param startAngle The start angle of the segment
   * @param stopAngle The stop angle of the segment
   */
  void clearSegment(int cyl, qreal startAngle, qreal stopAngle);
  /**
   * This slot is used to clear a list of segments from a cylinder.
   *
   * @param cyl The index of the cylinder for clearing
   * @param s The list of segments to clear
   */
  void clearSegments(int cyl, const SegmentList &s);
  /**
   * This slot is used to clear all cylinders of the QSegmentWidget.
   */
  void clear();

protected:
  class Cylinder
  {
    public:
      Cylinder():
        size(0.0),
        innerRadius(0.0),
        outerRadius(0.0)
      {  }
      Cylinder(const Cylinder &c):
        size(c.size),
        innerRadius(c.innerRadius),
        outerRadius(c.outerRadius),
        color(c.color)
      {  }
      qreal size;
      qreal innerRadius;
      qreal outerRadius;
      QColor color;
  };
  enum
  {
    BM_SIZE   =   400,
    BM_RADIUS =  (BM_SIZE / 2),
    BM_BEVEL  = 0,
    BM_OSIZE  =  (BM_SIZE + BM_BEVEL),
  };
//BM_BEVEL  = ((BM_SIZE * 8) / 100),
protected:
  QColor m_color;
  QList<Cylinder> m_cylinders;
  QPixmap *m_pixmap;

protected:
  void paintEvent(QPaintEvent *event);
  void InitPixmap();
  void DrawBevel(QPainter *painter);
  void DrawBackground(QPainter *painter);
  void DrawCylinder(const Cylinder &cyl, 
    const SegmentList &s, const bool erase);
  void DrawSegments(QPainter *painter, 
    const SegmentList &s, qreal radius);
  void CalculateRadians();
  qreal TrimAngle(const qreal &a);
};

#endif




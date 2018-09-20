/* © Pusterla Fabio 2011 */

/* © Pusterla Fabio 2011 */

#ifndef CHARTXYRENDER_H
  #define CHARTXYRENDER_H

  #include <QObject>
  #include <QAbstractItemView>
  #include <QPainter>
  #include <QBrush>
  #include <QPen>
  #include <QDateTime>
  #include <QVariant>
  #include "chartxysetting.h"
  #include "chartxyalgoritm.h"
  #include "chartxyfunction.h"

  class ChartXYRender: public QObject
    {
      Q_OBJECT
    public:
      explicit ChartXYRender(QAbstractItemView *item_view,ChartXYSetting *setting);

    signals:

    public slots:
      void paint(void);

    protected slots:
      void paintBorder(QPainter &painter);
      void paintGrid(QPainter &painter);
      void paintGridLine(QPainter &painter,QRect &rect);
      void paintGridLineX(QPainter &painter,QRect &rect);
      void paintGridLineY(QPainter &painter,QRect &rect);
      void paintAxis(QPainter &painter,QRect &rect);
      void paintAxisX(QPainter &painter,QRect &rect);
      void paintAxisXTickvalue(QPainter &painter,QRect &rect);
      void paintAxisY(QPainter &painter,QRect &rect);
      void paintAxisYTickvalue(QPainter &painter,QRect &rect);


      void paintData(QPainter &painter,QRect &rect);
      void paintDataColumn(QPainter &painter,QRect &rect,int column);




      // This function return a tickValue
      QVariant tickValue(const ChartXYLimitAxis &limit_axis,const ChartXYGridTick &grid_tick,unsigned int tick) const;
      // This function convert tick value to string
      QString tickValueToString(QVariant tick_value);
      // This function return pointF of the x,y data
      QPointF dataGridPosition(QRect &rect,QVariant x,QVariant y);

    private:
      QAbstractItemView *m_item_view;
      QWidget *m_widget;
      ChartXYSetting *m_setting;
    };

#endif // CHARTXYRENDER_H

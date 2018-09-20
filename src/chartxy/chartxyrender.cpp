#include "chartxyrender.h"

ChartXYRender::ChartXYRender(QAbstractItemView *item_view,ChartXYSetting *setting):
  QObject((QObject *)item_view)
  {
  m_item_view=item_view;
  m_widget=item_view->viewport();
  m_setting=setting;
  }

void ChartXYRender::paint(void)
  {
  QPainter painter(m_widget);

  paintBorder(painter);
  paintGrid(painter);
  }

void ChartXYRender::paintBorder(QPainter &painter)
  {
  painter.fillRect(m_widget->rect(),m_setting->border().backgroundColor());
//    painter.fillRect(m_widget->rect(), QBrush(QColor(10,10,10,0)));
  }

void ChartXYRender::paintGrid(QPainter &painter)
  {
  QRect rect=m_widget->rect();

  // If the grid space is too small render is stopped
  if(
     (m_widget->width()<=(int)(m_setting->border().left()+m_setting->border().right()+m_setting->grid().border()*2))||
     (m_widget->height()<=(int)(m_setting->border().top()+m_setting->border().bottom()+m_setting->grid().border()*2))
    )
    {
    return;
    }

  // Adjust the rect
  rect.adjust(m_setting->border().left(),
              m_setting->border().top(),
              -(m_setting->border().right()-1),
              -(m_setting->border().bottom())
              );

  // Fill background grid
  painter.fillRect(rect,m_setting->grid().backgroundColor());

  // Paint Grid
  if(m_setting->grid().visible())
    {
    paintGridLine(painter,rect);
    }

  // Axis
  paintAxis(painter,rect);

  // Data
  // Adjust the rect
  rect.adjust(m_setting->grid().border(),
              m_setting->grid().border(),
              -(m_setting->grid().border()),
              -(m_setting->grid().border())
              );
  paintData(painter,rect);
  }

void ChartXYRender::paintGridLine(QPainter &painter,QRect &rect)
  {
  painter.save();
  paintGridLineX(painter,rect);
  paintGridLineY(painter,rect);
  painter.restore();
  }

void ChartXYRender::paintGridLineX(QPainter &painter,QRect &rect)
  {
  QPen pen_line(QBrush(m_setting->grid().foregroundColor()),1,Qt::DotLine);
  QRect rect_axis=rect;
  QPointF point_major_top;
  QPointF point_major_bottom;
  unsigned int i;

  painter.setPen(pen_line);

  // Adjust axis border
  rect_axis.adjust(m_setting->grid().border(),0,-(m_setting->grid().border()),0);
  point_major_top=rect_axis.topLeft();
  point_major_bottom=rect_axis.bottomLeft();


  // Draw major tick
  for(i=0;i<m_setting->grid().horizzontalTick().tickMajor();i++)
    {
    QPointF point_delta=QPointF((((qreal)rect_axis.width()-1)*(i))/(m_setting->grid().horizzontalTick().tickMajor()-1),0);

    painter.drawLine(point_major_top+point_delta,point_major_bottom+point_delta);
    }
  }

void ChartXYRender::paintGridLineY(QPainter &painter,QRect &rect)
  {
  QPen pen_line(QBrush(m_setting->grid().foregroundColor()),1,Qt::DotLine);
  QRect rect_axis=rect;
  QPointF point_major_left;
  QPointF point_major_right;
  unsigned int i;

  painter.setPen(pen_line);

  // Adjust axis border
  rect_axis.adjust(0,m_setting->grid().border(),0,-(m_setting->grid().border()));
  point_major_left=rect_axis.bottomLeft();
  point_major_right=rect_axis.bottomRight();


  // Draw major tick
  for(i=0;i<m_setting->grid().verticalTick().tickMajor();i++)
    {
    QPointF point_delta=QPointF(0,-(((qreal)rect_axis.height()-1)*(i))/(m_setting->grid().verticalTick().tickMajor()-1));

    painter.drawLine(point_major_left+point_delta,point_major_right+point_delta);
    }
  }


void ChartXYRender::paintAxis(QPainter &painter,QRect &rect)
  {

  painter.save();
  paintAxisX(painter,rect);
  paintAxisY(painter,rect);
  painter.restore();
  }

void ChartXYRender::paintAxisX(QPainter &painter,QRect &rect)
  {
  QPen pen_axis(QBrush(m_setting->grid().foregroundColor()),2);
  QPen pen_major(QBrush(m_setting->grid().foregroundColor()),2);
  QPen pen_minor(QBrush(m_setting->grid().foregroundColor()),1);
  QRect rect_axis=rect;
  unsigned int i,j;

  // Draw axis
  painter.setPen(pen_axis);
  painter.drawLine(rect.bottomLeft(),rect.bottomRight());

  // Adjust axis border
  rect_axis.adjust(m_setting->grid().border(),0,-(m_setting->grid().border()),0);



  // Draw major tick
  for(i=0;i<m_setting->grid().horizzontalTick().tickMajor();i++)
    {
    QPointF point_major=rect_axis.bottomLeft();
    point_major+=QPointF((((qreal)rect_axis.width())*(i))/(m_setting->grid().horizzontalTick().tickMajor()-1),0);

    // Draw minor tick
    if(i<m_setting->grid().horizzontalTick().tickMajor()-1)
      {
      for(j=0;j<m_setting->grid().horizzontalTick().tickMinor();j++)
        {
        QPointF point_minor=point_major;

        point_minor+=QPointF(((((qreal)rect_axis.width())/(m_setting->grid().horizzontalTick().tickMajor()-1)*j)/(m_setting->grid().horizzontalTick().tickMinor()-1)),0);

        painter.setPen(pen_minor);
        painter.drawLine(point_minor,point_minor+QPointF(0,6));
        }
      }

    painter.setPen(pen_major);
    painter.drawLine(point_major,point_major+QPointF(0,8));
    }
  paintAxisXTickvalue(painter,rect);
  }

void ChartXYRender::paintAxisXTickvalue(QPainter &painter,QRect &rect)
  {
  unsigned int i;
  QRect rect_axis=rect;
  qreal half_text_length;
  int pixel_size=painter.fontInfo().pixelSize();

  // Adjust axis border
  rect_axis.adjust(m_setting->grid().border(),0,-(m_setting->grid().border()),0);
  half_text_length=((((qreal)rect_axis.width()))/(m_setting->grid().horizzontalTick().tickMajor()-1))/3.0;

  for(i=0;i<m_setting->grid().horizzontalTick().tickMajor();i++)
    {
    QString text;
    QPointF point=rect_axis.bottomLeft();
    point+=QPointF((((qreal)rect_axis.width())*(i))/(m_setting->grid().horizzontalTick().tickMajor()-1)-half_text_length,12);
    QRectF text_rect(point,point+QPoint(half_text_length*2,pixel_size));

    text=tickValueToString(tickValue(m_setting->scale().currentLimit().limitX(),m_setting->grid().horizzontalTick(),i));

    painter.drawText(text_rect,text,QTextOption((Qt::AlignHCenter)|(Qt::AlignTop)));
    }
  }


void ChartXYRender::paintAxisY(QPainter &painter,QRect &rect)
  {
  QPen pen_axis(QBrush(m_setting->grid().foregroundColor()),2);
  QPen pen_major(QBrush(m_setting->grid().foregroundColor()),2);
  QPen pen_minor(QBrush(m_setting->grid().foregroundColor()),1);
  QRect rect_axis=rect;
  unsigned int i,j;

  // Draw axis
  painter.setPen(pen_axis);
  painter.drawLine(rect.bottomLeft(),rect.topLeft());

  // Adjust axis border
  rect_axis.adjust(0,m_setting->grid().border(),0,-(m_setting->grid().border()));



  // Draw major tick
  for(i=0;i<m_setting->grid().verticalTick().tickMajor();i++)
    {
    QPointF point_major=rect_axis.bottomLeft();
    point_major+=QPointF(0,-(((qreal)rect_axis.height())*(i))/(m_setting->grid().verticalTick().tickMajor()-1));

    // Draw minor tick
    if(i<m_setting->grid().verticalTick().tickMajor()-1)
      {
      for(j=0;j<m_setting->grid().verticalTick().tickMinor();j++)
        {
        QPointF point_minor=point_major;

        point_minor+=QPointF(0,-((((qreal)rect_axis.height())/(m_setting->grid().verticalTick().tickMajor()-1)*j)/(m_setting->grid().verticalTick().tickMinor()-1)));

        painter.setPen(pen_minor);
        painter.drawLine(point_minor,point_minor+QPointF(-6,0));
        }
      }

    painter.setPen(pen_major);
    painter.drawLine(point_major,point_major+QPointF(-8,0));
    }
  paintAxisYTickvalue(painter,rect);
  }

void ChartXYRender::paintAxisYTickvalue(QPainter &painter,QRect &rect)
  {
  unsigned int i;
  QRect rect_axis=rect;
  qreal border_left=m_setting->border().left();
  int pixel_size=painter.fontInfo().pixelSize();

  if(border_left<16) return;

  // Adjust axis border
  rect_axis.adjust(0,m_setting->grid().border(),0,-(m_setting->grid().border()));
  //half_text_length=((((qreal)rect_axis.height()))/(m_setting->grid().verticalTick().major()-1))/3.0;

  for(i=0;i<m_setting->grid().verticalTick().tickMajor();i++)
    {
    QPointF point_major=rect_axis.bottomLeft();
    point_major+=QPointF(0,-(((qreal)rect_axis.height())*(i))/(m_setting->grid().verticalTick().tickMajor()-1));



    QString text;
    QPointF point=rect_axis.bottomLeft();


    point+=QPointF(-(border_left-16)-12,-(((qreal)rect_axis.height())*(i))/(m_setting->grid().verticalTick().tickMajor()-1)-(((pixel_size*10)+5)/20));
    QRectF text_rect(point,point+QPoint((border_left-16),pixel_size));

    text=tickValueToString(tickValue(m_setting->scale().currentLimit().limitY(),m_setting->grid().verticalTick(),i));


    painter.drawText(text_rect,text,QTextOption((Qt::AlignRight)|(Qt::AlignVCenter)));
    }
  }

void ChartXYRender::paintData(QPainter &painter,QRect &rect)
  {
  int i;


  for(i=1;i<m_item_view->model()->columnCount();i++)
    {
    paintDataColumn(painter,rect,i);
    }
  }

void ChartXYRender::paintDataColumn(QPainter &painter,QRect &rect,int column)
  {
  int i;
  QPointF point;
  QPointF previous_point;
  QVariant x;
  QVariant y;
  bool flag=false;
  QVariant decoration=m_item_view->model()->data(m_item_view->model()->index(0,column),Qt::DecorationRole);

  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setClipRect(rect);

  if(decoration.type()==QVariant::Color)
    {
    QPen pen(decoration.value<QColor>(),2);
    painter.setPen(pen);
    }

  for(i=0;i<m_item_view->model()->rowCount();i++)
    {
    x=m_item_view->model()->data(m_item_view->model()->index(i,0));
    y=m_item_view->model()->data(m_item_view->model()->index(i,column));
    if(
        (x.type()==m_setting->scale().currentLimit().limitX().type())&&
        (y.type()==m_setting->scale().currentLimit().limitY().type())
      )
      {
      point=dataGridPosition(rect,x,y);
      if(flag==false)
        {
        flag=true;
        previous_point=point;
        }

      painter.drawLine(previous_point,point);
      previous_point=point;
      }
    }
  painter.restore();
  }


QVariant ChartXYRender::tickValue(const ChartXYLimitAxis &limit_axis,const ChartXYGridTick &grid_tick,unsigned int tick) const
  {
  if(limit_axis.type()!=QVariant::Invalid)
    {
    if(tick==0)
      {
      return(limit_axis.minimun());
      }
    else
      {
      if(tick==grid_tick.tickMajor()-1)
        {
        return(limit_axis.maximun());
        }
      else
        {
        // Interpolation
        switch(limit_axis.type())
          {
          case QVariant::DateTime:
            {
            quint64 minimun=limit_axis.minimun().toDateTime().toMSecsSinceEpoch();
            quint64 maximun=limit_axis.maximun().toDateTime().toMSecsSinceEpoch();
            quint64 interpulate=(quint64)(ChartXYAlgoritm<double>::Interpulate(0,(double)minimun,grid_tick.tickMajor()-1,(double)maximun,tick));
            return(ChartXYFunction::uint64ToVariantDateTime(interpulate));
            break;
            }
          case QVariant::Date:
            {
            quint64 minimun=limit_axis.minimun().toDateTime().toMSecsSinceEpoch();
            quint64 maximun=limit_axis.maximun().toDateTime().toMSecsSinceEpoch();
            quint64 interpulate=(quint64)(ChartXYAlgoritm<double>::Interpulate(0,(double)minimun,grid_tick.tickMajor()-1,(double)maximun,tick));
            return(ChartXYFunction::uint64ToVariantDate(interpulate));
            break;
            }
          case QVariant::Time:
            {
            quint64 minimun=ChartXYFunction::variantToAbsolute(limit_axis.minimun());
            quint64 maximun=ChartXYFunction::variantToAbsolute(limit_axis.maximun());
            quint64 interpulate=(quint64)(ChartXYAlgoritm<double>::Interpulate(0,(double)minimun,grid_tick.tickMajor()-1,(double)maximun,tick));
            return(ChartXYFunction::uint64ToVariantTime(interpulate));
            break;
            }
          default:
            if(limit_axis.canConvert(QVariant::Double))
              {
              // Interpulate
              return(QVariant(ChartXYAlgoritm<double>::Interpulate(0,limit_axis.minimun().toDouble(),grid_tick.tickMajor()-1,limit_axis.maximun().toDouble(),tick)));
              }
            break;
          }
        }
      }
    /*
    if(limit_axis.canConvert(QVariant::Double))
      {
      if(tick==0)
        {
        return(limit_axis.minimun());
        }
      else
        {
        if(tick==grid_tick.major()-1)
          {
          return(limit_axis.maximun());
          }
        else
          {
          // Interpulate
          return(QVariant(ChartXYAlgoritm<double>::Interpulate(0,limit_axis.minimun().toDouble(),grid_tick.major()-1,limit_axis.maximun().toDouble(),tick)));
          }
        }
      }
    else
      {
      if(limit_axis.canConvert(QVariant::DateTime))
        {
        if(tick==0)
          {
          return(limit_axis.minimun());
          }
        else
          {
          if(tick==grid_tick.major()-1)
            {
            return(limit_axis.maximun());
            }
          else
            {
            quint64 minimun=limit_axis.minimun().toDateTime().toMSecsSinceEpoch();
            quint64 maximun=limit_axis.maximun().toDateTime().toMSecsSinceEpoch();
            quint64 interpulate=(quint64)(ChartXYAlgoritm<double>::Interpulate(0,(double)minimun,grid_tick.major()-1,(double)maximun,tick));
            QDateTime date_time;
            date_time.setMSecsSinceEpoch(interpulate);
            switch(limit_axis.type())
              {
              case QVariant::Date:
                return(QVariant(date_time.date()));
                break;
              case QVariant::Time:
                return(QVariant(date_time.time()));
                break;
              default:
                return(QVariant(date_time));
                break;
              }

            }
          }
        }
      }
    */
    }
  return(QVariant());
  }

QString ChartXYRender::tickValueToString(QVariant tick_value)
  {
  QString text;

  switch(tick_value.type())
    {
    case QVariant::Date:
      if(tick_value.canConvert(QVariant::Date))
        {
        text=QString("%1").arg(tick_value.toDate().toString("dd/MM/yyyy"));
        }
      break;
    case QVariant::Time:
      if(tick_value.canConvert(QVariant::Time))
        {
        text=QString("%1").arg(tick_value.toTime().toString("hh:mm:ss"));
        }
      break;
    case QVariant::DateTime:
      if(tick_value.canConvert(QVariant::DateTime))
        {
        text=QString("%1").arg(tick_value.toDateTime().toString("dd/MM/yyyy hh:mm:ss"));
        }
      break;
    default:
      if(tick_value.canConvert(QVariant::Double))
        {
        text=QString("%1").arg(tick_value.toDouble());
        }
      break;
    }
  return(text);
  }

QPointF ChartXYRender::dataGridPosition(QRect &rect,QVariant x,QVariant y)
  {
  qreal base_x1=(qreal)rect.bottomLeft().x();
  qreal base_x2=(qreal)rect.bottomRight().x();
  qreal limit_x1=ChartXYFunction::variantToAbsolute(m_setting->scale().currentLimit().limitX().minimun());
  qreal limit_x2=ChartXYFunction::variantToAbsolute(m_setting->scale().currentLimit().limitX().maximun());
  qreal point_x=ChartXYAlgoritm<qreal>::Interpulate(limit_x1,base_x1,limit_x2,base_x2,ChartXYFunction::variantToAbsolute(x));

  qreal base_y1=(qreal)rect.bottomLeft().y();
  qreal base_y2=(qreal)rect.topLeft().y();
  qreal limit_y1=ChartXYFunction::variantToAbsolute(m_setting->scale().currentLimit().limitY().minimun());
  qreal limit_y2=ChartXYFunction::variantToAbsolute(m_setting->scale().currentLimit().limitY().maximun());
  qreal point_y=ChartXYAlgoritm<qreal>::Interpulate(limit_y1,base_y1,limit_y2,base_y2,ChartXYFunction::variantToAbsolute(y));

  return(QPointF(point_x,point_y));
  }


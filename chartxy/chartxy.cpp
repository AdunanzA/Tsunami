#include "chartxy.h"

ChartXY::ChartXY(QWidget *parent):
  QAbstractItemView(parent)
  {
  m_setting=new ChartXYSetting(this);
  m_render=new ChartXYRender(this,m_setting);

  //grabMouse(Qt::CrossCursor);
  }

QRect	ChartXY::visualRect(const QModelIndex &index) const
  {
    Q_UNUSED(index);
  return(QRect());
  }

void ChartXY::scrollTo(const QModelIndex &index,ScrollHint hint)
  {
    Q_UNUSED(index);
    Q_UNUSED(hint);
  update();
  }

QModelIndex ChartXY::indexAt(const QPoint &point) const
  {
    Q_UNUSED(point);
  return(QModelIndex());
  }

ChartXYSetting &ChartXY::setting(void)
  {
  return(*m_setting);
  }

unsigned int ChartXY::version(void)
  {
  return(CHARTXY_VERSION);
  }

unsigned int ChartXY::revision(void)
  {
  return(CHARTXY_REVISION);
  }


QModelIndex ChartXY::moveCursor(CursorAction cursorAction,Qt::KeyboardModifiers modifiers)
  {
    Q_UNUSED(cursorAction);
    Q_UNUSED(modifiers);
  return(QModelIndex());
  }

int ChartXY::horizontalOffset() const
  {
  return(0);
  }
int ChartXY::verticalOffset() const

  {
  return(0);
  }

bool ChartXY::isIndexHidden(const QModelIndex &index) const
  {
    Q_UNUSED(index);
  return(false);
  }

void ChartXY::setSelection(const QRect &rect,QItemSelectionModel::SelectionFlags command)
  {
    Q_UNUSED(rect);
    Q_UNUSED(command);
  update();
  }

QRegion ChartXY::visualRegionForSelection(const QItemSelection &selection) const
  {
    Q_UNUSED(selection);
  return(QRegion());
  }

void ChartXY::dataChanged(const QModelIndex &topLeft,const QModelIndex &bottomRight)
  {
  //qDebug() << "Data is changed " << endl;
  QAbstractItemView::dataChanged(topLeft, bottomRight);
  updateChart();
  }


void ChartXY::paintEvent(QPaintEvent *event)
  {
    Q_UNUSED(event);
  m_render->paint();
  }

void ChartXY::resizeEvent(QResizeEvent *event)
  {
    Q_UNUSED(event);
  updateGeometries();
  }

void ChartXY::scrollContentsBy(int dx, int dy)
  {
    Q_UNUSED(dx);
    Q_UNUSED(dy);
  }

void ChartXY::mousePressEvent(QMouseEvent *event)
  {
  if(event->button()==Qt::LeftButton)
    {
    setCursor(Qt::CrossCursor);
    }
  }

void ChartXY::mouseReleaseEvent(QMouseEvent *event)
  {
    Q_UNUSED(event);
  setCursor(Qt::ArrowCursor);
  }

void ChartXY::updateChart(void)
  {
  m_setting->scale().calculateLimit(model(),m_setting->grid().horizzontalTick().tickMajor()-1,m_setting->grid().verticalTick().tickMajor()-1);
  viewport()->update();
  }

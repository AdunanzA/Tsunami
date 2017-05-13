#include "chartxygrid.h"

ChartXYGrid::ChartXYGrid(QObject *parent):
  QObject(parent)
  {
  m_visible=true;
  m_border=8;
  //m_background_color=QColor(32,128,32);
  m_background_color=QColor(192,192,192);
  m_foreground_color=QColor(0,0,0);
  m_vertical_tick=new ChartXYGridTick();
  m_horizzontal_tick=new ChartXYGridTick();
  }

bool ChartXYGrid::visible(void) const
  {
  return(m_visible);
  }

void ChartXYGrid::setVisible(bool visible)
  {
  m_visible=visible;
  }

unsigned int ChartXYGrid::border(void) const
  {
  return(m_border);
  }

void ChartXYGrid::setBorder(unsigned int border)
  {
  m_border=border;
  }

QColor ChartXYGrid::backgroundColor(void) const
  {
  return(m_background_color);
  }

void ChartXYGrid::setBackgroundColor(const QColor &color)
  {
  m_background_color=color;
  }

QColor ChartXYGrid::foregroundColor(void) const
  {
  return(m_foreground_color);
  }

void ChartXYGrid::setForegroundColor(const QColor &color)
  {
  m_foreground_color=color;
  }


ChartXYGridTick &ChartXYGrid::verticalTick(void)
  {
  return(*m_vertical_tick);
  }

ChartXYGridTick &ChartXYGrid::horizzontalTick(void)
  {
  return(*m_horizzontal_tick);
  }


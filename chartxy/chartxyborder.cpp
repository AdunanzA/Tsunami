#include "chartxyborder.h"

ChartXYBorder::ChartXYBorder(QObject *parent):
  QObject(parent)
  {
  m_top=16;
  m_right=16;
  m_bottom=64;
  m_left=64;
  m_background_color=Qt::transparent;
  }

unsigned int ChartXYBorder::top(void) const
  {
  return(m_top);
  }

unsigned int ChartXYBorder::right(void) const
  {
  return(m_right);
  }

unsigned int ChartXYBorder::bottom(void) const
  {
  return(m_bottom);
  }

unsigned int ChartXYBorder::left(void) const
  {
  return(m_left);
  }

void ChartXYBorder::setTop(unsigned int value)
  {
  m_top=value;
  }

void ChartXYBorder::setRight(unsigned int value)
  {
  m_right=value;
  }

void ChartXYBorder::setBottom(unsigned int value)
  {
  m_bottom=value;
  }

void ChartXYBorder::setLeft(unsigned int value)
  {
  m_left=value;
  }

QColor ChartXYBorder::backgroundColor(void) const
  {
  return(m_background_color);
  }

void ChartXYBorder::setBackgroundColor(const QColor &color)
  {
  m_background_color=color;
  }

#include "chartxysetting.h"

ChartXYSetting::ChartXYSetting(QObject *parent):
  QObject(parent)
  {
  m_border=new ChartXYBorder(this);
  m_grid=new ChartXYGrid(this);
  m_scale=new ChartXYScale(this);
  }

ChartXYBorder &ChartXYSetting::border(void)
  {
  return(*m_border);
  }

ChartXYGrid &ChartXYSetting::grid(void)
  {
  return(*m_grid);
  }

ChartXYScale &ChartXYSetting::scale(void)
  {
  return(*m_scale);
  }

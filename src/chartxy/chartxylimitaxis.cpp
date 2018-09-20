#include "chartxylimitaxis.h"

ChartXYLimitAxis::ChartXYLimitAxis()
  {
  }

ChartXYLimitAxis::ChartXYLimitAxis(const ChartXYLimitAxis &limit_axis)
  {
  m_minimun=limit_axis.m_minimun;
  m_maximun=limit_axis.m_maximun;
  }

ChartXYLimitAxis::ChartXYLimitAxis(const QVariant &minimun,const QVariant &maximun)
  {
  m_minimun=minimun;
  m_maximun=maximun;
  }


QVariant ChartXYLimitAxis::minimun(void) const
  {
  return(m_minimun);
  }

QVariant ChartXYLimitAxis::maximun(void) const
  {
  return(m_maximun);
  }

void ChartXYLimitAxis::setLimit(const QVariant &minimun,const QVariant &maximun)
  {
  m_minimun=minimun;
  m_maximun=maximun;
  }


QVariant::Type ChartXYLimitAxis::type(void) const
  {
  if(m_minimun.type()!=m_maximun.type()) return(QVariant::Invalid);

  return(m_minimun.type());
  }

bool ChartXYLimitAxis::canConvert(QVariant::Type type) const
  {
  if(
      (m_minimun.canConvert(type))&&
      (m_maximun.canConvert(type))
    )
    {
    return(true);
    }
  return(false);
  }


ChartXYLimitAxis &ChartXYLimitAxis::operator=(const ChartXYLimitAxis &limit_axis)
  {
  if(this!=&limit_axis)
    {
    m_minimun=limit_axis.m_minimun;
    m_maximun=limit_axis.m_maximun;
    }
  return(*this);
  }


bool ChartXYLimitAxis::operator==(const ChartXYLimitAxis &limit_axis) const
  {
  if(m_minimun!=limit_axis.m_minimun) return(false);
  if(m_maximun!=limit_axis.m_maximun) return(false);
  return(true);
  }

bool ChartXYLimitAxis::operator!=(const ChartXYLimitAxis &limit_axis) const
  {
  if(m_minimun!=limit_axis.m_minimun) return(true);
  if(m_maximun!=limit_axis.m_maximun) return(true);
  return(false);
  }


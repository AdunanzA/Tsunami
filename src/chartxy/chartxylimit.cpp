#include "chartxylimit.h"

ChartXYLimit::ChartXYLimit()
  {
  }

ChartXYLimit::ChartXYLimit(const ChartXYLimit &limit)
  {
  m_limit_x=limit.m_limit_x;
  m_limit_y=limit.m_limit_y;
  }

ChartXYLimit::ChartXYLimit(const ChartXYLimitAxis &limit_x,const ChartXYLimitAxis &limit_y)
  {
  m_limit_x=limit_x;
  m_limit_y=limit_y;
  }


const ChartXYLimitAxis &ChartXYLimit::limitX(void) const
  {
  return(m_limit_x);
  }

const ChartXYLimitAxis &ChartXYLimit::limitY(void) const
  {
  return(m_limit_y);
  }

QVariant::Type ChartXYLimit::type(void) const
  {
  if(m_limit_x.type()!=m_limit_y.type()) return(QVariant::Invalid);

  return(m_limit_x.type());
  }

bool ChartXYLimit::canConvert(QVariant::Type type) const
  {
  if(
      (m_limit_x.canConvert(type))&&
      (m_limit_y.canConvert(type))
    )
    {
    return(true);
    }
  return(false);
  }


ChartXYLimit &ChartXYLimit::operator=(const ChartXYLimit &limit)
  {
  if(&limit!=this)
    {
    m_limit_x=limit.m_limit_x;
    m_limit_y=limit.m_limit_y;
    }
  return(*this);
  }

bool ChartXYLimit::operator==(const ChartXYLimit &limit) const
  {
  if(m_limit_x!=limit.m_limit_x) return(false);
  if(m_limit_y!=limit.m_limit_y) return(false);
  return(true);
  }

bool ChartXYLimit::operator!=(const ChartXYLimit &limit) const
  {
  if(m_limit_x!=limit.m_limit_x) return(true);
  if(m_limit_y!=limit.m_limit_y) return(true);
  return(false);
  }

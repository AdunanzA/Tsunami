#include "chartxyscale.h"

ChartXYScale::ChartXYScale(QObject *parent):
  QObject(parent)
  {
  m_auto_base_limit=false;
  m_auto_current_limit=false;
  }


void ChartXYScale::setBaseLimit(const ChartXYLimit &limit)
  {
  if(m_base_limit!=limit)
    {
    m_base_limit=limit;
    }
  if((m_current_limit.type()==QVariant::Invalid)||(m_base_limit.type()!=m_current_limit.type()))
    {
    m_current_limit=limit;
    }
  }

void ChartXYScale::setCurrentLimit(const ChartXYLimit &limit)
  {
  if(m_current_limit!=limit)
    {
    m_current_limit=limit;
    }
  if((m_base_limit.type()==QVariant::Invalid)||(m_current_limit.type()!=m_base_limit.type()))
    {
    m_base_limit=limit;
    }
  }



const ChartXYLimit &ChartXYScale::baseLimit(void)
  {
  return(m_base_limit);
  }

const ChartXYLimit &ChartXYScale::currentLimit(void)
  {
  return(m_current_limit);
  }

void ChartXYScale::setAutoBaseLimit(bool auto_base_limit)
  {
  m_auto_base_limit=auto_base_limit;
  }

bool ChartXYScale::autoBaseLimit(void) const
  {
  return(m_auto_base_limit);
  }

void ChartXYScale::setAutoCurrentLimit(bool auto_current_limit)
  {
  m_auto_current_limit=auto_current_limit;
  }

bool ChartXYScale::autoCurrentLimit(void) const
  {
  return(m_auto_current_limit);
  }

void ChartXYScale::setNominalAutoLimit(bool nominal_auto_limit)
  {
  m_nominal_auto_limit=nominal_auto_limit;
  }

bool ChartXYScale::nominalAutoLimit(void) const
  {
  return(m_nominal_auto_limit);
  }


void ChartXYScale::calculateLimit(QAbstractItemModel *model,unsigned int x_step,unsigned int y_step)
  {
  QVariant x_minimun;
  QVariant x_maximun;
  QVariant y_minimun;
  QVariant y_maximun;
  QVariant x;
  QVariant y;
  int i,j;

  if((m_auto_base_limit==false)&&(m_auto_current_limit==false)) return;

  for(i=0;i<model->rowCount();i++)
    {
    x=model->data(model->index(i,0));
    if((!x_minimun.isValid())||((ChartXYFunction::variantToAbsolute(x))<(ChartXYFunction::variantToAbsolute(x_minimun))))
      {
      x_minimun=x;
      }
    if((!x_maximun.isValid())||((ChartXYFunction::variantToAbsolute(x))>(ChartXYFunction::variantToAbsolute(x_maximun))))
      {
      x_maximun=x;
      }
    for(j=1;j<model->columnCount();j++)
      {
      y=model->data(model->index(i,j));
      if((!y_minimun.isValid())||((ChartXYFunction::variantToAbsolute(y))<(ChartXYFunction::variantToAbsolute(y_minimun))))
        {
        y_minimun=y;
        }
      if((!y_maximun.isValid())||((ChartXYFunction::variantToAbsolute(y))>(ChartXYFunction::variantToAbsolute(y_maximun))))
        {
        y_maximun=y;
        }
      }
    }

  if(m_nominal_auto_limit)
    {
    Calculate_nominal_limit(x_minimun,x_maximun,x_step,1);
    Calculate_nominal_limit(y_minimun,y_maximun,y_step,1);
    }


  if(m_auto_base_limit)
    {
    setBaseLimit(ChartXYLimit(ChartXYLimitAxis(x_minimun,x_maximun),ChartXYLimitAxis(y_minimun,y_maximun)));
    }

  if(m_auto_current_limit)
    {
    setCurrentLimit(ChartXYLimit(ChartXYLimitAxis(x_minimun,x_maximun),ChartXYLimitAxis(y_minimun,y_maximun)));
    }
  }

void ChartXYScale::Calculate_nominal_limit(QVariant &minimun,QVariant &maximun,unsigned int step,qreal inc)
  {
  qreal value_min=ChartXYFunction::variantToAbsolute(minimun);
  qreal value_max=ChartXYFunction::variantToAbsolute(maximun);

  qreal scale_min=value_min;
  qreal scale_max=value_max;

  if(minimun.type()==QVariant::DateTime) return;
  if(minimun.type()==QVariant::Date) return;
  if(minimun.type()==QVariant::Time) return;
  if(!minimun.canConvert(QVariant::Double)) return;


  if(scale_min==scale_max) return;
  if(minimun.type()!=maximun.type()) return;


  qreal diff=scale_max-scale_min;
  qreal scale=0;
  qreal factor=0;
  bool flag=false;

  // Use decimal if the range is too small
  while(inc*step>diff)
    {
    if(inc/10>0)
      {
      inc/=10;
      }
    else
      {
      break;
      }
    }


  while(diff>scale)
    {
    factor+=inc;
    scale=factor*step;
    }

  while(!flag)
    {
    if(scale_max>=0)
      {
      scale_max=0;
      }
    else
      {
      scale_max=scale_min;
      }
    while(scale_max<value_max)
      {
      scale_max+=factor;
      }
    scale_min=scale_max-scale;
    if(scale_min<=value_min)
      {
      flag=true;
      }
    else
      {
      factor+=inc;
      scale=factor*step;
      }
    }

  while (scale_min+factor<=value_min)
    {
    scale_min+=factor;
    scale_max+=factor;
    }

  switch(minimun.type())
    {
    case QVariant::DateTime:
      /*
      minimun=ChartXYFunction::uint64ToVariantDateTime((scale_min));
      maximun=ChartXYFunction::uint64ToVariantDateTime((scale_max));
      */
      break;
    case QVariant::Date:
      /*
      minimun=ChartXYFunction::uint64ToVariantDate((scale_min));
      maximun=ChartXYFunction::uint64ToVariantDate((scale_max));
      */
      break;
    case QVariant::Time:
      /*
      minimun=ChartXYFunction::uint64ToVariantTime((scale_min));
      maximun=ChartXYFunction::uint64ToVariantTime((scale_max));
      */
      break;
    default:
      if(minimun.canConvert(QVariant::Double))
        {
        QVariant::Type type=minimun.type();
        minimun=QVariant((double)scale_min);
        minimun.convert(type);
        maximun=QVariant((double)scale_max);
        maximun.convert(type);
        }
      break;
    }

  }



QVariant::Type ChartXYScale::type(void) const
  {
  if(m_base_limit.type()!=m_current_limit.type()) return(QVariant::Invalid);

  return(m_base_limit.type());
  }

bool ChartXYScale::canConvert(QVariant::Type type) const
  {
  if(
      (m_base_limit.canConvert(type))&&
      (m_current_limit.canConvert(type))
    )
    {
    return(true);
    }
  return(false);
  }

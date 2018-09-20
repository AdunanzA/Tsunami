#include "chartxyfunction.h"

qreal ChartXYFunction::variantToAbsolute(const QVariant &variant)
  {
  qreal ret=0;

  switch(variant.type())
    {
    case QVariant::Date:
    case QVariant::DateTime:
      if(variant.canConvert(QVariant::DateTime))
        {
        QDateTime puppa=variant.toDateTime();
        ret=(qreal)(variant.toDateTime().toMSecsSinceEpoch());
        }
      break;
    case QVariant::Time:
      {
      QTime time=variant.toTime();
      ret=(qreal)(time.msec()+(time.second()*1000)+(time.minute()*60000)+(time.hour())*3600000);
      break;
      }
    default:
      if(variant.canConvert(QVariant::Double))
        {
        ret=(qreal)variant.toDouble();
        }
      break;
    }
  return(ret);
  }

QVariant ChartXYFunction::uint64ToVariantDateTime(const quint64 value)
  {
  QDateTime date_time;
  date_time.setMSecsSinceEpoch(value);
  return(QVariant(date_time));
  }

QVariant ChartXYFunction::uint64ToVariantDate(const quint64 value)
  {
  QDateTime date_time;
  date_time.setMSecsSinceEpoch(value);
  return(QVariant(date_time.date()));
  }

QVariant ChartXYFunction::uint64ToVariantTime(const quint64 value)
  {
  QTime time((value%86400000)/3600000,(value%3600000)/60000,(value%60000)/1000,(value%1000));
  return(QVariant(time));
  }

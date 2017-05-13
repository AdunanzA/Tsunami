/* © Pusterla Fabio 2011 */

#ifndef CHARTXYSCALE_H
  #define CHARTXYSCALE_H

  #include <QObject>
  #include <QAbstractItemModel>
  #include "chartxylimit.h"
  #include "chartxyfunction.h"

  class ChartXYScale: public QObject
    {
      Q_OBJECT
    public:
      explicit ChartXYScale(QObject *parent=0);


      void setBaseLimit(const ChartXYLimit &limit);
      void setCurrentLimit(const ChartXYLimit &limit);

      const ChartXYLimit &baseLimit(void);
      const ChartXYLimit &currentLimit(void);

      void setAutoBaseLimit(bool auto_base_limit);
      bool autoBaseLimit(void) const;

      void setAutoCurrentLimit(bool auto_current_limit);
      bool autoCurrentLimit(void) const;

      void setNominalAutoLimit(bool nominal_auto_limit);
      bool nominalAutoLimit(void) const;

      void calculateLimit(QAbstractItemModel *model,unsigned int x_step,unsigned int y_step);


      QVariant::Type type(void) const;
      bool canConvert(QVariant::Type type) const;
    signals:

    public slots:
    protected:
      void Calculate_nominal_limit(QVariant &minimun,QVariant &maximun,unsigned int step,qreal inc);

    private:
      ChartXYLimit m_base_limit;
      ChartXYLimit m_current_limit;
      bool m_auto_base_limit;
      bool m_auto_current_limit;
      bool m_nominal_auto_limit;
    };

#endif // CHARTXYSCALE_H

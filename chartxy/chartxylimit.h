/* © Pusterla Fabio 2011 */

#ifndef CHARTXYLIMIT_H
  #define CHARTXYLIMIT_H

  #include "chartxylimitaxis.h"

  class ChartXYLimit
    {
    public:
      ChartXYLimit();
      ChartXYLimit(const ChartXYLimit &limit);
      ChartXYLimit(const ChartXYLimitAxis &limit_x,const ChartXYLimitAxis &limit_y);

      const ChartXYLimitAxis &limitX(void) const;
      const ChartXYLimitAxis &limitY(void) const;

      QVariant::Type type(void) const;
      bool canConvert(QVariant::Type type) const;


      ChartXYLimit &operator=(const ChartXYLimit &limit);
      bool operator==(const ChartXYLimit &limit) const;
      bool operator!=(const ChartXYLimit &limit) const;
    private:
      ChartXYLimitAxis m_limit_x;
      ChartXYLimitAxis m_limit_y;

  };

#endif // CHARTXYLIMIT_H

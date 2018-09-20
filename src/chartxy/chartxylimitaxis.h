/* © Pusterla Fabio 2011 */

#ifndef CHARTXYLIMITAXIS_H
  #define CHARTXYLIMITAXIS_H

  #include <QDateTime>
  #include <QVariant>

  class ChartXYLimitAxis
    {
    public:
      ChartXYLimitAxis();
      ChartXYLimitAxis(const ChartXYLimitAxis &limit_axis);
      ChartXYLimitAxis(const QVariant &minimun,const QVariant &maximun);

      QVariant minimun(void) const;
      QVariant maximun(void) const;
      void setLimit(const QVariant &minimun,const QVariant &maximun);

      QVariant::Type type(void) const;
      bool canConvert(QVariant::Type type) const;

      ChartXYLimitAxis &operator=(const ChartXYLimitAxis &limit_axis);
      bool operator==(const ChartXYLimitAxis &limit_axis) const;
      bool operator!=(const ChartXYLimitAxis &limit_axis) const;
    signals:

    public slots:
    private:
      QVariant m_minimun;
      QVariant m_maximun;

  };

#endif // CHARTXYLIMITAXIS_H

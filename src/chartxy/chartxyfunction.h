/* © Pusterla Fabio 2011 */

#ifndef CHARTXYFUNCTION_H
  #define CHARTXYFUNCTION_H

  #include <QVariant>
  #include <QDateTime>

  class ChartXYFunction
    {
    public:
      // This function convert variant to absolute qreal
      static qreal variantToAbsolute(const QVariant &variant);

      static QVariant uint64ToVariantDateTime(const quint64 value);
      static QVariant uint64ToVariantDate(const quint64 value);
      static QVariant uint64ToVariantTime(const quint64 value);
    };

#endif // CHARTXYFUNCTION_H

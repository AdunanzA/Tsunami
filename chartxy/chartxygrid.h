/* © Pusterla Fabio 2011 */

#ifndef CHARTXYGRID_H
  #define CHARTXYGRID_H

  #include <QObject>
  #include <QColor>
  #include "chartxygridtick.h"

  class ChartXYGrid: public QObject
    {
      Q_OBJECT
    public:
      explicit ChartXYGrid(QObject *parent=0);

      bool visible(void) const;
      void setVisible(bool visible);

      unsigned int border(void) const;
      void setBorder(unsigned int border);

      QColor backgroundColor(void) const;
      void setBackgroundColor(const QColor &color);

      QColor foregroundColor(void) const;
      void setForegroundColor(const QColor &color);

      ChartXYGridTick &verticalTick(void);
      ChartXYGridTick &horizzontalTick(void);
    signals:

    public slots:

    private:
      bool m_visible;
      unsigned int m_border; // C
      QColor m_background_color;
      QColor m_foreground_color;
      ChartXYGridTick *m_vertical_tick;
      ChartXYGridTick *m_horizzontal_tick;
    };

#endif // CHARTXYGRID_H

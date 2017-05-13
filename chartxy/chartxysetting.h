/* © Pusterla Fabio 2011 */

#ifndef CHARTXYSETTING_H
  #define CHARTXYSETTING_H

  #include <QObject>
  #include "chartxyborder.h"
  #include "chartxygrid.h"
  #include "chartxyscale.h"

  class ChartXYSetting: public QObject
    {
      Q_OBJECT
    public:
      explicit ChartXYSetting(QObject *parent=0);

      ChartXYBorder &border(void);
      ChartXYGrid &grid(void);
      ChartXYScale &scale(void);
    signals:

    public slots:

    private:
      ChartXYBorder *m_border;
      ChartXYGrid *m_grid;
      ChartXYScale *m_scale;
    };

#endif // CHARTXYSETTING_H

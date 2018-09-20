/* © Pusterla Fabio 2011 */

#ifndef CHARTXYBORDER_H
  #define CHARTXYBORDER_H

  #include <QObject>
  #include <QColor>

  class ChartXYBorder: public QObject
    {
      Q_OBJECT
    public:
      explicit ChartXYBorder(QObject *parent=0);

      unsigned int top(void) const;
      unsigned int right(void) const;
      unsigned int bottom(void) const;
      unsigned int left(void) const;

      void setTop(unsigned int value);
      void setRight(unsigned int value);
      void setBottom(unsigned int value);
      void setLeft(unsigned int value);

      QColor backgroundColor(void) const;
      void setBackgroundColor(const QColor &color);

    signals:

    public slots:

    private:
      unsigned int m_top;
      unsigned int m_right;
      unsigned int m_bottom;
      unsigned int m_left;
      QColor m_background_color;
  };

#endif // CHARTXYBORDER_H

/* © Pusterla Fabio 2011 */

#ifndef CHARTXY_H
  #define CHARTXY_H

  #include <QAbstractItemView>
  #include <QMouseEvent>
  #include "chartxysetting.h"
  #include "chartxyrender.h"
  #include <qglobal.h>
  #include <QtCore>

  #define CHARTXY_VERSION                                   0
  #define CHARTXY_REVISION                                  8

  class ChartXY: public QAbstractItemView
    {
      Q_OBJECT
    public:
      explicit ChartXY(QWidget *parent=0);

      virtual QRect	visualRect(const QModelIndex &index) const;
      virtual void scrollTo(const QModelIndex &index,ScrollHint hint=EnsureVisible);
      virtual QModelIndex indexAt(const QPoint &point) const;

      ChartXYSetting &setting(void);

      static unsigned int version(void);
      static unsigned int revision(void);
    protected:
      virtual QModelIndex moveCursor(CursorAction cursorAction,Qt::KeyboardModifiers modifiers);
      virtual int horizontalOffset() const;
      virtual int verticalOffset() const;
      virtual bool isIndexHidden(const QModelIndex &index) const;
      virtual void setSelection(const QRect &rect,QItemSelectionModel::SelectionFlags command);
      virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

      virtual void dataChanged(const QModelIndex &topLeft,const QModelIndex &bottomRight);

      virtual void paintEvent(QPaintEvent *event);
      virtual void resizeEvent(QResizeEvent *event);
      virtual void scrollContentsBy(int dx, int dy);

      virtual void mousePressEvent(QMouseEvent *event);
      virtual void mouseReleaseEvent(QMouseEvent *event);

    signals:

    public slots:
      void updateChart(void);
    private:
      ChartXYSetting *m_setting;
      ChartXYRender *m_render;
    };

#endif // CHARTXY_H

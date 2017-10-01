#ifndef TSUTREEWIDGET_H
#define TSUTREEWIDGET_H

#include <QObject>
#include <QtGui>
#include <QTreeWidget>

class tsuTreeWidget : public QTreeWidget
{
public:
    explicit tsuTreeWidget(QWidget* parent);
    ~tsuTreeWidget();

public slots:
  void showDirectory(QTreeWidgetItem *item, int column);
  void expandDirectory(QTreeWidgetItem *item);

private:
  QTreeWidgetItem *p_headerItem;

  void loadDriveStructure(const QString drive);
  void addChildren(QTreeWidgetItem* item, QString filePath);

  QString convertSize(const int &size);
  QString convertSizeUnit(const int &size);
};

#endif // TSUTREEWIDGET_H

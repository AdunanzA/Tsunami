#include "tsutreewidget.h"

tsuTreeWidget::tsuTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setAnimated(true);
    this->setStyleSheet("background-color: rgb(23, 23, 23);");

    p_headerItem = new QTreeWidgetItem();
    p_headerItem->setText(0,QString("File Name"));
//    p_headerItem->setText(1,QString("Size (Bytes)"));
//    p_headerItem->setText(1,QString("Path"));
    p_headerItem->setTextColor(0, Qt::black);
//    p_headerItem->setTextColor(1, Qt::black);
//    p_headerItem->setTextColor(2, Qt::black);
    this->setHeaderItem(p_headerItem);

    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            if (!storage.isReadOnly()) {
                loadDriveStructure(storage.rootPath());
            }
        }
    }

    connect(this, &tsuTreeWidget::itemExpanded, this, &tsuTreeWidget::expandItem);
    connect(this, &tsuTreeWidget::itemClicked, this, &tsuTreeWidget::showDirectory);
}

tsuTreeWidget::~tsuTreeWidget()
{
    qDebug("tsuTreeWidget destroyed");
}

void tsuTreeWidget::showDirectory(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if (item->childCount() > 0) return;

    QDir* rootDir = new QDir(item->text(1));
    QFileInfoList filesList = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filesList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;

//        QTreeWidgetItem* child = new QTreeWidgetItem();
//        child->setText(0,fileInfo.fileName());

//        if(fileInfo.isFile())
//        {
//            child->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
//            child->setText(1, QString("%0%1").arg(convertSize(fileInfo.size())).arg(convertSizeUnit(fileInfo.size())));
//            child->setIcon(0,*(new QIcon(":/images/file-doc.png")));
//        }

        if(fileInfo.isDir())
        {
            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setText(0,fileInfo.fileName());
            child->setText(1,fileInfo.filePath());
            child->setCheckState(0, Qt::Unchecked);
            child->setFlags(child->flags() | Qt::ItemIsAutoTristate);
            child->setIcon(0,*(new QIcon(":/images/folder.png")));
            item->addChild(child);
        }

//        item->addChild(child);

        resizeColumnToContents(0);
    }
}

void tsuTreeWidget::expandDirectory(QTreeWidgetItem *item)
{
    showDirectory(item, 0);
}

void tsuTreeWidget::loadDriveStructure(const QString drive)
{
    QTreeWidgetItem* driveItem = new QTreeWidgetItem();
    driveItem->setIcon(0, *(new QIcon(":/images/drive.png")));
    driveItem->setText(0, drive);
    this->addTopLevelItem(driveItem);

    QDir *rootDir = new QDir(drive);
    rootDir->setSorting(QDir::Name);

    QFileInfoList filesList = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filesList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;

//        QTreeWidgetItem* item = new QTreeWidgetItem();
//        item->setText(0,fileInfo.fileName());

//        if(fileInfo.isFile())
//        {
//            item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
//            item->setIcon(0,*(new QIcon(":/images/file-doc.png")));
//            item->setText(1, QString("%0%1").arg(convertSize(fileInfo.size())).arg(convertSizeUnit(fileInfo.size())));
//        }

        if(fileInfo.isDir())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0,fileInfo.fileName());
            item->setText(1,fileInfo.filePath());
            item->setCheckState(0, Qt::Unchecked);
            item->setFlags(item->flags() | Qt::ItemIsAutoTristate);
            item->setIcon(0,*(new QIcon(":/images/folder.png")));
            addChildren(item,fileInfo.filePath());
            driveItem->addChild(item);
        }

//        item->setText(2,fileInfo.filePath());
//        driveItem->addChild(item);
    }

}

void tsuTreeWidget::addChildren(QTreeWidgetItem *item, QString filePath)
{
    QDir* rootDir = new QDir(filePath);
    QFileInfoList filesList = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filesList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;

//        QTreeWidgetItem* child = new QTreeWidgetItem();
//        child->setText(0,fileInfo.fileName());

//        if(fileInfo.isFile())
//        {
//            child->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
//            child->setIcon(0,*(new QIcon(":/images/file-doc.png")));
//            child->setText(1, QString("%0%1").arg(convertSize(fileInfo.size())).arg(convertSizeUnit(fileInfo.size())));
//        }

        if(fileInfo.isDir())
        {
            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setText(0,fileInfo.fileName());
            child->setText(1,fileInfo.filePath());
            child->setCheckState(0, Qt::Unchecked);
            child->setFlags(child->flags() | Qt::ItemIsAutoTristate);
            child->setIcon(0,*(new QIcon(":/images/folder.png")));
            item->addChild(child);
        }

//        item->addChild(child);
    }
}

QString tsuTreeWidget::convertSize(const int &size)
{
    if (size==0) return "0";
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("b");

    while(num >= 1000.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1000.0;
    }

    int length = 1;
    int x = num;
    while ( x /= 10 )
       length++;

    return QString().setNum(num,'f',3-length);
}

QString tsuTreeWidget::convertSizeUnit(const int &size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("b");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return unit;
}

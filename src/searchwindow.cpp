#include "searchwindow.h"
#include "ui_searchwindow.h"

searchwindow::searchwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::searchwindow)
{
    ui->setupUi(this);

    // setting default tsunami script folder
    QString localTsunami = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation); // win -> C:\Users\user\AppData\Local\Tsunami
    localTsunami = QString("%0/%1").arg(localTsunami).arg("script");
    p_tsunamiScriptFolder = QDir::toNativeSeparators(localTsunami);

//    if (!QDir(p_tsunamiScriptFolder).exists()) {
//        if (QDir().mkpath(p_tsunamiScriptFolder)) {
//            qDebug() << "created" << p_tsunamiScriptFolder;
//            // download scripts
//        } else {
//            qWarning() << "cannot create" << p_tsunamiScriptFolder;
//        }
//    } else {
//        // load scripts
//        // check for updated scripts
//        qDebug() << "using" << p_tsunamiScriptFolder;
//    }

    // setting default tsunami cache folder
    localTsunami = QString("%0/%1").arg(localTsunami).arg("cache");
    p_tsunamiCacheFolder = QDir::toNativeSeparators(localTsunami);

//    if (!QDir(p_tsunamiCacheFolder).exists()) {
//        if (QDir().mkpath(p_tsunamiCacheFolder)) {
//            qDebug() << "created" << p_tsunamiCacheFolder;
//        } else {
//            qWarning() << "cannot create" << p_tsunamiCacheFolder;
//        }
//    } else {
//        qDebug() << "using" << p_tsunamiCacheFolder;
//    }

    p_crawler = new tsuCrawler();
    p_crawler->setScriptsPath(p_tsunamiScriptFolder);
    p_crawler->setCachePath(p_tsunamiCacheFolder);

    p_crawler_thread = new QThread();
    p_crawler->moveToThread(p_crawler_thread);

    connect(p_crawler, &tsuCrawler::providerAdded, this, &searchwindow::providerAdded);
    connect(p_crawler, &tsuCrawler::providerNotValid, this, &searchwindow::providerNotValid);
    connect(p_crawler, &tsuCrawler::itemFound, this, &searchwindow::itemFound);
    connect(p_crawler, &tsuCrawler::finishedSearch, this, &searchwindow::finishedSearch);
//    connect(p_crawler, &tsuCrawler::iconDownloaded, this, &searchwindow::iconDownloaded);

    connect(this, &searchwindow::loadProviders , p_crawler, &tsuCrawler::loadProviders);
//    connect(this, &searchwindow::providerNeedIconDownload, p_crawler, &tsuCrawler::providerNeedIconDownload);
//    connect(this, &searchwindow::changeProviderActivation, p_crawler, &tsuCrawler::changeProviderActivation);
    connect(this, &searchwindow::cancelSearch, p_crawler, &tsuCrawler::requestedCancelSearch);

    connect(ui->tableResults, &QTableWidget::cellClicked, this, &searchwindow::cellClicked);

    connect(ui->txtSearch, &QLineEdit::returnPressed, this, &searchwindow::txtSearch_returnPressed);

    ui->tableResults->setColumnCount(12);
    ui->tableResults->hideColumn(tableColumns::Category);
    ui->tableResults->hideColumn(tableColumns::Provider);
    ui->tableResults->hideColumn(tableColumns::Hash);
    ui->tableResults->hideColumn(tableColumns::Link);
    ui->tableResults->hideColumn(tableColumns::Magnet);

    QHeaderView *headerView = new QHeaderView(Qt::Horizontal, ui->tableResults);
    ui->tableResults->setHorizontalHeader(headerView);
    headerView->setSectionResizeMode(tableColumns::Name, QHeaderView::Stretch); // first column (name) stretch to fill table size
    headerView->setVisible(false);

    QIcon sortIcon = QIcon();
    sortIcon.addPixmap(QPixmap(":/images/draw-arrow-down.png"), QIcon::Normal, QIcon::Off);
    sortIcon.addPixmap(QPixmap(":/images/draw-arrow-up.png"), QIcon::Normal, QIcon::On);
    ui->btnOrderDirection->setIcon(sortIcon);

    // TO DO: read from config
    p_orderBy = tableColumns::Seeds;
    p_sortOrder = Qt::DescendingOrder;
    ui->btnOrderDirection->setChecked(p_sortOrder == Qt::DescendingOrder);
    sortTable();

    p_crawler_thread->start();
    emit loadProviders();
}

searchwindow::~searchwindow()
{
    delete ui;
    p_crawler_thread->deleteLater();
    qDebug("searchwindow destroyed");
}

void searchwindow::providerAdded(const tsuProvider *item)
{
    qDebug() << "provider" << item->getName() << "loaded";
    if (p_crawler->getProvidersCount() > 0) {
        // enable search
        ui->btnSearch->setEnabled(true);
    }
    ui->lblResultsCount->setText(QString("%0 providers loaded").arg(p_crawler->getProvidersCount()));
}

void searchwindow::providerNotValid(const QString providerName)
{
    qDebug() << "provider" << providerName << "not valid";
    if (p_crawler->getProvidersCount() == 0) {
        // disable search
        ui->btnSearch->setEnabled(false);
    }
    ui->lblResultsCount->setText(QString("%0 providers loaded").arg(p_crawler->getProvidersCount()));
}

void searchwindow::itemFound(const tsuProvider::searchItem item)
{
    QString itemKey;
    if (!item.hash.isNull() && !item.hash.isEmpty()) {
        itemKey = item.hash;
    } else {
        itemKey = item.name;
    }
    int countSeeds = item.seeds.toInt();
    int countLeeches = item.leeches.toInt();

    QTableWidgetItem *itemName = new QTableWidgetItem;
    itemName->setData(Qt::EditRole, item.name);

    QTableWidgetItem *itemDate = new QTableWidgetItem;
    uint unixTime = item.date.left(10).toUInt();
    if (unixTime == 0) {
        itemDate->setData(Qt::EditRole, "");
    } else {
        QDateTime dateItem;
        dateItem.setSecsSinceEpoch(unixTime);
//        itemDate->setData(Qt::EditRole, dateItem.toUTC());
        itemDate->setData(Qt::EditRole, dateItem.toString(Qt::DateFormat::SystemLocaleShortDate));
    }

    QTableWidgetItem *itemSeeds = new QTableWidgetItem;
    if (countSeeds == 0) {
        itemSeeds->setData(Qt::EditRole, "");
    } else {
        itemSeeds->setData(Qt::EditRole, item.seeds.toInt());
        itemSeeds->setForeground(QColor(205, 205, 205));
    }

    QTableWidgetItem *itemLeeches = new QTableWidgetItem;
    if (countLeeches == 0) {
        itemLeeches->setData(Qt::EditRole, "");
    } else {
        itemLeeches->setData(Qt::EditRole, item.leeches.toInt());
    }

    QTableWidgetItem *itemSize = new QTableWidgetItem;
    itemSize->setData(Qt::DisplayRole, item.size);
    itemSize->setForeground(QColor(205, 205, 205));

    QTableWidgetItem *itemCategory = new QTableWidgetItem;
    itemCategory->setData(Qt::EditRole, item.category);

    QTableWidgetItem *itemProvider = new QTableWidgetItem;
    itemProvider->setData(Qt::EditRole, item.provider);

    QTableWidgetItem *itemHash = new QTableWidgetItem;
    itemHash->setData(Qt::EditRole, item.hash);

    QTableWidgetItem *itemLink = new QTableWidgetItem;
    itemLink->setData(Qt::EditRole, item.link);

    QTableWidgetItem *itemMagnet = new QTableWidgetItem;
    itemMagnet->setData(Qt::EditRole, item.magnet);

    int itemIndex = 0;

    for (int i=0; i<ui->tableResults->rowCount(); i++)
    {
        if (ui->tableResults->item(i, 7)->text() == itemKey || ui->tableResults->item(i, 0)->text() == itemKey) {
            // update
            itemIndex = i;

            if (!ui->tableResults->item(itemIndex, tableColumns::Name)->text().isEmpty() && itemName->text().isEmpty())
                itemName->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Name)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Eta)->text().isEmpty() && itemDate->text().isEmpty())
                itemDate->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Eta)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Seeds)->text().isEmpty() && itemSeeds->text().isEmpty())
                itemSeeds->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Seeds)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Leeches)->text().isEmpty() && itemLeeches->text().isEmpty())
                itemLeeches->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Leeches)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Size)->text().isEmpty() && itemSize->text().isEmpty())
                itemSize->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Size)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Category)->text().isEmpty() && itemCategory->text().isEmpty())
                itemCategory->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Category)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Provider)->text().isEmpty() && itemProvider->text().isEmpty())
                itemProvider->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Provider)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Hash)->text().isEmpty() && itemHash->text().isEmpty())
                itemHash->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Hash)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Link)->text().isEmpty() && itemLink->text().isEmpty())
                itemLink->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Link)->data(Qt::EditRole));

            if (!ui->tableResults->item(itemIndex, tableColumns::Magnet)->text().isEmpty() && itemMagnet->text().isEmpty())
                itemMagnet->setData(Qt::EditRole, ui->tableResults->item(itemIndex, tableColumns::Magnet)->data(Qt::EditRole));

            break;
        }
    }

    if (itemIndex == 0) {
        // add
        itemIndex = ui->tableResults->rowCount();
        ui->tableResults->insertRow(ui->tableResults->rowCount());
    }

    ui->tableResults->setItem(itemIndex, tableColumns::Name, itemName);
    ui->tableResults->setItem(itemIndex, tableColumns::Eta, itemDate);
    ui->tableResults->setItem(itemIndex, tableColumns::Seeds, itemSeeds);
    ui->tableResults->setItem(itemIndex, tableColumns::Leeches, itemLeeches);
    ui->tableResults->setItem(itemIndex, tableColumns::Size, itemSize);
    ui->tableResults->setItem(itemIndex, tableColumns::Category, itemCategory);
    ui->tableResults->setItem(itemIndex, tableColumns::Provider, itemProvider);
    ui->tableResults->setItem(itemIndex, tableColumns::Hash, itemHash);
    ui->tableResults->setItem(itemIndex, tableColumns::Link, itemLink);
    ui->tableResults->setItem(itemIndex, tableColumns::Magnet, itemMagnet);

    if (!item.link.isEmpty()) {
        QTableWidgetItem *icon_item = new QTableWidgetItem;
        QIcon icon(":/images/homepage.png");
        icon_item->setData(Qt::DecorationRole, icon);
        icon_item->setToolTip("Open link in " + item.provider);
        ui->tableResults->setItem(itemIndex, tableColumns::OpenLink, icon_item);
    }

    if (!item.magnet.isEmpty() || !item.hash.isEmpty()) {
        QTableWidgetItem *download_item = new QTableWidgetItem;
        QIcon icon(":/images/download.png");
        download_item->setData(Qt::DecorationRole, icon);
        download_item->setToolTip("Download this file!");
        ui->tableResults->setItem(itemIndex, tableColumns::OpenMagnet, download_item);
    }

    ui->tableResults->setColumnWidth(tableColumns::OpenLink, 22);
    ui->tableResults->setColumnWidth(tableColumns::OpenMagnet, 22);
    ui->tableResults->resizeColumnToContents(tableColumns::Eta);
    ui->tableResults->resizeColumnToContents(tableColumns::Seeds);
    ui->tableResults->resizeColumnToContents(tableColumns::Leeches);
    ui->tableResults->resizeColumnToContents(tableColumns::Size);

    ui->tableResults->hideColumn(tableColumns::Category);
    ui->tableResults->hideColumn(tableColumns::Provider);
    ui->tableResults->hideColumn(tableColumns::Hash);
    ui->tableResults->hideColumn(tableColumns::Link);
    ui->tableResults->hideColumn(tableColumns::Magnet);

    emit web_itemFound(item);
}

void searchwindow::finishedSearch(int itemsFound, qint64 elapsed)
{
    ui->btnSearch->setEnabled(true);
    ui->btnSearch->setText("Search");
    ui->txtSearch->setEnabled(true);
    ui->tableResults->setSortingEnabled(true);
    QString msg = QString("%0 results in %1 seconds using %2 providers").arg(itemsFound)
                            .arg(QDateTime::fromMSecsSinceEpoch(elapsed).toUTC().toString("ss"))
                            .arg(p_crawler->getProvidersCount());
    ui->lblResultsCount->setText(msg);
    qInfo() << msg;
    ui->tableResults->setColumnWidth(tableColumns::OpenLink, 22);
    ui->tableResults->setColumnWidth(tableColumns::OpenMagnet, 22);
    ui->tableResults->resizeColumnToContents(tableColumns::Eta);
    ui->tableResults->resizeColumnToContents(tableColumns::Seeds);
    ui->tableResults->resizeColumnToContents(tableColumns::Leeches);
    ui->tableResults->resizeColumnToContents(tableColumns::Size);

    emit web_finishedSearch(itemsFound, elapsed, p_crawler->getProvidersCount());
}

void searchwindow::cellClicked(int row, int column)
{
    ui->tableResults->item(row, column)->setSelected(false);

    if (column == tableColumns::OpenLink) {
        QString link = ui->tableResults->item(row, tableColumns::Link)->text();
        QDesktopServices::openUrl(QUrl(link));
    }

    if (column == tableColumns::OpenMagnet) {
        QString magnet = ui->tableResults->item(row, tableColumns::Magnet)->text();
        QString hash = ui->tableResults->item(row, tableColumns::Hash)->text();

        if (magnet.isEmpty() && !hash.isEmpty()) {
            magnet = QString("%0%1").arg("magnet:?xt=urn:btih:").arg(hash);
        }

        emit downloadFromMagnet(magnet);
    }
}

void searchwindow::searchRequestFromWeb(const QString textToSearch, const int category)
{
//    tsuProvider::categories cat = static_cast<tsuProvider::categories>(category);

//    // defaults, maybe from web in the future?
//    int resultsWanted = 50;
//    tsuProvider::sortRules sort = tsuProvider::sortRules::seeds_d;

//    p_crawler->search(textToSearch, cat, resultsWanted, sort);
    ui->txtSearch->setText(textToSearch);
    switch (category) {
    case 0:
        ui->radioAll->setChecked(true);
        break;
    case 1:
        ui->radioVideo->setChecked(true);
        break;
    case 2:
        ui->radioAudio->setChecked(true);
        break;
    case 3:
        ui->radioApps->setChecked(true);
        break;
    case 4:
        ui->radioGames->setChecked(true);
        break;
    case 5:
        ui->radioPorn->setChecked(true);
        break;
    case 6:
        ui->radioOther->setChecked(true);
        break;
    default:
        break;
    }
    on_btnSearch_released();
}

void searchwindow::on_btnSearch_released()
{
    if (ui->btnSearch->text() == "Search") {
        ui->btnSearch->setText("CANCEL");
        ui->txtSearch->setEnabled(false);

        ui->tableResults->clear();
        ui->tableResults->setRowCount(0);
        ui->tableResults->setSortingEnabled(false);

        QString textToSearch = ui->txtSearch->text();
        tsuProvider::categories wantedCategory = tsuProvider::categories::all;// (tsuProvider::categories)ui->cmbCategories->currentData().toInt();
        if (ui->radioApps->isChecked()) wantedCategory = tsuProvider::categories::apps;
        if (ui->radioAudio->isChecked()) wantedCategory = tsuProvider::categories::audio;
        if (ui->radioGames->isChecked()) wantedCategory = tsuProvider::categories::games;
        if (ui->radioOther->isChecked()) wantedCategory = tsuProvider::categories::other;
        if (ui->radioPorn->isChecked()) wantedCategory = tsuProvider::categories::porn;
        if (ui->radioVideo->isChecked()) wantedCategory = tsuProvider::categories::video;
        int resultsWanted = 50;//ui->spinResultsWanted->value();
        tsuProvider::sortRules sort = tsuProvider::sortRules::seeds_d;// (tsuProvider::sortRules)ui->cmbSort->currentData().toInt();

        ui->lblResultsCount->setText(QString("Searching '%0'...").arg(textToSearch));

        p_crawler->search(textToSearch, wantedCategory, resultsWanted, sort);

    } else {
        ui->btnSearch->setText("Search");
        ui->btnSearch->setEnabled(false);
        emit cancelSearch();
    }
}

void searchwindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
}

void searchwindow::on_btnOrderName_released()
{
    p_orderBy = tableColumns::Name;
    sortTable();
}

void searchwindow::on_btnOrderSeeds_released()
{
    p_orderBy = tableColumns::Seeds;
    sortTable();
}

void searchwindow::on_btnOrderDirection_released()
{
    p_sortOrder = (ui->btnOrderDirection->isChecked()) ? Qt::DescendingOrder : Qt::AscendingOrder;
    sortTable();
}

void searchwindow::sortTable()
{
    ui->tableResults->sortByColumn(p_orderBy, p_sortOrder);
    switch (p_orderBy) {
    case tableColumns::Name:
        ui->btnOrderName->setStyleSheet("color:#AAAAAA");
        ui->btnOrderSeeds->setStyleSheet("color: #606060");
        break;
    case tableColumns::Seeds:
        ui->btnOrderName->setStyleSheet("color:#606060");
        ui->btnOrderSeeds->setStyleSheet("color: #AAAAAA");
        break;
    default:
        break;
    }
}

void searchwindow::txtSearch_returnPressed()
{
    ui->btnSearch->released();
}

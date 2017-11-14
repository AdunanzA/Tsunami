#include "tsucrawler.h"

//tsuCrawler::tsuCrawler(QObject *parent) : QObject(parent)
//{
//    p_providerList = new QList<tsuProvider*>();
//}

tsuCrawler::tsuCrawler()
{
    qDebug() << "created";
}

tsuCrawler::~tsuCrawler()
{
    for (tsuProvider *provider : p_providerList) {
        provider->deleteLater();
    }
    p_providerList.clear();
    qDebug() << "destroyed";
}

void tsuCrawler::loadProviders()
{
    for (tsuProvider* provider : p_providerList) {
        provider->deleteLater();
    }
    p_providerList.clear();

    QDirIterator it(p_scriptsPath, QStringList() << "*.js",  QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
//        qDebug() << "found" << it.next();

        QFile scriptFile(it.next());
        scriptFile.open(QIODevice::ReadOnly);
        QTextStream stream(&scriptFile);
        QString contents = stream.readAll();
        scriptFile.close();

        QString providerName = QDir::toNativeSeparators(scriptFile.fileName()).replace(p_scriptsPath, "")
                                                                              .replace(QDir::separator(), "")
                                                                              .replace(".js", "");
        tsuProvider *newProvider = new tsuProvider(this);
        newProvider->setName(providerName);
        newProvider->setScriptFile(contents);

        connect(this, &tsuCrawler::searchRequested, newProvider, &tsuProvider::search);
        connect(this, &tsuCrawler::cancelSearch, newProvider, &tsuProvider::cancelSearch);
        connect(newProvider, &tsuProvider::itemFound, this, &tsuCrawler::providerItemFound);
        connect(newProvider, &tsuProvider::iconDownloaded, this, &tsuCrawler::providerIconDownloaded);
        connect(newProvider, &tsuProvider::finished, this, &tsuCrawler::providerFinishedSearch);

        p_providerList.append(newProvider);

        emit providerAdded(newProvider);
    }

}

void tsuCrawler::providerNeedIconDownload(QString providerName)
{
    for (tsuProvider *item : p_providerList) {
        if (item->getName() == providerName) {
            item->loadIcon();
            break;
        }
    }
}

void tsuCrawler::search(const QString textToSearch, const tsuProvider::categories category,
                        const int resultsWantedPerProvider, const tsuProvider::sortRules sortToApply)
{
    p_itemsFoundCount = 0;
    p_timer.start();
    QMetaEnum metaEnumCat = QMetaEnum::fromType<tsuProvider::categories>();
    QMetaEnum metaEnumSor = QMetaEnum::fromType<tsuProvider::sortRules>();

    qDebug() << "starting search for" << textToSearch << "in category" << metaEnumCat.valueToKey(category) << "for" << resultsWantedPerProvider << "results requested sorted by" << metaEnumSor.valueToKey(sortToApply);

    emit searchRequested(textToSearch, category, resultsWantedPerProvider, sortToApply);

//    const auto& arg1 = textToSearch;
//    const auto& arg2 = category;
//    QtConcurrent::map(p_providerList->constBegin(), p_providerList->constEnd(), [&arg1, &arg2](tsuProvider *provider) {
//        provider->search(arg1, arg2);
//    });
}

void tsuCrawler::providerItemFound(const tsuProvider::searchItem item)
{
    ++p_itemsFoundCount;
    emit itemFound(item);
}

void tsuCrawler::providerFinishedSearch(const QString providerName)
{
    qDebug() << providerName << "finished";
    int active = 0;
    for (tsuProvider* provider : p_providerList) {
        if (!provider->isFinished() && provider->isValid() && provider->isActive())
        {
            ++active;
        }
        if (provider->getName() == providerName && !provider->isValid()) {
            qDebug() << providerName << "finished but invalid";
            emit providerNotValid(providerName);
            --active;
        }
    }

    if (active > 0) {
        qDebug() << "awaiting" << active << "providers to finish";
    } else {
        qint64 elapsed = p_timer.elapsed();
        emit finishedSearch(p_itemsFoundCount, elapsed);
    }
}

void tsuCrawler::providerIconDownloaded(const QString providerName, const QPixmap icon)
{
    emit iconDownloaded(providerName, icon);
}

void tsuCrawler::changeProviderActivation(QString providerName, bool active)
{
    for (tsuProvider *provider : p_providerList) {
        if (provider->getName() == providerName) {
            provider->setActive(active);
            qDebug() << providerName << "set active" << ((active) ? "true" : "false");
            break;
        }
    }
}

void tsuCrawler::requestedCancelSearch()
{
    emit cancelSearch();
}

QString tsuCrawler::getCachePath() const
{
    return p_cachePath;
}

void tsuCrawler::setCachePath(const QString &value)
{
    p_cachePath = value;
}

int tsuCrawler::getProvidersCount()
{
    return p_providerList.count();
}

QString tsuCrawler::getScriptsPath() const
{
    return p_scriptsPath;
}

void tsuCrawler::setScriptsPath(const QString &value)
{
    p_scriptsPath = value;
}

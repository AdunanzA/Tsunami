#ifndef TSUCRAWLER_H
#define TSUCRAWLER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDirIterator>
#include <QElapsedTimer>

#include <src/tsuCrawler/tsuprovider.h>

class tsuCrawler : public QObject
{
    Q_OBJECT
public:
//    explicit tsuCrawler(QObject *parent = 0);
    explicit tsuCrawler();
    ~tsuCrawler();

    void search(const QString textToSearch,
                const tsuProvider::categories category,
                const int resultsWantedPerProvider,
                const tsuProvider::sortRules sortToApply = tsuProvider::sortRules::siteDefault);

    QString getScriptsPath() const;
    void setScriptsPath(const QString &value);

    QString getCachePath() const;
    void setCachePath(const QString &value);

    int getProvidersCount();

signals:
    void providerAdded(const tsuProvider *item);
    void providerNotValid(const QString providerName);
    void searchRequested(const QString textToSearch, const tsuProvider::categories category, const int resultsWanted, const tsuProvider::sortRules sortToApply);

    void itemFound(const tsuProvider::searchItem item);
    void finishedSearch(int itemsFound, qint64 elapsed);
    void iconDownloaded(const QString providerName, const QPixmap icon);
    void cancelSearch();

public slots:
    void loadProviders();
    void providerNeedIconDownload(QString providerName);
    void providerItemFound(const tsuProvider::searchItem item);
    void providerFinishedSearch(const QString providerName);
    void providerIconDownloaded(const QString providerName, const QPixmap icon);
    void changeProviderActivation(QString providerName, bool active);

    void requestedCancelSearch();

private:
    QList<tsuProvider*> p_providerList;
    QString p_scriptsPath;
    QString p_cachePath;
    QElapsedTimer p_timer;

    int p_itemsFoundCount{};

private slots:

};
#endif // TSUCRAWLER_H

#ifndef TSUPROVIDER_H
#define TSUPROVIDER_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScriptValueIterator>
#include <QMetaEnum>
#include <QIcon>
#include <QStyle>
#include <QApplication>
#include <iostream>

#include <tsuCrawler/replytimeout.h>

class tsuProvider : public QObject
{
    Q_OBJECT

public:
    explicit tsuProvider(QObject *parent = 0);
    ~tsuProvider();

    QString getName() const;
    void setName(const QString &value);

    QString getScriptFile() const;
    void setScriptFile(const QString &value);

    struct searchItem {
        QString provider;
        QString name;
        QString category;
        QString link;
        QString size;
        QString hash;
        QString date;
        QString seeds;
        QString leeches;
        QString magnet;
    };
    enum categories {
        all,
        video,
        audio,
        apps,
        games,
        porn,
        other
    };
    Q_ENUM(categories)

    enum sortRules {
        siteDefault,
        name_a,
        name_d,
        size_a,
        size_d,
        date_a,
        date_d,
        seeds_a,
        seeds_d,
        leech_a,
        leech_d
    };
    Q_ENUM(sortRules)

    void loadIcon() const;

    bool isFinished() const;
    void setFinished(bool value);

    bool isValid() const;

    bool isActive() const;
    void setActive(bool value);

    int getResultsFounds() const;

signals:
    void itemFound(const tsuProvider::searchItem item);
    void iconDownloaded(const QString providerName, const QPixmap icon);
    void finished(const QString providerName);

public slots:
    void search(const QString text, const tsuProvider::categories cat, const int resultsWantedPerProvider, const tsuProvider::sortRules sortToApply);
    void cancelSearch();

private:
    QScriptEngine *p_scriptEngine;
    QNetworkAccessManager *p_namPage;
    QNetworkAccessManager *p_namIcon;

    QNetworkReply *p_reply;

    QString p_name;
    QString p_version;
    QString p_scriptFile;
    QHash<int, QString> p_urls;
    int p_actualUrlIndex;
    QHash<categories, QString> p_searchPaths;
    QHash<QString, categories> p_siteCategory;
    int p_startPage;
    int p_actualPage;
    QPixmap *p_siteIcon;
    QUrl p_siteIconUrl;
    int p_timeoutMilliseconds;
    int p_resultsWanted;
    int p_resultsFounds;
    int p_resultsPerPage;
    QString p_stringToSearch;
    categories p_categoryToUse;
    QHash<sortRules, QString> p_sortRules;
    sortRules p_sortToApply;

    bool p_finished;
    bool p_valid;
    bool p_active;

    void doSearch();
    void processPage();
    QList<QString> splitRows();
    QList<QString> splitColumns(const QString row);
    tsuProvider::searchItem parseColumn(const QList<QString> columns);

private slots:
    void downloadPageFinished(QNetworkReply *reply);
    void downloadIconFinished(QNetworkReply *reply);

};
//Q_DECLARE_METATYPE(tsuProvider)
Q_DECLARE_METATYPE(tsuProvider::searchItem)
#endif // TSUPROVIDER_H

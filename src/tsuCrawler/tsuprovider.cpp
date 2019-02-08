#include "tsuprovider.h"

static QScriptValue extract(QScriptContext *context, QScriptEngine *engine) {
    Q_UNUSED(engine);
    QString result = "";

    // check of parameters

    QString fromText = context->argument(0).toString();
    QString startString = context->argument(1).toString();
    QString endString = context->argument(2).toString();
    qint32  startingFrom = context->argument(3).toInt32();

//    qDebug() << QString("requested extract for %0 chars, from '%1' to '%2' starting from %3").arg(fromText.length()).arg(startString).arg(endString).arg(startingFrom);

    int startIndex = fromText.indexOf(startString, startingFrom, Qt::CaseInsensitive);
    if (startIndex == -1) {
        startIndex = 0;
    } else {
        startIndex += startString.length();
    }

    int endIndex = fromText.indexOf(endString, startIndex, Qt::CaseInsensitive);
    if (endIndex == -1) {
        endIndex = fromText.length();
    } else {
        endIndex -= startIndex;
    }

    result = fromText.mid(startIndex, endIndex);

//    qDebug() << QString("result in %0 chars from start index %1 and end index %2").arg(result.length()).arg(startIndex).arg(endIndex);

    return result;
}

tsuProvider::tsuProvider(QObject *parent) : QObject(parent)
{
    p_scriptEngine = new QScriptEngine(this);
    p_namPage = new QNetworkAccessManager(this);
    p_namIcon = new QNetworkAccessManager(this);
    p_siteIcon = new QPixmap();
    p_valid = true;
    p_active = true;
    p_actualUrlIndex = 0;

    connect(p_namPage, &QNetworkAccessManager::finished, this, &tsuProvider::downloadPageFinished);
    connect(p_namIcon, &QNetworkAccessManager::finished, this, &tsuProvider::downloadIconFinished);

    QScriptValue extractJSFunction = p_scriptEngine->newFunction(extract);
    p_scriptEngine->globalObject().setProperty("extract", extractJSFunction);
}

tsuProvider::~tsuProvider()
{
    p_scriptEngine->deleteLater();
//    p_reply->deleteLater();
    p_namPage->deleteLater();
    p_namIcon->deleteLater();

    p_scriptEngine = Q_NULLPTR;
//    p_reply = Q_NULLPTR;
    p_namPage = Q_NULLPTR;
    p_namIcon = Q_NULLPTR;
}

QString tsuProvider::getName() const
{
    return p_name;
}

void tsuProvider::setName(const QString &value)
{
    p_name = value;
}

QString tsuProvider::getScriptFile() const
{
    return p_scriptFile;
}

void tsuProvider::setScriptFile(const QString &value)
{
    p_scriptFile = value.simplified();
//    p_scriptFile = p_scriptFile.replace("\r", "").replace("\n", "").replace("\t", "");

    // TO DO: if someone doesn't evaluate here, provider is not valid (maybe for mandatory functions only?)
    try
    {
        QScriptValue evaluate = p_scriptEngine->evaluate(p_scriptFile, p_name);
        if (p_scriptEngine->uncaughtException().toBool()) {
            qWarning() << QString("uncaught exception at line %0: %1").arg(p_scriptEngine->uncaughtExceptionLineNumber()).arg(evaluate.toString());
            p_valid = false;
            setFinished(true);
            return;
        }

        // siteAddresses
        QScriptValue siteAdresses = p_scriptEngine->globalObject().property("siteAddresses");
        QScriptValue array = siteAdresses.call(QScriptValue());
        // iterate a returning array
        int length = array.property("length").toInteger();
        for(int i = 0; i < length; i++) {
//            qDebug() << "url #" << p_urls.size() <<  url.toString() << "added";
            p_urls.insert(p_urls.size(), array.property(i).toString());
        }

        // siteConfiguration
        QScriptValue siteConfiguration = p_scriptEngine->globalObject().property("siteConfiguration");
        QScriptValue hash = siteConfiguration.call(QScriptValue());
        // iterate a returning hashmap (key:value)
        QScriptValueIterator sc_iterator(hash);
        while (sc_iterator.hasNext()) {
            sc_iterator.next();
//            qDebug() << sc_iterator.name() << ":" << sc_iterator.value().toString();
            if (sc_iterator.name() == "timeoutMilliseconds")
                p_timeoutMilliseconds = sc_iterator.value().toInteger();

            if (sc_iterator.name() == "siteIcon") {
                QString iconUrl = sc_iterator.value().toString();
                if (!iconUrl.isEmpty()) {
                    if (iconUrl.left(4) != "http") {
                        iconUrl = QString("%0%1").arg(p_urls[0]).arg(iconUrl);
                    }
                    p_siteIconUrl = QUrl(iconUrl);
                }
            }

            if (sc_iterator.name() == "version")
                p_version = sc_iterator.value().toInteger();
        }

        // paginationRules
        QScriptValue startPage = p_scriptEngine->globalObject().property("paginationRules");
        hash = startPage.call(QScriptValue());
        // iterate a returning hashmap (key:value)
        QScriptValueIterator sp_iterator(hash);
        while (sp_iterator.hasNext()) {
            sp_iterator.next();
//            qDebug() << it.name() << ":" << it.value().toString();
            if (sp_iterator.name() == "startPage")
                p_startPage = sp_iterator.value().toInteger();

            if (sp_iterator.name() == "resultsPerPage")
                p_resultsPerPage = sp_iterator.value().toInteger();
        }

        // sortRules (optional)
        QScriptValue sortRulesPage = p_scriptEngine->globalObject().property("sortRules");
        hash = sortRulesPage.call(QScriptValue());
        // iterate a returning hashmap (key:value)
        QScriptValueIterator sr_iterator(hash);
        while (sr_iterator.hasNext()) {
            sr_iterator.next();
//            qDebug() << it.name() << ":" << it.value().toString();
            sortRules rule = sortRules::siteDefault;
            if (sr_iterator.name() == "name_asc") rule = sortRules::name_a;
            if (sr_iterator.name() == "name_desc") rule = sortRules::name_d;
            if (sr_iterator.name() == "size_asc") rule = sortRules::size_a;
            if (sr_iterator.name() == "size_desc") rule = sortRules::size_d;
            if (sr_iterator.name() == "date_asc") rule = sortRules::date_a;
            if (sr_iterator.name() == "date_desc") rule = sortRules::date_d;
            if (sr_iterator.name() == "seeds_asc") rule = sortRules::seeds_a;
            if (sr_iterator.name() == "seeds_desc") rule = sortRules::seeds_d;
            if (sr_iterator.name() == "leeches_asc") rule = sortRules::leech_a;
            if (sr_iterator.name() == "leeches_desc") rule = sortRules::leech_d;
            p_sortRules.insert(rule, sr_iterator.value().toString());
        }

        // siteSearchPaths
        QScriptValue searchPaths = p_scriptEngine->globalObject().property("siteSearchPaths");
        hash = searchPaths.call(QScriptValue());
        // iterate a returning hashmap (key:value)
        QScriptValueIterator spp_iterator(hash);
        while (spp_iterator.hasNext()) {
            spp_iterator.next();
//            qDebug() << it.name() << ":" << it.value().toString();
            categories key = categories::all;
            if (spp_iterator.name() == "video") key = categories::video;
            if (spp_iterator.name() == "audio") key = categories::audio;
            if (spp_iterator.name() == "apps") key = categories::apps;
            if (spp_iterator.name() == "games") key = categories::games;
            if (spp_iterator.name() == "porn") key = categories::porn;
            if (spp_iterator.name() == "other") key = categories::other;
            p_searchPaths.insert(key, spp_iterator.value().toString());
        }

        // siteCategories
        QScriptValue siteCategories = p_scriptEngine->globalObject().property("siteCategories");
        hash = siteCategories.call(QScriptValue());

        // iterate a returning hashmap (key:value)
        QScriptValueIterator scc_iterator(hash);
        while (scc_iterator.hasNext()) {
            scc_iterator.next();
//            qDebug() << cIt.name() << ":" << cIt.value().toString();
            categories value = categories::all;
            if (scc_iterator.value().toString() == "video") value = categories::video;
            if (scc_iterator.value().toString() == "audio") value = categories::audio;
            if (scc_iterator.value().toString() == "apps") value = categories::apps;
            if (scc_iterator.value().toString() == "games") value = categories::games;
            if (scc_iterator.value().toString() == "porn") value = categories::porn;
            if (scc_iterator.value().toString() == "other") value = categories::other;
            p_siteCategory.insert(scc_iterator.name(), value);
        }
        qDebug() << p_name << "script loaded";
    }
    catch (std::exception &exc)
    {
        qCritical() << QString("setScriptFile throws %0").arg(exc.what());
        p_valid = false;
        setFinished(true);
    }

}

void tsuProvider::search(const QString text, const tsuProvider::categories cat, const int resultsWantedPerProvider, const sortRules sortToApply)
{
    // TO DO: multi URL fallback

    // No valid URL, what to do?
    if (p_urls.size() < 1 || !isValid() || !isActive()) {
        qDebug() << p_name << "isValid" << (isValid() ? "true" : "false") << ", isActive" << (isActive() ? "true" : "false");
        p_finished = false; // i don' want to send finished event
        return;
    } else {
        setFinished(false);
    }

    p_categoryToUse = cat;
    p_resultsWanted = resultsWantedPerProvider;
    p_resultsFounds = 0;
    p_actualPage = p_startPage;
    p_sortToApply = sortToApply;

    // TO DO: check for invalid char in text before to add
    QString cleanText = text;
    p_stringToSearch = cleanText.replace(" ", "+");

    doSearch();
//    QString searchPath = p_searchPaths.value(p_categoryToUse);
//    searchPath = searchPath.replace("$SEARCH", p_stringToSearch).replace("$PAGE", QString::number(p_actualPage));

//    QString urlPath = p_urls[p_actualUrlIndex];
//    if (urlPath.left(5) == "POST:") {
//        urlPath = urlPath.replace("POST:", "");
//        QUrl url(urlPath);
//        QNetworkRequest request(url);
//        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
//        QByteArray postData;
//        postData.append(searchPath);
//        qDebug() << "requesting (post)" << urlPath << "with parameters" << searchPath;
//        p_reply = p_namPage->post(request, postData);
//        ReplyTimeout::set(p_reply, p_timeoutMilliseconds);
//    } else {
//        QString sUrl = QString("%0%1").arg(urlPath).arg(searchPath);
//        QUrl url(sUrl);
//        QNetworkRequest request(url);
//        qDebug() << "requesting" << sUrl;
//        //QNetworkReply* reply = p_namPage->get(request);
//        p_reply = p_namPage->get(request);
//        ReplyTimeout::set(p_reply, p_timeoutMilliseconds);
//    }
}

void tsuProvider::doSearch()
{
    QString searchPath = p_searchPaths.value(p_categoryToUse);

    if (searchPath.indexOf("$SORT") == -1 ) {
        searchPath = searchPath.replace("$SEARCH", p_stringToSearch)
                               .replace("$PAGE", QString::number(p_actualPage));
    } else {
        searchPath = searchPath.replace("$SEARCH", p_stringToSearch)
                               .replace("$PAGE", QString::number(p_actualPage))
                               .replace("$SORT", p_sortRules.value(p_sortToApply));
    }

    QString urlPath = p_urls[p_actualUrlIndex];
    if (urlPath.left(5) == "POST:") {
        urlPath = urlPath.replace("POST:", "");
        QUrl url(urlPath);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        QByteArray postData;
        postData.append(searchPath);
        qDebug() << "requesting (post)" << urlPath << "with parameters" << searchPath;
        p_reply = p_namPage->post(request, postData);
        ReplyTimeout::set(p_reply, p_timeoutMilliseconds);
    } else {
        QString sUrl = QString("%0%1").arg(urlPath).arg(searchPath);
        QUrl url(sUrl);
        QNetworkRequest request(url);
        qDebug() << "requesting" << sUrl;
        //QNetworkReply* reply = p_namPage->get(request);
        p_reply = p_namPage->get(request);
        ReplyTimeout::set(p_reply, p_timeoutMilliseconds);
    }
}

void tsuProvider::cancelSearch()
{
    try {
        if (p_reply && p_reply->isRunning() && !isFinished()) {
            p_reply->abort();
            qDebug() << p_name << "request cancelled";
        } else {
            qWarning() << p_name << "cannot cancel request";
            // setFinished(true);
            p_finished = true;
        }
    } catch (std::exception &exc) {
        qWarning() << QString("cancelSearch throws %0").arg(exc.what());
    }
}

int tsuProvider::getResultsFounds() const
{
    return p_resultsFounds;
}

void tsuProvider::loadIcon() const
{
    QNetworkRequest reqIcon(p_siteIconUrl);
    QNetworkReply* reply = p_namIcon->get(reqIcon);
    ReplyTimeout::set(reply, 4000);
}

bool tsuProvider::isFinished() const
{
    return p_finished;
}

bool tsuProvider::isValid() const
{
    return p_valid;
}

bool tsuProvider::isActive() const
{
    return p_active;
}

void tsuProvider::setActive(bool value)
{
    p_active = value;
}

void tsuProvider::setFinished(bool value)
{
    p_finished = value;
    if (value) {
        qDebug() << p_name << "emitting finished";
        emit finished(p_name);
    }
}

QList<QString> tsuProvider::splitRows(){
    QScriptValue scriptfunction = p_scriptEngine->globalObject().property("splitRows");
    QScriptValue rowsArray = scriptfunction.call(QScriptValue());
    QList<QString> result;
    int length = rowsArray.property("length").toInteger();
    for(int i = 0; i < length; i++) {
        result.append(rowsArray.property(i).toString());
    }
//    qDebug() << "extracted" << result.length() << "rows";
    return result;
}

QList<QString> tsuProvider::splitColumns(const QString row)
{
//    qDebug() << "received row" << row;
    QScriptValue scriptfunction = p_scriptEngine->globalObject().property("splitColumns");
    QScriptValueList args;
    args << row;
    QScriptValue columnsArray = scriptfunction.call(QScriptValue(), args);
//    qDebug() << "result" << columnsArray.toString();
    QList<QString> result;
    int length = columnsArray.property("length").toInteger();
    for(int i = 0; i < length; i++) {
        result.append(columnsArray.property(i).toString());
    }
//    qDebug() << "extracted" << result.length() << "columns";
    return result;
}

tsuProvider::searchItem tsuProvider::parseColumn(const QList<QString> columns)
{
    QScriptValue scriptfunction = p_scriptEngine->globalObject().property("parseColumn");
    QScriptValueList args;
    args << qScriptValueFromSequence(p_scriptEngine, columns);
    QScriptValue retVal = scriptfunction.call(QScriptValue(), args);
    tsuProvider::searchItem si;
    si.provider = p_name;
    QMetaEnum metaEnum = QMetaEnum::fromType<categories>();
    si.category = metaEnum.valueToKey(p_siteCategory.value(retVal.property("category").toString()));
    si.link = retVal.property("link").toString();
    si.name = retVal.property("name").toString();
    si.size = retVal.property("size").toString();
    si.hash = retVal.property("hash").toString();
    si.date = retVal.property("date").toString();
    si.seeds = retVal.property("seeds").toString();
    si.leeches = retVal.property("leeches").toString();
    si.magnet = retVal.property("magnet").toString();

    if (si.link.left(4) != "http") {
        si.link = QString("%0%1").arg(p_urls[p_actualUrlIndex]).arg(si.link);
    }

    return si;
}

void tsuProvider::downloadPageFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::OperationCanceledError) {
        qDebug() << p_name << "cancelled";
        setFinished(true);
//        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << QString("%0 cannot download page, error %1").arg(p_name).arg(reply->errorString());
        p_valid = false;
        setFinished(true);
//        reply->deleteLater();
        return;
    }

    qDebug() << p_name << "Download finished";

    QByteArray content = reply->readAll();
    std::string str = content.toStdString();
//    std::cout << "std str:" << str << std::endl;

    QString page = QString::fromStdString(str);
    p_scriptEngine->globalObject().setProperty("page", page);

//    reply->deleteLater();

    processPage();
//    QList<QString> rowsArray = splitRows();
//    qDebug() << p_name << "processing" << rowsArray.length() << "rows";

    //    QtConcurrent::map(rowsArray.begin(), rowsArray.end(), [this](const QString &row){ processPage(row); });// &tsuProvider::processPage);
}

void tsuProvider::downloadIconFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray icoData = reply->readAll();
        p_siteIcon->loadFromData(icoData);
        if (p_siteIcon->isNull()) {
//            p_siteIcon = QIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical));
            qDebug() << p_name << "icon is null";
        } else{
            qDebug() << p_name << "icon downloaded";
            emit iconDownloaded(p_name, *p_siteIcon);
        }
    } else {
//        p_siteIcon = QIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical));
        qInfo() << QString("%0 cannot download icon, error %1").arg(p_name).arg(reply->errorString());
    }
}

void tsuProvider::processPage()
{
    QList<QString> rowsArray = splitRows();
    qDebug() << p_name << "processing" << rowsArray.length() << "rows";
    int itemsFoundNow = 0;
    foreach (QString row, rowsArray) {
        // TO DO: validate row
        QList<QString> columnsArray = splitColumns(row);
        // TO DO: validate columns
        tsuProvider::searchItem si = parseColumn(columnsArray);
        if (!si.name.isNull() && !si.name.isEmpty()) {
            ++p_resultsFounds;
            ++itemsFoundNow;
            emit itemFound(si);
        } else {
            qDebug() << p_name << "received item" << p_resultsFounds << "with no name";
        }
        if (p_resultsFounds == p_resultsWanted)
            break;
    }

    if (p_resultsFounds >= p_resultsWanted) {
        qDebug() << p_name << p_resultsFounds << "results found for" << p_resultsWanted << "results requested. finished";
        setFinished(true);
    } else {
        if (itemsFoundNow > 0 && itemsFoundNow >= p_resultsPerPage && !isFinished()) {
            ++p_actualPage;
            qDebug() << p_name << p_resultsFounds << "results found for" << p_resultsWanted << "results requested and" << itemsFoundNow << "items found. requesting page" << p_actualPage;
            doSearch();
        } else {
            qDebug() << p_name << p_resultsFounds << "results found for" << p_resultsWanted << "results requested but" << itemsFoundNow << "items found. no more request. finished";
            setFinished(true);
        }
        // per ora esco, ma da qui devo richiamare la ricerca network
//        setFinished(true);
    }

//    QFuture<void> work = QtConcurrent::mapped(rowsArray, [this](const QString &row) {
//        // TO DO: validate row
//        QList<QString> columnsArray = splitColumns(row);
//        // TO DO: validate columns
//        tsuProvider::searchItem si = parseColumn(columnsArray);
//        if (!si.name.isNull() && !si.name.isEmpty())
//            emit itemFound(si);
//    });
}

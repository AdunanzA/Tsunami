#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QSettings>

#include <tsumanager.h>
#include <settingswindow.h>

#define SPLASH_DURATION 3000
#define ORGANIZATION_NAME "adunanza"
#define ORGANIZATION_DOMAIN "adunanza.com"
#define PROJECT_NAME "tsunami"

void showSplashScreen(const MainWindow & w);

void logMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    QString txt;
    QString txtContext = QString("%0::%2::%3").arg(context.file).arg(context.function).arg(context.line);
    QString date = QDateTime::currentDateTime().toString(Qt::SystemLocaleDate);

    QSettings settings(settingswindow::settingsFileName, settingswindow::settingsFileFormat);
    QString debugLevel = settings.value("Debug/Level", "Info").toString();

    switch (type) {
    case QtDebugMsg:
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg(debugLevel).arg(txtContext).arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("I").arg(txtContext).arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("W").arg(txtContext).arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("C").arg(txtContext).arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("F").arg(txtContext).arg(msg);
        break;
    }
    QFile outFile("Tsunami.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream ts(&outFile);
    ts << txt << endl;
    QTextStream out(stdout);
    out << txt << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<tsuEvents::tsuEvent>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<QVector<tsuEvents::tsuEvent>>();
    qRegisterMetaType<QPair<int,int>>();

    // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output
    qInstallMessageHandler(logMessageHandler);

    MainWindow w;

    QApplication::setOrganizationName(ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QApplication::setApplicationName(PROJECT_NAME);

    //if (!params.noSplash) {
        showSplashScreen(w);
    //} else {
        //w.show();
    //}

    return a.exec();
}

void showSplashScreen(const MainWindow &w)
{
    QPixmap splash_img(":/images/adunanza.jpg");
    QPainter painter(&splash_img);
    QString version = VERSION;
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", 12, QFont::Black));
    int paddingRight = 9;
    int paddingBottom = 9;
    painter.drawText(splash_img.width() - paddingRight - painter.fontMetrics().width(version), splash_img.height() - paddingBottom, version);
    QSplashScreen *splash = new QSplashScreen(splash_img);
    splash->show();

    QTimer::singleShot(SPLASH_DURATION, splash, SLOT(close()));
    QTimer::singleShot(SPLASH_DURATION, &w, SLOT(show()));

//    qApp->processEvents();
}


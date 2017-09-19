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
//#define ORGANIZATION_NAME "Adunanza"
//#define ORGANIZATION_DOMAIN "adunanza.com"
//#define PROJECT_NAME "tsunami"

void showSplashScreen(const MainWindow & w);

void logMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    QString txt;

    QString txtContext = QString("");
    QString funcName = QString::fromUtf8(context.function);
    QString message = QString(msg);

    // strip out all chars after '(' like '(class QWidget *)' from function name
    funcName = funcName.left(funcName.indexOf("("));

    // strip out all chars like 'void __cdecl ' before function name
    funcName = funcName.right(funcName.length() - funcName.lastIndexOf(" ") - 1);

    // strip out double quotes around string message (if present)
    if (message.left(1) == "\"") {
        message = message.right(message.length() - 1);

        if (message.right(1) == "\"") {
            message = message.left(message.length() - 1);
        }
    }

    txtContext = QString("%0::%1").arg(funcName).arg(context.line);

    //if (QLibraryInfo::isDebugBuild()) {
    //    txtContext = QString("%0::%1::%2").arg(context.file).arg(context.function).arg(context.line);
    //}

    QString date = QDateTime::currentDateTime().toString(Qt::SystemLocaleDate);

    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME));
    int debugLevel = settings.value("Debug/Level", 1).toInt();

    switch (type) {
    case QtDebugMsg:
        if (debugLevel == 0)
            txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("Debug   ").arg(txtContext).arg(message);
        break;
    case QtInfoMsg:
        if (debugLevel >= 1)
            txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("Info    ").arg(txtContext).arg(message);
        break;
    case QtWarningMsg:
        if (debugLevel >= 2)
            txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("Warning ").arg(txtContext).arg(message);
        break;
    case QtCriticalMsg:
        if (debugLevel >= 3)
            txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("Critical").arg(txtContext).arg(message);
        break;
    case QtFatalMsg:
        if (debugLevel >= 4)
            txt = QString("%0 - %1 - %2 -> %3").arg(date).arg("Fatal   ").arg(txtContext).arg(message);
        break;
    }
    if (!txt.isNull()) {
        QFile outFile("Tsunami.log");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        QTextStream ts(&outFile);
        ts << txt << endl;
        QTextStream out(stdout);
        out << txt << endl;
    }
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

    QApplication::setOrganizationName(QStringLiteral(APP_ORGANIZATION_NAME));
    QApplication::setOrganizationDomain(QStringLiteral(APP_ORGANIZATION_DOMAIN));
    QApplication::setApplicationName(QStringLiteral(APP_PROJECT_NAME));

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


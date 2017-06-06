#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>

#include "tsumanager.h"

#define SPLASH_DURATION 3000
#define ORGANIZATION_NAME "adunanza"
#define ORGANIZATION_DOMAIN "adunanza.com"
#define PROJECT_NAME "tsunami"

void showSplashScreen(const MainWindow & w);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<tsuEvents::tsuEvent>();
    qRegisterMetaType<std::string>();
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


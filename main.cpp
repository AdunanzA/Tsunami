#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

#define SPLASH_DURATION 3000

void showSplashScreen(const MainWindow & w);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

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
    painter.drawText(450 - painter.fontMetrics().width(version), 136, version);
    QSplashScreen *splash = new QSplashScreen(splash_img);
    splash->show();

    QTimer::singleShot(SPLASH_DURATION, splash, SLOT(close()));
    QTimer::singleShot(SPLASH_DURATION, &w, SLOT(show()));

//    qApp->processEvents();
}

#include "settingswindow.h"
#include "ui_settingswindow.h"

settingswindow::settingswindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingswindow)
{
    ui->setupUi(this);

    ui->txtDownloadPath->installEventFilter(this);

    loadSettings();
}

settingswindow::~settingswindow()
{
    delete ui;
}

void settingswindow::loadSettings()
{
//    QSettings settings(settingsFileName, settingsFileFormat);
//    settings.sync();
//    QStringList Groups = settings.childGroups();
//    foreach (QString group, Groups) {
//        QTreeWidgetItem *wi = new QTreeWidgetItem(ui->treeWidget);
//        wi->setForeground(0, QColor(255,255,255));
//        wi->setText(0, group);
//        settings.beginGroup(group);
//        QStringList List = settings.allKeys();
//        foreach (QString item, List) {
//            QTreeWidgetItem *wi2 = new QTreeWidgetItem(wi);
//            wi2->setForeground(0, QColor(180,180,180));
//            wi2->setText(0, item);
//        }
//        settings.endGroup();
//    }
    QSettings settings(settingsFileName, settingsFileFormat);
    QString downloadPath = settings.value("Download/downloadPath", QCoreApplication::applicationDirPath()).toString();
    setDownloadPath(downloadPath);
}

void settingswindow::saveSettings()
{
    QSettings settings(settingsFileName, settingsFileFormat);
    settings.beginGroup("Download");
    settings.setValue("downloadPath", getDownloadPath());
    settings.endGroup();
}

void settingswindow::setLastBrowsedDir(const QString &path)
{
    QSettings settings(settingsFileName, settingsFileFormat);
    settings.setValue("Download/lastBrowsedDir", path);
}

QString settingswindow::getLastBrowsedDir()
{
    QSettings settings(settingsFileName, settingsFileFormat);
    return settings.value("Download/lastBrowsedDir", QCoreApplication::applicationDirPath()).toString();
}

QString settingswindow::getDownloadPath() const
{
    return p_downloadPath;
}

void settingswindow::setDownloadPath(const QString &value)
{
    ui->txtDownloadPath->setText(value);
    p_downloadPath = value;
}

bool settingswindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == ui->txtDownloadPath && event->type() == QEvent::MouseButtonPress) {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Choose download path"),
                                                     getDownloadPath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            setDownloadPath(dir);
        }
    }
    return false;
}

void settingswindow::on_btnSave_released()
{
    saveSettings();
}

void settingswindow::on_btnCancel_released()
{
    loadSettings();
}

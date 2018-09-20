#include "changelog.h"
#include "ui_changelog.h"

changelog::changelog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changelog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint

    connect(ui->cmbVersions, &QComboBox::currentTextChanged,
            this, &changelog::onCmbChange);

    //QFile changelog(QApplication::applicationDirPath() + "/CHANGELOG.md");
    QFile changelog(":/CHANGELOG.md");
    changelog.open(QIODevice::ReadOnly);

    QTextStream in(&changelog);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isNull() || line.isEmpty() || line.contains("Unreleased") || line.left(3) != "## ") continue;
        ui->cmbVersions->addItem(line.mid(4, line.indexOf("]") - 4));
    }
}

changelog::~changelog()
{
    delete ui;
}

void changelog::onCmbChange(const QString &selectedText)
{
    ui->lblText->clear();
    QFile changelog(":/CHANGELOG.md");
    changelog.open(QIODevice::ReadOnly);

    ui->lblVersion->setText(selectedText);

    QTextStream in(&changelog);

    bool archiveRow = false;
    QStringList sl;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.toUpper().contains("UNRELEASED") || line.isNull() || line.isEmpty()) continue;

        if ( line.left(4) == "## [" && archiveRow)
            break;

        if ( archiveRow ) {
//            QString oldText = ui->lblText->text() + "<br>";
            if (line.left(3) == "###") {
                line.replace("### ", "<span><b><i>").append("</i></b></span>");
            }
            if (line.left(2) == "- ") {
                line.append("</tt>");
            }
//            ui->lblText->setText(oldText + "<tt>" + line);
            sl.append("<tt>" + line);
        }

        if ( line.mid(4, line.indexOf("]") - 4) == selectedText ) {
            archiveRow = true;
            QString stringDate(line.right(10));
            QDateTime date = QDateTime::fromString(stringDate, "yyyy-MM-dd");
            QString newStringDate = date.toString(Qt::SystemLocaleShortDate).left(10); // user friendly date without time
            ui->lblDate->setText(newStringDate);
        }
    }
    ui->lblText->setText(sl.join("<br>"));
}

void changelog::on_btnClose_released()
{
    this->close();
    this->deleteLater();
}

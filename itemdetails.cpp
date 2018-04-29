#include "itemdetails.h"
#include "ui_itemdetails.h"

#include "mainwindow.h"
MainWindow *p_mw;

itemDetails::itemDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::itemDetails)
{
    ui->setupUi(this);
    p_mw = reinterpret_cast<MainWindow*>(parent->parent());

    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    p_refreshTimer = new QTimer(this);
    connect(p_refreshTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
//    p_refreshTimer->start(1000);
}

itemDetails::~itemDetails()
{
    delete ui;
    qDebug("itemDetails destroyed");
}

void itemDetails::setHash(const std::string &hash)
{
    p_hash = std::string(hash);
    libtorrent::sha1_hash sh(p_hash);
    libtorrent::session *ses = p_mw->sessionManager->p_session.data();
    th = QSharedPointer<libtorrent::torrent_handle>::create(ses->find_torrent(sh));

    libtorrent::torrent_status ts = th.data()->status();
    ui->lblTorrentName->setText(ts.name.c_str());

    int pieceLength = -1;
    int piecesCount = -1;
    if (th.data()->torrent_file())
    {
        pieceLength = th.data()->torrent_file()->piece_length();
    }
    ui->txtPiecesLength->setText(QLocale().toString(pieceLength));

    if (th.data()->torrent_file())
    {
        piecesCount = th.data()->torrent_file()->num_pieces();
    }
    ui->txtPiecesCount->setText(QLocale().toString(piecesCount));

    ui->chkSequential->setChecked(ts.sequential_download);

    update();
}

std::string itemDetails::getHash() const
{
    return p_hash;
}

void itemDetails::update()
{
    if (p_hash.empty() || !this->isVisible()) return;

    if (!th.data()->is_valid()) {
        this->close();
    }
//    qDebug("processing details update");

    libtorrent::torrent_status ts = th.data()->status();
    bool isPaused = ts.paused && !ts.auto_managed;
    float ratio = 0;
    int eta = -1;

    if (ts.all_time_download > 0)
    {
        ratio = (float)ts.all_time_upload / (float)ts.all_time_download;
    }

    if (!isPaused)
    {
        int64_t remaining_bytes = ts.total_wanted - ts.total_wanted_done;
        if (remaining_bytes > 0 && ts.download_payload_rate > 0)
        {
            eta = (int)(remaining_bytes / ts.download_payload_rate);
        }
    }

    std::vector<int64_t> progress;
    th.data()->file_progress(progress, libtorrent::torrent_handle::file_progress_flags_t::piece_granularity);

    /* FILES LIST */
    ui->tableFiles->setRowCount(0);
//    ui->lblPieces->setPlaceholderText("");

    if (!th.data()->torrent_file()) {
        return;
    }

    libtorrent::file_storage files = th.data()->torrent_file()->files();
    for (int i = 0; i < files.num_files(); i++)
    {
//        libtorrent::file_index_t fi(i);
//        double prg = (double)((float)progress.at(i) / files.file_size(fi))*100;
        double prg = (double)((float)progress.at(i) / files.file_size(i))*100;

        ui->tableFiles->insertRow(i);
//        QTableWidgetItem *item1 = new QTableWidgetItem(QString(files.file_path(fi).c_str()));
        QTableWidgetItem *item1 = new QTableWidgetItem(QString(files.file_path(i).c_str()));
//        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(th.data()->file_priority(fi)));
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(th.data()->file_priority(i)));
        QTableWidgetItem *item3 = new QTableWidgetItem(QString("%0%").arg(QString::number(prg)));
//        QTableWidgetItem *item4 = new QTableWidgetItem(QLocale().toString((qint64)files.file_size(fi)));
        QTableWidgetItem *item4 = new QTableWidgetItem(QLocale().toString((qint64)files.file_size(i)));
        ui->tableFiles->setItem(i, 0, item1);
        ui->tableFiles->setItem(i, 1, item2);
        ui->tableFiles->setItem(i, 2, item3);
        ui->tableFiles->setItem(i, 3, item4);

    }

//    QString htmlOff = "<span style='background-color:#808080;font-size:7px;border:1px solid #999999'>&nbsp;&nbsp;&nbsp;&nbsp;</span>";
//    QString htmlOff = "<img src='qrc:/images/htmlOff.png'></img>"; background-image: url(:/images/htmlOff.png);
//    QString htmlOff = "<div style='background-image: url(:/images/htmlOff.png);background-repeat:no-repeat;background-position: center center;min-height:8px;min-width:8px'></div>";

//    QString htmlOn  = "<span style='background-color:#90EE90;font-size:7px;border:1px solid #999999'>&nbsp;&nbsp;&nbsp;&nbsp;</span>";
//    QString htmlOn  = "<img src='qrc:/images/htmlOn.png'></img>";
//    QString htmlOn = "<span style='background-image: url(:/images/htmlOn.png);background-repeat:no-repeat;background-position: center center;min-height:8px;min-width:8px'></span>";

    ui->listWidget->clear();

//    QString result = "";
//    ui->lblPieces->setHtml(result);
    for (int j = 0; j < ts.pieces.size(); ++j) {

        QListWidgetItem *item = new QListWidgetItem("  ", ui->listWidget);
        item->setFont(QFont("Arial", 5));
        item->setFlags(Qt::NoItemFlags | Qt::ItemNeverHasChildren);

//        libtorrent::piece_index_t pi(j);
//        if (ts.pieces.get_bit(pi)) {
        if (ts.pieces.get_bit(j)) {
            item->setBackground(QColor(144, 238, 144));
//            result += htmlOn;
        } else {
            item->setBackground(QColor(128, 128, 128));
//            result += htmlOff;;
        }
    }
//    ui->lblPieces->setHtml(result);

    /* STATE */
    bool hasError = ts.paused && ts.errc;
    bool isSeeding = (ts.state == libtorrent::torrent_status::state_t::finished || ts.state == libtorrent::torrent_status::state_t::seeding);
    bool isQueued = (ts.paused && ts.auto_managed);
    bool isChecking = (ts.state == libtorrent::torrent_status::state_t::checking_files || ts.state == libtorrent::torrent_status::state_t::checking_resume_data);
    bool isForced = (!ts.paused && !ts.auto_managed);

    QString state = "Torrent::State::Unknown";

    if (isPaused)
    {
        if (hasError)
        {
            state = "Torrent::State::Error";
        }
        else
        {
            if (isSeeding)
            {
                state = "Torrent::State::UploadingPaused";
            }
            else
            {
                state = "Torrent::State::DownloadingPaused";
            }
        }
    }
    else
    {
        if (isQueued && !isChecking)
        {
            if (isSeeding)
            {
                state = "Torrent::State::UploadingQueued";
            }
            else
            {
                state = "Torrent::State::DownloadingQueued";
            }
        }
        else
        {
            switch (ts.state)
            {
            case libtorrent::torrent_status::state_t::finished:
            case libtorrent::torrent_status::state_t::seeding:
            {
                if (isForced)
                {
                    state = "Torrent::State::UploadingForced";
                }
                else if (isPaused)
                {
                    state = "Torrent::State::Complete";
                }
                else
                {
                    if (ts.upload_payload_rate > 0)
                    {
                        state = "Torrent::State::Uploading";
                    }
                    else
                    {
                        state = "Torrent::State::UploadingStalled";
                    }
                }
                break;
            }

            case libtorrent::torrent_status::state_t::checking_resume_data:
            {
                state = "Torrent::State::CheckingResumeData2";
                break;
            }

            case libtorrent::torrent_status::state_t::checking_files:
            {
                state = "Torrent::State::DownloadingChecking";
                break;
            }

            case libtorrent::torrent_status::state_t::downloading_metadata:
            {
                state = "Torrent::State::DownloadingMetadata";
                break;
            }

            case libtorrent::torrent_status::state_t::downloading:
            {
                if (isForced)
                {
                    state = "Torrent::State::DownloadingForced";
                }
                else
                {
                    if (ts.download_payload_rate > 0)
                    {
                        state = "Torrent::State::Downloading";
                    }
                    else
                    {
                        state = "Torrent::State::DownloadingStalled";
                    }
                }
                break;
            }
            }
        }
    }
    ui->lblState->setText(state);
}

void itemDetails::timerTick()
{
    update();
}

void itemDetails::closeEvent(QCloseEvent *event)
{
    p_refreshTimer->stop();
    event->accept();
}

void itemDetails::showEvent(QShowEvent *event)
{
    qDebug("called showEvent");
    if (p_hash.empty() || !this->isVisible()) return;

    libtorrent::torrent_status ts = th.data()->status();
    ui->chkSequential->setChecked(ts.sequential_download);
    update();
    p_refreshTimer->start(1000);
    qDebug("showing details");
    event->accept();
}

void itemDetails::on_chkSequential_toggled(bool checked)
{
    th.data()->set_sequential_download(checked);
}

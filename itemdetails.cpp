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

    p_refreshTimer = new QTimer(this);
    connect(p_refreshTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
//    p_refreshTimer->start(1000);
}

itemDetails::~itemDetails()
{
    delete ui;
}

void itemDetails::setHash(const std::string &hash)
{
    p_hash = std::string(hash);
    lt::sha1_hash sh(p_hash);
    lt::session *ses = p_mw->sessionManager->p_session.data();
    th = QSharedPointer<lt::torrent_handle>::create(ses->find_torrent(sh));
    update();
}

void itemDetails::update()
{
    if (p_hash.empty() || !this->isVisible()) return;

    if (!th.data()->is_valid()) {
        this->close();
    }

    lt::torrent_status ts = th.data()->status();
    ui->lblTorrentName->setText(ts.name.c_str());

    bool isPaused = ts.paused && !ts.auto_managed;
    float ratio = 0;
    int eta = -1;
    int pieceLength = -1;
    int piecesCount = -1;

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

    std::vector<int64_t> progress;
    th.data()->file_progress(progress, lt::torrent_handle::file_progress_flags_t::piece_granularity);

    /* FILES LIST */
    ui->tableFiles->setRowCount(0);
    ui->lblPieces->setPlaceholderText("");
    lt::file_storage files = th.data()->torrent_file()->files();
    for (int i = 0; i < files.num_files(); i++)
    {
        lt::file_index_t fi(i);
        double prg = (double)((float)progress.at(i) / files.file_size(fi))*100;

        ui->tableFiles->insertRow(i);
        QTableWidgetItem *item1 = new QTableWidgetItem(QString(files.file_path(fi).c_str()));
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(th.data()->file_priority(fi)));
        QTableWidgetItem *item3 = new QTableWidgetItem(QString("%0%").arg(QString::number(prg)));
        QTableWidgetItem *item4 = new QTableWidgetItem(QLocale().toString(files.file_size(fi)));
        ui->tableFiles->setItem(i, 0, item1);
        ui->tableFiles->setItem(i, 1, item2);
        ui->tableFiles->setItem(i, 2, item3);
        ui->tableFiles->setItem(i, 3, item4);

    }

//    std::vector<int> pieces;
//    th.piece_availability(pieces);
//    for (int j = 0; j < pieces.size(); ++j) {
//        ui->lblPieces->setPlaceholderText(QString("%0 %1").arg(ui->lblPieces->placeholderText()).arg(QString::number(pieces.at(j))));
//    }
//    lt::typed_bitfield ps = ts.pieces;
//    QString htmlOff = "<span style='background-color:#808080;font-size:7px'>&nbsp;&nbsp;&nbsp;&nbsp;</span>";
    QString htmlOff = "<img src='qrc:/images/htmlOff.png'></img>";
//    QString htmlOn  = "<span style='background-color:#90EE90;font-size:7px'>&nbsp;&nbsp;&nbsp;&nbsp;</span>";
    QString htmlOn  = "<img src='qrc:/images/htmlOn.png'></img>";
    QString result = "";
    ui->lblPieces->setHtml(result);
    for (int j = 0; j < ts.pieces.size(); ++j) {
        lt::piece_index_t pi(j);
        if (ts.pieces.get_bit(pi)) {
            result += htmlOn;
        } else {
            result += htmlOff;;
        }
    }
    ui->lblPieces->setHtml(result);

    /* STATE */
    bool hasError = ts.paused && ts.errc;
    bool isSeeding = (ts.state == lt::torrent_status::state_t::finished || ts.state == lt::torrent_status::state_t::seeding);
    bool isQueued = (ts.paused && ts.auto_managed);
    bool isChecking = (ts.state == lt::torrent_status::state_t::checking_files || ts.state == lt::torrent_status::state_t::checking_resume_data);
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
            case lt::torrent_status::state_t::finished:
            case lt::torrent_status::state_t::seeding:
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

            case lt::torrent_status::state_t::checking_resume_data:
            {
                state = "Torrent::State::CheckingResumeData2";
                break;
            }

            case lt::torrent_status::state_t::checking_files:
            {
                state = "Torrent::State::DownloadingChecking";
                break;
            }

            case lt::torrent_status::state_t::downloading_metadata:
            {
                state = "Torrent::State::DownloadingMetadata";
                break;
            }

            case lt::torrent_status::state_t::downloading:
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
    p_refreshTimer->start(1000);
    event->accept();
}

void itemDetails::on_checkBox_toggled(bool checked)
{
    th.data()->set_sequential_download(checked);
}

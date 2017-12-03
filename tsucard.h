#ifndef TSUCARD_H
#define TSUCARD_H

#include <QWidget>
#include <QDebug>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>

#include <libtorrent/hasher.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

//namespace lt = libtorrent;

namespace Ui {
    class tsucard;
}

class tsucard : public QWidget
{
    Q_OBJECT


public:
    explicit tsucard(QWidget *parent = 0);
    ~tsucard();

    QString get_Name() const;
    void set_Name(const QString name);

    libtorrent::sha1_hash get_Hash() const;
    void set_Hash(const libtorrent::sha1_hash &value);

    void set_State(const QString actual_state);
    void set_Total(const int qty);
    void set_Downloaded(const int qty);
    void set_Uploaded(const int qty);
    void set_Percentage(const int qty);
    void set_DownloadRate(const int qty);
    void set_UploadRate(const int qty);

    int get_Downloaded();
    int get_Uploaded();
    int get_DownloadRate();
    int get_UploadRate();

    QString InfoSize(int value);
    QString InfoUnit(int value);

private:
    Ui::tsucard *ui;
    libtorrent::sha1_hash p_hash;

    int p_downloaded = 0;
    int p_uploaded = 0;
    int p_downloadRate = 0;
    int p_uploadRate = 0;

    int total_size = 0;

private slots:
    void btnCancelClick();

signals:
    void cancelRequested(tsucard *sender);
};

#endif // TSUCARD_H

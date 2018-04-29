#ifndef ITEMDETAILS_H
#define ITEMDETAILS_H

#include <QDialog>
#include <QTimer>

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/bitfield.hpp>

namespace Ui {
    class itemDetails;
}

class itemDetails : public QDialog
{
    Q_OBJECT

public:
    explicit itemDetails(QWidget *parent = 0);
    ~itemDetails();

    void setHash(const std::string &hash);
    std::string  getHash() const;
    void update();

public slots:


private:
    Ui::itemDetails *ui;
    std::string p_hash;
    QSharedPointer<libtorrent::torrent_handle> th;
    QTimer *p_refreshTimer;

private slots:
    void timerTick();

    // QWidget interface
    void on_chkSequential_toggled(bool checked);

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

};

#endif // ITEMDETAILS_H

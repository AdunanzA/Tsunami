#ifndef TSUEVENTS_H
#define TSUEVENTS_H

namespace tsuEvents {
    class tsuEvent
    {
        public:
        tsuEvent()
        { }
        tsuEvent(const std::string &_hash, const QString &_name, const int &_downloaded = 0,
                 const int &_uploaded = 0, const int &_downRate = 0, const int &_upRate = 0,
                 const int &_size = 0, const int &_state = 0, const int &_percentage = 0,
                 const int &_numSeeds = 0, const int &_numPeers = 0):
            hash(_hash),
            name(_name),
            downloaded(_downloaded),
            uploaded(_uploaded),
            downloadRate(_downRate),
            uploadRate(_upRate),
            total_size(_size),
            state(_state),
            percentage(_percentage),
            numSeeds(_numSeeds),
            numPeers(_numPeers)
        { }
        ~tsuEvent()
        { }

            std::string hash ="";
            QString name ="";
            int downloaded = 0;
            int uploaded = 0;
            int downloadRate = 0;
            int uploadRate = 0;
            int total_size = 0;
            int state = 0;
            int percentage = 0;
            int numSeeds = 0;
            int numPeers = 0;
    };
}


#endif // TSUEVENTS_H

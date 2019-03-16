#ifndef TSUEVENTS_H
#define TSUEVENTS_H

#include <string> // needed for std::string
#include <QString> // needed for QString

namespace tsuEvents {
    class tsuEvent
    {
        public:
            // member variables
            std::string hash;
            QString name;
            uint64_t downloaded {};
            uint64_t uploaded {};
            int downloadRate {};
            int uploadRate {};
            uint64_t total_size {};
            int state {};
            int percentage {};
            int numSeeds {};
            int numPeers {};


        public:
            // methods
            tsuEvent()
            {

            }

            tsuEvent(const std::string& _hash,
                     const QString& _name,
                     uint64_t _downloaded = 0,
                     uint64_t _uploaded = 0,
                     int _downRate = 0,
                     int _upRate = 0,
                     uint64_t _size = 0,
                     int _state = 0,
                     int _percentage = 0,
                     int _numSeeds = 0,
                     int _numPeers = 0):
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
            {

            }

            ~tsuEvent()
            {
            }

    };
}


#endif // TSUEVENTS_H

#include "logger.h"
//#include "preferences.h"

logger::logger(QObject *parent) : QObject(parent)
{
    //setLevel();
}

void logger::setLevel(logLevel level) {
    //this->level() = level;
    p_level = level;
}

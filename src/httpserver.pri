# Enable very detailed debug messages when compiling the debug version
CONFIG(debug, debug|release) {
    #DEFINES += SUPERVERBOSE
}

INCLUDEPATH += $$PWD/../src

HEADERS += $$PWD/../src/httpserver/httpglobal.h \
           $$PWD/../src/httpserver/httplistener.h \
           $$PWD/../src/httpserver/httpconnectionhandler.h \
           $$PWD/../src/httpserver/httpconnectionhandlerpool.h \
           $$PWD/../src/httpserver/httprequest.h \
           $$PWD/../src/httpserver/httpresponse.h \
           $$PWD/../src/httpserver/httpcookie.h \
           $$PWD/../src/httpserver/httprequesthandler.h \
           $$PWD/../src/httpserver/httpsession.h \
           $$PWD/../src/httpserver/httpsessionstore.h \
           $$PWD/../src/httpserver/staticfilecontroller.h \
           $$PWD/../src/httpserver/controller/fileuploadcontroller.h \
           $$PWD/../src/httpserver/controller/formcontroller.h \
           $$PWD/../src/httpserver/controller/sessioncontroller.h \
           $$PWD/../src/httpserver/handler/websockethandler.h \
           $$PWD/../src/httpserver/handler/requesthandler.h

SOURCES += $$PWD/../src/httpserver/httpglobal.cpp \
           $$PWD/../src/httpserver/httplistener.cpp \
           $$PWD/../src/httpserver/httpconnectionhandler.cpp \
           $$PWD/../src/httpserver/httpconnectionhandlerpool.cpp \
           $$PWD/../src/httpserver/httprequest.cpp \
           $$PWD/../src/httpserver/httpresponse.cpp \
           $$PWD/../src/httpserver/httpcookie.cpp \
           $$PWD/../src/httpserver/httprequesthandler.cpp \
           $$PWD/../src/httpserver/httpsession.cpp \
           $$PWD/../src/httpserver/httpsessionstore.cpp \
           $$PWD/../src/httpserver/staticfilecontroller.cpp \
           $$PWD/../src/httpserver/controller/fileuploadcontroller.cpp \
           $$PWD/../src/httpserver/controller/formcontroller.cpp \
           $$PWD/../src/httpserver/controller/sessioncontroller.cpp \
           $$PWD/../src/httpserver/handler/websockethandler.cpp \
           $$PWD/../src/httpserver/handler/requesthandler.cpp

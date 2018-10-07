/**
  @file
  @author Stefan Frings
*/

#ifndef SESSIONCONTROLLER_H
#define SESSIONCONTROLLER_H

#include <src/httpserver/httprequest.h>
#include <src/httpserver/httpresponse.h>
#include <src/httpserver/httprequesthandler.h>
#include <src/httpserver/httpsessionstore.h>

using namespace stefanfrings;

/**
  This controller demonstrates how to use sessions.
*/

class SessionController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(SessionController)
public:

    /** Constructor */
    SessionController(HttpSessionStore *session);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);

private:
    HttpSessionStore* p_sessionStore;

};

#endif // SESSIONCONTROLLER_H

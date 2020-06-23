#include <QtCore>
#include <QtNetwork>
#include <iostream>

#include <bhttp.hpp>

BHttp::BHttp(QObject *parent) : QObject(parent) {
    connect(&http, SIGNAL(done(bool)), this, SIGNAL(done(bool)));
}

bool BHttp::checkUrl( const QUrl &url ) {
    if (!url.isValid()) {
        return false;
    }

    if (url.scheme() != "http") {
        return false;
    }

    if (url.path().isEmpty()) {
        return false;
    }

    return true;
}

bool BHttp::getToString(const QUrl &url) {
    http.abort() ;
	//if( !checkUrl( url ) ) 
	//return false;

    output = "";

    connect(&http, SIGNAL(requestFinished(int,bool)), this, SLOT(readHttpData(int,bool)));
    
    http.setHost(url.host(), url.port(80));
    http.get(url.path());
    http.close();

    return true;
}

void BHttp::readHttpData(int id, bool error)
{
     if( ! error ) output.append ( http.readAll().data() );

}

QString BHttp::getOutputString()
{
	return output;
}


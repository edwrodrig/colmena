#ifndef BHTTP_H
#define BHTTP_H

#include <QHttp>

class QUrl;

class BHttp : public QObject
{
    Q_OBJECT

	public:
		BHttp( QObject * parent = 0 );

		bool getToString (const QUrl & url ) ;
		QString getOutputString ( void ) ;

	signals:
		void done ( bool error ) ;

	private slots:
		void readHttpData ( int id , bool error ) ;

	private:
		QHttp http;
		QString output;
		int httpid;

		bool checkUrl ( const QUrl &url ) ;
} ;

#endif

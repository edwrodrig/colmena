#ifndef ___SAVER___
#define ___SAVER___

#include <QWidget>
#include <QTcpSocket>
#include <QWebView>
#include <QTimer>
#include <QList>
#include <QStringList>
#include <QHttp>
#include <bhttp.hpp>
#include <QDataStream>

class QEvent ;

class Saver : public QWidget
{
	Q_OBJECT
	
	public :
		Saver ( void ) ;
		Saver ( WId window ) ;
	
		void init ( void ) ;
		void connectToSlave ( void ) ;

	protected :
		bool event ( QEvent * event ) ;
		bool checkUrl ( const QUrl & url ) ;


	private slots :
		void changeList ( void ) ;
		void nextPage ( void ) ;
		void request ( void ) ;
		void generalPageListRequestFinished ( bool error ) ;
		void specificPageListRequestFinished ( bool error ) ;
	
	signals :
		void inputEvent ( void ) ;

	private :
		bool checkIncommingBytes ( void ) ;
		bool waitForIncommingBytes ( int bytes ) ;
		void requestFinished ( void ) ;
		
		QTcpSocket socket ;
		QWebView * view ;

		QUrl generalPageListUrl ;
		QUrl specificPageListUrl ;
		
		BHttp generalPageListRequest ;
		BHttp specificPageListRequest ;
	
		QStringList strings ;
		QList<QUrl> pageList ;
		QTimer changeTimer ;
		QTimer requestTimer ;
		int currentPageIndex ;
		int currentRequest ;
		
		QDataStream stream ;  
		
		int eventCount ;
		
} ;

#endif


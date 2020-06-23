#include <saver.hpp>
#include <slavessaverdef.hpp>
#include <QDesktopWidget>
#include <QtWebKit> 
#include <QUrl>
#include <QEvent>
#include <inputstream.hpp>
#include <QSettings>

Saver::Saver ( void ) : QWidget() , view ( 0L )
{
	init () ;
}

Saver::Saver ( WId window ) : QWidget() , view ( 0L )
{
	if ( window ) create ( window , false ) ;
	init () ;
}

bool Saver::checkUrl( const QUrl & url )
{
    if ( ! url.isValid() ) return false ;
    if ( url.scheme() != "http" ) return false ;
    if ( url.path().isEmpty() ) return false ;
	return true ;
}

void Saver::init ( void )
{
	eventCount = 0 ;
	setObjectName ( "Colmena Virtual Cluster Screensaver" ) ;
	QDesktopWidget screen ;
	
	showFullScreen() ;
	setFixedSize ( screen.size() ) ;
	setMouseTracking( true ) ;
	
	currentPageIndex = 0 ;
	
	view = new QWebView ( this ) ;
	view->setFixedSize ( screen.size() ) ;
	view->show() ;
	
	pageList.clear() ;
	pageList.push_back ( QUrl ( QString("file:///").append( APP_PATH ).append( "/stats/index.html" ) ) ) ;		
	pageList.push_back ( QUrl ( QString("file:///").append( APP_PATH ).append( "/stats/alt.html" ) ) ) ;
	
	connect ( & changeTimer , SIGNAL ( timeout() ) , this , SLOT ( nextPage () ) ) ;
	changeTimer.start( 15000 ) ;
	
}

void Saver::connectToSlave ( void )
{
	QSettings settings ( QSettings::SystemScope , "Colmena" , "Slave" ) ;
	if ( ! settings.contains ( "ssaverport" ) ) settings.setValue ( "ssaverport" , QVariant ( SLAVESSAVER_PORT ) ) ;
	quint16 ssaverport = settings.value ( "ssaverport" ).toUInt() ;

	connect( & generalPageListRequest , SIGNAL( done ( bool ) ) , this , SLOT ( generalPageListRequestFinished ( bool ) ) ) ;
	connect( & specificPageListRequest , SIGNAL( done ( bool ) ) , this ,  SLOT ( specificPageListRequestFinished ( bool ) ) ) ;
	connect ( & requestTimer , SIGNAL ( timeout() ) , this , SLOT ( request () ) ) ;
	
	connect ( & socket , SIGNAL ( readyRead() ) , this , SLOT ( changeList() ) ) ;
	stream.setDevice ( & socket ) ;
	stream.setVersion( QDataStream::Qt_4_1 ) ; 
	
	socket.connectToHost( QHostAddress::LocalHost , ssaverport ) ;
	nextPage() ;
}

bool Saver::checkIncommingBytes ( void )
{
	quint32 size ;
	if ( ! waitForIncommingBytes( (int)sizeof(quint32) ) ) return false ;
	stream >> size ;
	if ( ! waitForIncommingBytes( (int)size ) )	return false ;
	return true ;
}

bool Saver::waitForIncommingBytes ( int bytes )
{
	while ( socket.bytesAvailable() < bytes ) if ( ! socket.waitForReadyRead( 30000 ) ) return false ;
	return true ;
}

void Saver::changeList ( void )
{
	QTextStream out ( stdout ) ;	
	QDataStream stream ( & socket ) ;
	stream.setVersion( QDataStream::Qt_4_1 ) ;  		
	QString string ;
	quint8 type ;
	
	if ( ! checkIncommingBytes () ) return ;
	
	stream >> type ;
	if ( type == SLAVESSAVER_CHANGEPAGELIST )
	{
		stream >> string ;
		generalPageListUrl = QUrl ( string ) ;
		stream >> string ;
		specificPageListUrl = QUrl ( string ) ;

		request () ;
		requestTimer.start ( 120000 ) ;	
	}
	else if ( type == SLAVESSAVER_GENERATEDHTML )
	{
		if ( currentPageIndex == 0 && view ) view->load ( pageList.at ( 0 ) ) ;	
	}
}

void Saver::request ( void )
{
	strings.clear() ;
	currentRequest = 0 ;
	generalPageListRequest.getToString ( generalPageListUrl ) ;
	specificPageListRequest.getToString ( specificPageListUrl ) ;
}

void Saver::requestFinished ( void )
{
	currentRequest ++ ;
	if ( currentRequest >= 2 )
	{
		pageList.clear() ;
		pageList.push_back ( QUrl ( QString("file:///").append( APP_PATH ).append( "/stats/index.html" ) ) ) ;
		pageList.push_back ( QUrl ( QString("file:///").append( APP_PATH ).append( "/stats/alt.html" ) ) ) ;
		for ( int i = 0 ; i < strings.size() ; i ++ )
		{
			QUrl url ( strings.at(i) ) ;
			if ( checkUrl ( url ) )
			{
				pageList.push_back ( url ) ;
			}
		}
	}
}

void Saver::generalPageListRequestFinished ( bool error )
{
	if ( ! error ) strings << generalPageListRequest.getOutputString().split("\n") ;
	requestFinished () ;
}
void Saver::specificPageListRequestFinished ( bool error )
{
	if ( ! error ) strings << specificPageListRequest.getOutputString().split("\n") ;
	requestFinished () ;
}

void Saver::nextPage ( void )
{
	currentPageIndex ++ ;
	if ( currentPageIndex >= pageList.size() )
	{
		currentPageIndex = 0 ;
		InputStream input ( false ) ;
		input.in() << (quint32) 0 ;
		socket.write ( input.data( false ) ) ;
	}
	else if ( view ) view->load ( pageList.at ( currentPageIndex ) ) ;
}

bool Saver::event ( QEvent * event )
{
	switch ( event->type() )
	{
		case QEvent::KeyPress :
		case QEvent::KeyRelease :
		case QEvent::MouseButtonPress :
		case QEvent::MouseButtonRelease :
		case QEvent::MouseTrackingChange :
			emit inputEvent() ;
			break ;
		case QEvent::MouseMove :
			if ( eventCount < 1 ) eventCount ++ ;
			else emit inputEvent() ;
			break ;
		default :
			return QWidget::event ( event ) ;
	}
	return true ;
}


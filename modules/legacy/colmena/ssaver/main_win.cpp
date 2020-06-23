#include <QApplication>
#include <saver.hpp>
#include <config.hpp>
#include <QStringList>
#include <QDesktopWidget>
#include <QColor>

#include <windows.h>

int main ( int argc , char *argv[] )
{
	QApplication app( argc , argv ) ;
	QStringList args = app.arguments() ;
	app.setApplicationName ( "Colmena Virtual Cluster ScreenSaver" ) ;

	if ( args.size() == 1 )
	{
		Config config ;
		config.show() ;
		return app.exec() ;
	}
	else if ( args.size() == 2 &&  args.at(1).startsWith("/s")  )
	{
		app.setOverrideCursor ( QCursor ( Qt::BlankCursor ) ) ;
		Saver saver ;
		saver.connectToSlave() ;
		saver.show() ;
		saver.connect  ( &saver , SIGNAL ( inputEvent () ) , &app , SLOT ( quit () ) ) ;
		return app.exec();	
	}
	else if ( args.size() == 2 && args.at(1).startsWith("/c") ) 
	{
		QPalette palette = app.palette() ;
		palette.setColor( QPalette::Window , Qt::black ) ;
		palette.setColor( QPalette::WindowText , Qt::green ) ;
		app.setPalette ( palette ) ;

		QString number = args.at(1) ;
		number.remove ( 0 , 3 ) ;
		Config config ;
		SetParent ( config.effectiveWinId() , (WId)(number.toInt()) ) ;
		config.show() ;
		return app.exec() ;
	}
	return 0 ;

}


#include <QCoreApplication>
#include <slavessaverdef.hpp>
#include <QSettings>
#include <QStringList>
#include <QTcpSocket>
#include <QProcess>
#include <QHostAddress>
#include <QtDebug>

int main ( int argc , char *argv[] )
{
	QCoreApplication app( argc , argv ) ;
	QStringList args = app.arguments() ;
	app.setApplicationName ( "Colmena Virtual Cluster ScreenSaver" ) ;

        QSettings settings ( QSettings::SystemScope , "Colmena" , "Slave" ) ;
	

	QString ssaverBin = settings.value ( "ssaverbin" ).toString() ;
	qWarning() << "ssaverBin : " << ssaverBin ;
	QProcess process ;

	QTcpSocket socket ;

	if ( args.size() > 1 && ( args.at(1) == "-root" || args.at(1) == "--root" ) )
	{
		if ( ! settings.contains ( "ssaverport" ) ) settings.setValue ( "ssaverport" , QVariant ( SLAVESSAVER_PORT ) ) ;
		quint16 ssaverport = settings.value ( "ssaverport" ).toUInt() ;
		qWarning() << "ssaverPort : " << ssaverport ;

        	socket.connectToHost( QHostAddress::LocalHost , ssaverport ) ;

	}
	args.removeFirst() ;
	process.start ( ssaverBin , args ) ;
	if ( process.waitForStarted () ) qWarning("started" ) ;
	else qWarning( "fail" ) ;
	process.waitForFinished() ;	

	return 0 ;
}


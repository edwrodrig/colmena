#include <QCoreApplication>
#include <slavemanager.hpp>
#include <slavessaverdef.hpp>
#include <QSettings>
#include <QTextStream>
#include <QHostAddress>

int main ( int argc , char *argv[] )
{
	QCoreApplication app( argc , argv ) ;
	app.addLibraryPath ( APP_PATH ) ;

	QSettings settings ( QSettings::SystemScope , "Colmena" , "Slave" ) ;
	if ( ! settings.contains ( "masterip") ) settings.setValue ( "masterip" , QVariant ( QString ( "127.0.0.1") ) ) ;
	if ( ! settings.contains ( "masterport" ) ) settings.setValue ( "masterport" , QVariant ( 40000 ) ) ;
	if ( ! settings.contains ( "listenport" ) ) settings.setValue ( "listenport" , QVariant ( 40001 ) ) ;	
	if ( ! settings.contains ( "maxtasks" ) ) settings.setValue ( "maxtasks" , QVariant ( 1 ) ) ;	
	if ( ! settings.contains ( "alwayshigh" ) ) settings.setValue ( "alwayshigh" , QVariant ( 0 ) ) ;
	if ( ! settings.contains ( "group" ) ) settings.setValue ( "group" , QVariant ( "" ) ) ;
	if ( ! settings.contains ( "ssaverport" ) ) settings.setValue ( "ssaverport" , QVariant ( SLAVESSAVER_PORT ) ) ;
	if ( ! settings.contains ( "ssaverbin" ) ) settings.setValue ( "ssaverbin" , QVariant ( "" ) ) ;
	
	QStringList args = app.arguments() ;
	
	if ( args.size() == 2 )
	{	
		if ( args.at(1) == "run" )
		{
			SlaveManager manager ;
			if ( ! manager.init() ) return false ;
			return app.exec() ;		
		}
		else if ( args.at(1) == "info" )
		{
			QTextStream out ( stdout ) ;
			out << "Version                 : " << SLAVEVERSION << endl
				<< "Master Ip               : " << settings.value("masterip" ).toString() << endl
				<< "Master Port             : " << settings.value("masterport" ).toInt() << endl 
				<< "Listen Port             : " << settings.value("listenport" ).toInt() << endl
				<< "Max Tasks               : " << settings.value("maxtasks" ).toInt() << endl
				<< "Always on high priority : " << settings.value("alwayshigh" ).toInt() << endl
				<< "Group                   : " << settings.value("group" ).toString() << endl
				<< "Screensaver Port        : " << settings.value("ssaverport").toUInt() << endl 
				<< "Screensaver Bin         : " << settings.value("ssaverbin").toString() << endl ;
				return 0 ;		
		}
	}
	else if ( args.size() == 3 )
	{
		bool flag ;
		if ( args.at(1) == "setmasterip" )
		{
			QHostAddress ip ;
			ip.setAddress ( args.at(2) ) ;
			if ( ! ip.isNull() )
			{
				settings.setValue ( "masterip" , QVariant ( args.at(2) ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setmasterport" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() > 0 )
			{
				settings.setValue ( "masterport" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setlistenport" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() > 0 )
			{
				settings.setValue ( "listenport" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setmaxtasks" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() > 0 )
			{
				settings.setValue ( "maxtasks" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setpriority" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() >= 0 )
			{
				settings.setValue ( "alwayshigh" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setssaverport" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() >= 0 )
			{
				settings.setValue ( "ssaverport" , QVariant ( (quint16)args.at(2).toUInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setssaverbin" )
		{
			settings.setValue ( "ssaverbin" , QVariant ( args.at(2) ) ) ;
			return 0 ;
		}
		else if ( args.at(1) == "setgroup" )
		{
			settings.setValue ( "group" , QVariant ( args.at(2) ) ) ;
			return 0 ;	
		}
	}
	QTextStream out ( stdout ) ;
	out << "Usage :" << endl
		<< "run                    Run VC Slave" << endl 
		<< "[help]                 Show Help" << endl
		<< "info                   Show configuration info" << endl
		<< "setmasterip ip         Set slave master ip" << endl
		<< "setmasterport port     Set slave master port" << endl
		<< "setlistenport port     Set slave listen port" << endl
		<< "setmaxtasks tasks      Set slave max tasks" << endl
		<< "setpriority priority   Set if slave is always on high priority" << endl
		<< "setgroup group         Set slave group " << endl
		<< "setssaverport port     Set ssaver port " << endl
		<< "setssaverbin file      Set ssaver bin " << endl ;
	return 0 ;

}


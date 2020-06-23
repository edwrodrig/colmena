#include <QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include "mastermanager.hpp"
#include <QTimer>
#include <signal.h>

void catch_signal(int sig)
{
	qWarning( "SIGNAL CATCHED : %d" , sig ) ;
	exit(1) ;
}

int main ( int argc , char *argv[] )
{
	signal ( SIGSEGV , catch_signal ) ;	


	QCoreApplication app( argc , argv ) ;
    QCoreApplication::setOrganizationName("Colmena");
    QCoreApplication::setApplicationName("Master");

	//QTimer::singleShot(180000, &app, SLOT(quit()));

	QSettings settings ;
	if ( ! settings.contains ( "slaveport") ) settings.setValue ( "slaveport" , QVariant ( 40000 ) ) ;
	if ( ! settings.contains ( "clientport" ) ) settings.setValue ( "clientport" , QVariant ( 50000 ) ) ;
	if ( ! settings.contains ( "timeout" ) ) settings.setValue ( "slavetimeout" , QVariant ( 30000 ) ) ;	
	if ( ! settings.contains ( "rootpassword" ) ) settings.setValue ( "rootpassword" , QVariant ( QString ( "pass" ) ) ) ;	
	if ( ! settings.contains ( "generallist" ) ) settings.setValue ( "generallist" , QVariant ( QString ( "" ) ) ) ;	
	
	QStringList args = app.arguments() ;
	
	if ( args.size() == 1 )
	{
		MasterManager master ;
		master.init() ;
	
		return app.exec() ;
	}
	else if ( args.size() == 2 )
	{	
		if ( args.at(1) == "info" )
		{
			QTextStream out ( stdout ) ;
			out << "Slave Listen Port  : " << settings.value("slaveport" ).toInt() << endl
				<< "Client Listen Port : " << settings.value("clientport" ).toInt() << endl 
				<< "Slave Timeout      : " << settings.value("slavetimeout" ).toInt() << endl
				<< "Root Password      : " << settings.value("rootpassword" ).toString() << endl ;
				return 0 ;		
		}
	}
	else if ( args.size() == 3 )
	{
		bool flag ;
		if ( args.at(1) == "setport" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() > 0 )
			{
				settings.setValue ( "slaveport" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setclientport" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() > 0 )
			{
				settings.setValue ( "clientport" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setslavetimeout" )
		{
			args.at(2).toInt ( &flag ) ;
			if ( flag && args.at(2).toInt() > 0 )
			{
				settings.setValue ( "slavetimeout" , QVariant ( args.at(2).toInt() ) ) ;
				return 0 ;
			}
		}
		else if ( args.at(1) == "setrootpassword" )
		{
			settings.setValue ( "rootpassword" , QVariant ( args.at(2) ) ) ;
			return 0 ;	
		}
	}
	QTextStream out ( stdout ) ;
	out << "Usage :" << endl
		<< "master                        Run VC Master" << endl 
		<< "master [help]                 Show Help" << endl
		<< "master info                   Show configuration info" << endl
		<< "master setslaveport port      Set master slave listen port" << endl
		<< "master setclientport port     Set master client listen port" << endl
		<< "master setslavetimeout msecs  Set master slave delete timeout time " << endl
		<< "master setrootpassword pass   Set master root password " << endl ;
	return 0 ;
}


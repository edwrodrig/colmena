#include <QTextStream>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QList>
#include <QSettings>
#include <slavelist.hpp>
#include <joblist.hpp>
#include <mastermanager.hpp>
#include <masterslavecommunicator.hpp>
#include <masterclientlistener.hpp>
#include <masterkernel.hpp>
#include <QDir>
#include <QFile>
#include <slave.hpp>

MasterManager::MasterManager ( void ) : QObject () { ; }

MasterManager::~MasterManager ( void )
{
	if ( MasterKernel::slaveCom ) delete MasterKernel::slaveCom ;
	if ( MasterKernel::clientCom ) delete MasterKernel::clientCom ;
}

bool MasterManager::init ( void )
{
        QTextStream out ( stdout ) ;
        out << "======================================\n        Virtual Cluster System\n    by Edwin Rodriguez Leon (2009)\n======================================\n           Master App v0.01\n======================================" ;

	if ( initConfig() && initDatabase() && initConnections() ) return true ;
	return false ;
}

bool MasterManager::initConfig ( void )
{
	QTextStream out ( stdout ) ;
        out << tr ("Loading configuration...") << endl ;
	
	QSettings settings ;

	slaveport = settings.value ( "slaveport" ).toInt() ;
	clientport = settings.value ( "clientport" ).toInt() ;
	slavetimeout = settings.value ( "slavetimeout" ).toInt() ;
	MasterKernel::rootpassword = settings.value ( "rootpassword" ).toString() ;
	Slave::generalList = settings.value ( "generallist" ).toString() ;
        out << tr("Configuration loaded.") << endl ;
	
	QDir dir ;
	if ( dir.exists("jobs") || dir.mkdir("jobs") )
	{
		dir.cd ( "jobs" ) ;
		QStringList list = dir.entryList ( QDir::Files ) ;
		for ( int i = 0 ; i < list.size() ; i ++ ) QFile::remove ( dir.absoluteFilePath( list.at(i) ) ) ;
	}
	else return false ;
	
	return true ;
}

bool MasterManager::initDatabase ( void )
{
	QTextStream out ( stdout ) ;
        out << tr("Creating Database...") << endl ;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , "mastermanager" ) ;
            db.setDatabaseName("bd.db") ;
            if ( ! db.open() )
            {
                    out << tr("Database can't be opened.") << endl ;
                    return false ;
            }
            QSqlQuery query ( db ) ;
            query.exec ( "CREATE TABLE users ( user TEXT , pass TEXT , PRIMARY KEY ( user ) ) ;" ) ;
            query.exec ( "CREATE TABLE groups ( name TEXT , url TEXT , user TEXT , PRIMARY KEY ( name ) ) ;" ) ;
        }
        QSqlDatabase::removeDatabase ( "mastermanager" ) ;
        out << tr("Database opened.") << endl ;
	return true ;
}

bool MasterManager::initConnections ( void )
{
	QTextStream out ( stdout ) ;
        out << tr("Init server...") << endl ;

        MasterKernel::slaveCom = new MasterSlaveCommunicator ( slaveport ) ;

	connect ( MasterKernel::slaveCom , SIGNAL ( stateArrived ( const Message & ) ) , this , SLOT ( stateArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( fullStateArrived ( const Message & ) ) , this , SLOT ( fullStateArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( acceptedTaskArrived ( const Message & ) ) , this , SLOT ( acceptedTaskArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( sendAckArrived ( const Message & ) ) , this , SLOT ( sendAckArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( sendArrived ( const Message & ) ) , this , SLOT ( sendArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( errorTaskArrived ( const Message & ) ) , this , SLOT ( errorTaskArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( notHaveAppArrived ( const Message & ) ) , this , SLOT ( notHaveAppArrived ( const Message & ) ) ) ;
	connect ( MasterKernel::slaveCom , SIGNAL ( pageListAckArrived ( const Message & ) ) , this , SLOT ( pageListAckArrived ( const Message & ) ) ) ;	

        out << tr("Server running at port ") << MasterKernel::slaveCom->serverPort() << " ." << endl ;

	MasterKernel::clientCom = new MasterClientListener () ;

	if ( ! MasterKernel::clientCom->listen ( QHostAddress::Any , clientport ) )
	{
                out << tr("Server can't listen clients. ") << endl ;
		return false ;
	}

	connect ( & slaveTimeoutTimer , SIGNAL ( timeout () ) , this , SLOT ( slaveTimeout () ) ) ;
	slaveTimeoutTimer.start( slavetimeout ) ;
	
        out << tr ( "Listening clients at port " ) << clientport << " ." << endl ;


	return true ;
}

void MasterManager::slaveTimeout ( void )
{
	MasterKernel::slaveList.timeoutChecker () ;
}

void MasterManager::stateArrived ( const Message & m ) { MasterKernel::slaveList.stateArrived ( m ) ; }

void MasterManager::fullStateArrived ( const Message & m ) { MasterKernel::slaveList.fullStateArrived ( m ) ; }

void MasterManager::acceptedTaskArrived ( const Message & m ) { MasterKernel::jobList.acceptedTaskArrived ( m ) ; }

void MasterManager::sendAckArrived ( const Message & m ) { MasterKernel::jobList.sendAckArrived ( m ) ; }

void MasterManager::sendArrived ( const Message & m ) { MasterKernel::jobList.sendArrived ( m ) ; }

void MasterManager::pageListAckArrived ( const Message & m ) { MasterKernel::slaveList.pageListAckArrived ( m ) ; }

void MasterManager::errorTaskArrived ( const Message & m ) { MasterKernel::jobList.errorTaskArrived ( m ) ; }

void MasterManager::notHaveAppArrived ( const Message & m ) { MasterKernel::slaveList.notHaveAppArrived ( m ) ; }

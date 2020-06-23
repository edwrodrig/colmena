#include <slavemanager.hpp>
#include <slavekernel.hpp>
#include <slavemastercommunicator.hpp>
#include <messagerepeater.hpp>
#include <QSettings>
#include <QTcpSocket>
#include <QSqlQuery>
#include <QVariant>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <slavessaverdef.hpp>
#include <QHostAddress>
#include <taskindex.hpp>
#include <inputstream.hpp>
#include <QProcess>

SlaveManager::SlaveManager ( void ) : fullStateRepeater ( 0L ) { ; }

SlaveManager::~SlaveManager ( void )
{
	if ( SlaveKernel::masterCom ) delete SlaveKernel::masterCom ;
}

bool SlaveManager::init ( void )
{
	SlaveKernel::out << "======================================"  << endl
	                 << "        Colmena Virtual Cluster"         << endl
	                 << "    by Edwin Rodriguez Leon (2009)"      << endl
	                 << "======================================"  << endl
	                 << "            Slave App v" << SLAVEVERSION << endl
	                 << "======================================"  << endl ;
	QDir::setCurrent( QString ( APP_PATH ) ) ;
	clearJobs () ;	
	if ( initConfig () && initDatabase() && initSSaverListener() && initApps() && initConnections() ) return true ; 
	return false ;
}

bool SlaveManager::initConfig ( void )
{
	QSettings settings ( QSettings::SystemScope , "Colmena" , "Slave" ) ;
	SlaveKernel::out << "Loading configuration..." << endl ;
	
	SlaveKernel::masterip = QHostAddress ( settings.value("masterip" ).toString() ) ;
	SlaveKernel::masterport = settings.value("masterport" ).toInt() ;
	SlaveKernel::listenport = settings.value("listenport" ).toInt() ;
	SlaveKernel::maxtasks = settings.value("maxtasks" ).toInt() ;
	SlaveKernel::alwayshigh = settings.value("alwayshigh" ).toInt() ;
	SlaveKernel::group = settings.value("group" ).toString() ;
	SlaveKernel::ssaverport = settings.value("ssaverport").toUInt() ;
	
	SlaveKernel::out << "Configuration loaded." << endl ;	
	return true ;
}

bool SlaveManager::initApps ( void )
{
	SlaveKernel::out << "Loading apps..." << endl ;
	checkApps ( QDir ( "apps" ) ) ;
	SlaveKernel::out << "Loading environment vars." << endl ;	
	checkEnvVars ( QDir ( "envvars" ) ) ;
	return true ;
}


bool SlaveManager::initDatabase ( void )
{
	SlaveKernel::out << tr("Opening Database...") << endl ;
	SlaveKernel::db = QSqlDatabase::addDatabase("QSQLITE") ; 
	SlaveKernel::db.setDatabaseName("bd.db") ;
	if ( ! SlaveKernel::db.open() )
	{
		SlaveKernel::out << tr("Database can't be opened.") << endl ;
		return false ;
	}
	QSqlQuery query ( SlaveKernel::db ) ;
	if ( ! query.exec ( "CREATE TABLE apps( name TEXT , app TEXT , PRIMARY KEY(name));" ) ) query.exec ( "DELETE FROM apps ;") ;
	query.exec ( "CREATE TABLE history( app TEXT , completed INTEGER, error INTEGER , other INTEGER , completedtime INTEGER , errortime INTEGER , othertime INTEGER );" ) ;

	SlaveKernel::out << tr("Database opened.") << endl ;
	return true ;
}


bool SlaveManager::initConnections ( void )
{
	QTextStream out ( stdout ) ;
	SlaveKernel::out << "Init communicator..." << endl ;
	SlaveKernel::masterCom = new SlaveMasterCommunicator ( SlaveKernel::listenport ) ;
	
	connect ( SlaveKernel::masterCom , SIGNAL ( registeredArrived ( const Message & ) ) , this , SLOT ( registeredArrived ( const Message & ) ) ) ;
	connect ( SlaveKernel::masterCom , SIGNAL ( fullStateAckArrived ( const Message & ) ) , this , SLOT ( fullStateAckArrived ( const Message & ) ) ) ;
	connect ( SlaveKernel::masterCom , SIGNAL ( postTaskArrived ( const Message & ) ) , this , SLOT ( postTaskArrived ( const Message & ) ) ) ;
	connect ( SlaveKernel::masterCom , SIGNAL ( freeTaskArrived ( const Message & ) ) , this , SLOT ( freeTaskArrived ( const Message & ) ) ) ;
	connect ( SlaveKernel::masterCom , SIGNAL ( sendArrived ( const Message & ) ) , this , SLOT ( sendArrived ( const Message & ) ) ) ;
	connect ( SlaveKernel::masterCom , SIGNAL ( sendAckArrived ( const Message & ) ) , this , SLOT ( sendAckArrived ( const Message & ) ) ) ;
	connect ( SlaveKernel::masterCom , SIGNAL ( pageListArrived ( const Message & ) ) , this , SLOT ( pageListArrived ( const Message & ) ) ) ;

	SlaveKernel::out << "Communicator running at port " << SlaveKernel::listenport << " ." << endl ;

	connect ( & stateTimer , SIGNAL ( timeout() ) , this , SLOT ( sendState() ) ) ;
	SlaveKernel::out << "Starting sending slave state to server..." << endl ;
	start() ;
	return true ;

}

bool SlaveManager::initSSaverListener ( void )
{
	SlaveKernel::ssaverSocket = 0L ;
	SlaveKernel::out << "Init screensaver listener..." << endl ;
	connect ( & SlaveKernel::ssaverListener , SIGNAL ( newConnection() ) , this , SLOT ( ssaverStarted() ) ) ;
	while ( ! SlaveKernel::ssaverListener.listen ( QHostAddress::LocalHost , SlaveKernel::ssaverport ) ) ;
	SlaveKernel::out << "Listening screensaver." << endl ;
	return true ;
}

void SlaveManager::ssaverStarted ( void )
{
	if ( SlaveKernel::ssaverListener.hasPendingConnections () )
	{
		SlaveKernel::generateHtmlRes() ;
		SlaveKernel::generateHtml() ;
		SlaveKernel::ssaverSocket = SlaveKernel::ssaverListener.nextPendingConnection() ;
		connect ( SlaveKernel::ssaverSocket , SIGNAL ( disconnected() ) , this , SLOT ( ssaverClosed() ) ) ;
		connect ( SlaveKernel::ssaverSocket , SIGNAL ( readyRead() ) , this , SLOT ( ssaverGenerateHtml() ) ) ;
		if ( ! SlaveKernel::alwayshigh )
		{
			SlaveKernel::priority = Message::HIGH ;
			SlaveKernel::taskList.highPriority () ;
			SlaveKernel::out << "Changed to HIGH priority." << endl ;
			start() ;
		}
		sendSSaverPages() ;
	}
	while ( SlaveKernel::ssaverListener.hasPendingConnections () ) SlaveKernel::ssaverListener.nextPendingConnection() ;
}

void SlaveManager::ssaverClosed ( void )
{
	SlaveKernel::ssaverSocket = 0L ;
	if ( ! SlaveKernel::alwayshigh )
	{	
		SlaveKernel::priority = Message::LOW ;
		SlaveKernel::taskList.lowPriority () ;
		SlaveKernel::out << "Changed to LOW priority" << endl ;
		start() ;
	}
}

void SlaveManager::clearJobs ( void )
{
	QDir dir ;
	if ( dir.exists ( "tasks" ) )
	{
		dir.cd ( "tasks" ) ;
		QStringList dirs = dir.entryList ( QDir::Dirs | QDir::NoDotAndDotDot ) ;
		for ( int i = 0 , size = dirs.size() ; i < size ; ++i )
		{
			dir.cd ( dirs.at(i) ) ;
			QStringList files = dir.entryList ( QDir::Files ) ;
			for ( int j = 0 , s = files.size() ; j < s ; ++j ) QFile::remove ( dir.absoluteFilePath ( files.at(j) ) ) ;
			dir.cdUp() ;
			dir.rmdir ( dirs.at(i) ) ;
		}
		dir.cdUp() ;
	
	}
	else dir.mkdir ( "tasks" ) ;
	if ( ! dir.exists ( "stats" ) ) dir.mkdir ( "stats" ) ;

}

void SlaveManager::checkApps ( QDir dir )
{
	QStringList files = dir.entryList( QDir::Files | QDir::Executable ) ;

	for ( int i = 0 , size = files.size() ; i < size ; ++i )
	{
		QString file = files.at(i) ; 
		QSqlQuery query ( SlaveKernel::db ) ;
		query.prepare ( "INSERT INTO apps ( app , name ) VALUES ( ? , ? ) " ) ;

		query.addBindValue( QVariant ( dir.absoluteFilePath( file ) ) ) ;		
		query.addBindValue( QVariant ( file.replace( QString(".exe") , QString ( "" ) , Qt::CaseInsensitive ) ) ) ;				

		if ( query.exec() ) SlaveKernel::out << "Added ( "  << file << " ) to app list." << endl ;	
	}
	QStringList dirs = dir.entryList ( QDir::Dirs | QDir::NoDotAndDotDot ) ;
	for ( int i = 0 ; i < dirs.size() ; ++i )
	{
		dir.cd ( dirs.at(i) ) ;
		checkApps ( dir ) ;
		dir.cdUp() ;
	}
}

void SlaveManager::checkEnvVars ( QDir dir )
{
	SlaveKernel::envVars = QProcess::systemEnvironment();
	QStringList files = dir.entryList( QDir::Files ) ;
	for ( int i = 0 , size = files.size() ; i < size ; ++i )
	{
		QFile file ( dir.absoluteFilePath( files.at(i) ) ) ;
		if ( file.open ( QIODevice::ReadOnly ) )
		{
			QTextStream stream ( &file ) ;
			QString var  = stream.readLine() ;
			QString value = stream.readLine() ;
			while ( var.size() > 0 )
			{
				if ( var.toUpper() == "PATH" )
				{
					SlaveKernel::envVars.replaceInStrings ( QRegExp ( "^PATH=(.*)" , Qt::CaseInsensitive ), var + "=\\1;" + value ) ;
				}
				else
				{
					SlaveKernel::envVars << var  + "=" + value ;
				}
				SlaveKernel::out << "Added ( "  << var <<  "=" << value << " ) to environment vars list." << endl ;
				var  = stream.readLine() ;
				value = stream.readLine() ;
			}
		}
	}
}

void SlaveManager::sendState ( void )
{
	Message m ( Message::STATE , SlaveKernel::masterip , SlaveKernel::masterport ) ;
	m.setPriority ( SlaveKernel::priority || SlaveKernel::alwayshigh ) ;
	m.setIdslave ( SlaveKernel::idslave ) ;
	m.setNumtasks ( SlaveKernel::taskList.count() ) ;
	SlaveKernel::masterCom->send ( m ) ;
}

void SlaveManager::registeredArrived ( const Message & m )
{
	SlaveKernel::idslave = m.idslave() ;
	SlaveKernel::out << "Slave registered as slave " << m.idslave() << endl ;
	SlaveKernel::taskList.clear() ;
	SlaveKernel::pageId = 0 ;
	delete fullStateRepeater ;
	fullStateRepeater = new MessageRepeater ( m.createFullState ( SlaveKernel::group , SlaveKernel::maxtasks ) , SlaveKernel::masterCom ) ;
}

void SlaveManager::fullStateAckArrived ( const Message & m )
{
	if ( SlaveKernel::pageId == 0 )
	{
		SlaveKernel::generalPageList = m.generallist() ;
		SlaveKernel::specificPageList = m.specificlist() ;
		sendSSaverPages() ;
	}
	delete fullStateRepeater ;
	fullStateRepeater = 0L ;
	SlaveKernel::out << "Slave received pageLists : " << m.generallist() << " " << m.specificlist() << endl ;
	start() ;
}

void SlaveManager::postTaskArrived ( const Message & m )
{
	SlaveKernel::taskList.postTaskArrived ( m ) ;
}

void SlaveManager::freeTaskArrived ( const Message & m )
{
	SlaveKernel::taskList.clearTask ( TaskIndex ( m.idjob() , m.idtask() ) ) ;
	start() ;
}

void SlaveManager::sendArrived ( const Message & m )
{
	SlaveKernel::taskList.sendArrived ( m ) ;
}

void SlaveManager::sendAckArrived ( const Message & m )
{
	SlaveKernel::taskList.sendAckArrived ( m ) ;
	if ( m.numpack() == -1 ) start() ;
}
	
void SlaveManager::pageListArrived ( const Message & m )
{
	if ( SlaveKernel::pageId < m.idpage() )
	{
		SlaveKernel::generalPageList = m.generallist() ;
		SlaveKernel::specificPageList = m.specificlist() ;	
		sendSSaverPages() ;
	}
	SlaveKernel::masterCom->send ( m.createAck() ) ;
	SlaveKernel::out << "Slave received pageLists : " << m.generallist() << " " << m.specificlist() << endl ; 
}

void SlaveManager::sendSSaverPages( void )
{
	if ( SlaveKernel::ssaverSocket )
	{
		InputStream input ;
		input.in() << (quint8) SLAVESSAVER_CHANGEPAGELIST << SlaveKernel::generalPageList << SlaveKernel::specificPageList ;
		SlaveKernel::ssaverSocket->write ( input.data() ) ;
	}
}

void SlaveManager::ssaverGenerateHtml ( void )
{
	if ( SlaveKernel::ssaverSocket )
	{
		while ( SlaveKernel::ssaverSocket->bytesAvailable() < sizeof(quint8) ) if ( ! SlaveKernel::ssaverSocket->waitForReadyRead( 30000 ) ) return ;
		SlaveKernel::generateHtml() ;
		InputStream input ;
		input.in() << (quint8) SLAVESSAVER_GENERATEDHTML ;
		SlaveKernel::ssaverSocket->write ( input.data() ) ;
	}
}

void SlaveManager::start ( void )
{
	if ( ! SlaveKernel::alwayshigh )
	{
		if ( SlaveKernel::priority == Message::LOW )
		{
			if ( SlaveKernel::taskList.isEmpty() ) stateTimer.stop () ;
		}
		else if ( SlaveKernel::priority == Message::HIGH )
		{
			stateTimer.start ( 15000 ) ;
			sendState() ;
		}
	}
	else
	{
		stateTimer.start ( 15000 ) ;
		sendState() ;
	}
}	

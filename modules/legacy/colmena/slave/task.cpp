#include <task.hpp>
#include <messagerepeater.hpp>
#include <datareceiver.hpp>
#include <datasender.hpp>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QVariant>
#include <QProcess>
#include <slavekernel.hpp>
#include <slavessaverdef.hpp>
#include <slavemastercommunicator.hpp>
#include <inputstream.hpp>

Task::Task ( const Message & m , const QString & filePath , const QString & app ) : QObject () , message ( m ) , program ( app ) , command ( filePath )
{
	repeater = new MessageRepeater ( m.createAcceptedTask()  , SlaveKernel::masterCom ) ;
	dataReceiver = new DataReceiver ( m , SlaveKernel::masterCom ) ;
	dataSender = 0L ;
	process = 0L ;
	_state = WAITING ;
}

Task::~Task ( void )
{
	if ( repeater ) delete repeater ;
	if ( dataReceiver ) delete dataReceiver ;
	if ( dataSender ) delete dataSender ;
	if ( process )
	{
		delete process ;
		SlaveKernel::addHistoryEntry ( program , HISTORYACTION_OTHER , startTime.secsTo ( QDateTime::currentDateTime() ) ) ;
	}
	deleteDir() ;
}

void Task::deleteDir ( void )
{
	QString dirName = QString("job").append ( QString::number( message.idjob() ) ).append ( "task").append ( QString::number ( message.idtask() ) ) ;
	if ( dir.dirName () == dirName )
	{
		QStringList files = dir.entryList( QDir::Files ) ;
		for ( int i = 0 ; i < files.size() ; ++i ) dir.remove ( files.at(i) ) ;
		dir.cdUp() ;
		dir.rmdir( dirName ) ;
	}
}

void Task::sendArrived ( const Message & m )
{
	if ( repeater )
	{
		delete repeater ;
		repeater = 0L ;
		_state = RECEIVINGINPUT ;
	}
	if ( dataReceiver ) dataReceiver->receivePart ( m ) ;
	if ( m.numpack() == -1 )
	{
		if ( dataReceiver )
		{
			QString dirName = QString("job").append ( QString::number( message.idjob() ) ).append ( "task").append ( QString::number ( message.idtask() ) ) ;		
			dir.cd ( "tasks" ) ;
			SlaveKernel::out << "Task " << m.idjob() << "-" << m.idtask() << " : Dir created " << dirName << endl ;
			if ( dir.exists ( dirName ) ) deleteDir() ;
			if ( dir.mkdir ( dirName ) )
			{
				dir.cd ( dirName ) ;
				if ( process ) delete process ;
				process = new QProcess() ;
				connect ( process , SIGNAL ( finished ( int , QProcess::ExitStatus ) ) , this , SLOT ( finished( int , QProcess::ExitStatus ) ) ) ;	
				process->setWorkingDirectory( dir.absolutePath() ) ;
				process->setEnvironment ( SlaveKernel::envVars ) ;
				//SlaveKernel::out << "Task Environment: " << SlaveKernel::envVars.join(" : ") << endl ;
				
				//process->setStandardErrorFile ( QString ( "C:\\" ).append ( QString::number( message.idjob() ) ).append ( "-").append ( QString::number ( message.idtask() ) ) ) ;

				QDataStream stream ( dataReceiver->data () , QIODevice::ReadOnly ) ;
				stream.setVersion( QDataStream::Qt_4_1 ) ;
				QString fileName ;
				QString fileData ;
				stream >> fileName ;	// ARGUMENTOS
				command.append(" ").append ( fileName ) ;
							
				stream >> fileData ;	// STDIN
				if ( fileData.size() > 0 )
				{
					QFile file ( dir.filePath( "stdin" ) ) ;
					if ( file.open( QIODevice::WriteOnly ) )
					{
						QTextStream sfile ( &file ) ;
						sfile << fileData ;
						file.close() ;
						process->setStandardInputFile ( dir.filePath("stdin") ) ;
					}
					else return error() ;				
				}
				stream >> fileName ;	// RETRIEVE LIST
				retrieve = fileName.split(",") ;
				if ( retrieve.size() == 0 ) retrieve << "stdout" ;
				stream >> fileName ;	// FILES
				while ( fileName != "." )
				{
					stream >> fileData ;
					QFile file ( dir.filePath( fileName ) ) ;
					if ( file.open( QIODevice::WriteOnly ) )
					{
						SlaveKernel::out << "Task " << m.idjob() << "-" << m.idtask() << " : Infile created " << fileName << endl ;
						QTextStream sfile ( &file ) ;
						sfile << fileData ;
						file.close() ;
					}
					else return error () ;
					stream >> fileName ;
				}	
				process->setStandardOutputFile ( dir.filePath("stdout") ) ;
				
				//EJECUTAR
				SlaveKernel::out << "Task " << m.idjob() << "-" << m.idtask() << " : Executing " << command << endl ;
				process->start ( command ) ; 
				
				if ( process->waitForStarted( 10000 ) )
				{
					_state = PROCESSING ;
					startTime = QDateTime::currentDateTime() ;
					SlaveKernel::out << "Task " << m.idjob() << "-" << m.idtask() << " : Started" << endl ;
					if ( SlaveKernel::alwayshigh == 0 && SlaveKernel::priority == Message::LOW ) lowPriority() ;
					else highPriority() ;
				}
				else return error() ;
			}
			else return error() ;
			
			delete dataReceiver ;
			dataReceiver = 0L ;
		}
		else SlaveKernel::masterCom->send ( m.createAck() ) ;
	}
}

void Task::finished ( int exitCode , QProcess::ExitStatus status )
{
	SlaveKernel::out << "Task " << message.idjob() << "-" << message.idtask() << " : Finished " << endl ;

	if ( status == QProcess::CrashExit )
	{ 
		SlaveKernel::out << "Task " << message.idjob() << "-" << message.idtask() << " : Crashed at finish." << endl ;
		return error() ;
	} 
	
	(void)exitCode ;

	
	InputStream input ( false ) ;
	QString fileData ;

	for ( int i = 0 ; i < retrieve.size() ; i ++ )
	{
		QFile file ( dir.filePath( retrieve.at(i) ) ) ;
		if ( file.open( QIODevice::ReadOnly ) )
		{
			SlaveKernel::out << "Task " << message.idjob() << "-" << message.idtask() << " : Retrieving file " << retrieve.at(i) << endl ;
			QTextStream sfile ( &file ) ;
			input.in() << retrieve.at(i) << sfile.readAll() ;
			file.close() ;
		}
		else
		{
			SlaveKernel::out << "Task " << message.idjob() << "-" << message.idtask() << " : File " << retrieve.at(i) << " not found at retrieving." << endl ;
			return error() ;
		}
	}
	input.in() << QString ( "." ) ;
	deleteDir() ;
	if ( process )
	{
		delete process ;
		process = 0L ;
	}
	dataSender = new DataSender ( message.createSend() , input.data( false ) , SlaveKernel::masterCom ) ;
	_state = SENDINGOUTPUT ;
	SlaveKernel::addHistoryEntry ( program , HISTORYACTION_COMPLETED , startTime.secsTo ( QDateTime::currentDateTime() ) ) ;	
	SlaveKernel::out << "Task " << message.idjob() << "-" << message.idtask() << " : Finished. Starting to sending output..." << endl ;
}

void Task::sendAckArrived ( const Message & m )
{
	if ( dataSender ) dataSender->send( m ) ;
}

void Task::error ( void )
{
	int errorCode = (int)process->error();	
	QString errorString = process->errorString();

	if ( repeater )
	{
		delete repeater ;	
		repeater = 0L ;
	}
	if ( process )
	{
		delete process ;
		process = 0L ;
	}
	_state = ERROR ;
	SlaveKernel::addHistoryEntry ( program , HISTORYACTION_ERROR , startTime.secsTo ( QDateTime::currentDateTime() ) ) ;	
	repeater = new MessageRepeater ( message.createErrorTask() , SlaveKernel::masterCom ) ;

	SlaveKernel::out << "Task " << message.idjob() << "-" << message.idtask() << " : Error " << errorCode << " (" << errorString << ") " << ". Notifying to Master... " << endl ;
}

QString Task::app ( void ) const { return program ; }
int Task::idjob ( void ) const { return message.idjob() ; }
int Task::idtask ( void ) const { return message.idtask() ; }
int Task::state ( void ) const { return _state ; }







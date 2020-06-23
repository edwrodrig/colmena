#include <masterclientcommunicator.hpp>
#include <QDataStream>
#include <QVariant>
#include <clientmasterdef.hpp>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <inputstream.hpp>
#include <masterkernel.hpp>
#include <QSettings>
#include <slave.hpp>
#include <QtSql>
#include <job.hpp>
#include <QtDebug>

MasterClientCommunicator::MasterClientCommunicator ( int socketDescriptor ) : QTcpSocket ()
{
        setSocketDescriptor ( socketDescriptor ) ;
        //connect ( this , SIGNAL ( disconnected() ) , this , SLOT ( deleteLater() ) ) ;
}

MasterClientCommunicator::~MasterClientCommunicator ( void )
{
}

bool MasterClientCommunicator::checkIncommingBytes ( void )
{
	quint32 size ;
	if ( ! waitForIncommingBytes( (int)sizeof(quint32) ) ) return false ;
	stream >> size ;
	if ( ! waitForIncommingBytes( (int)size ) )	return false ;
	return true ;
}

bool MasterClientCommunicator::waitForIncommingBytes ( int bytes )
{
	while ( bytesAvailable() < bytes ) if ( ! waitForReadyRead( 600000 ) ) return false ;
	return true ;
}

void MasterClientCommunicator::readClient( void ) {
	stream.setDevice( this );
	stream.setVersion(QDataStream::Qt_4_1);
	quint8 order;
	QString user;
	QString pass;

	if( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }

	stream >> order >> user >> pass ;

	if( ! checkPerms ( user , pass , order ) ) {
		disconnectFromHost();
		return;
	}
	switch( order ) {
		case CLIENTMASTER_JOB | CLIENTMASTER_ADD :
			rcvJobAdd ( user ) ;
			break ;
		case CLIENTMASTER_JOB | CLIENTMASTER_ADL :
			rcvJobAddL ( user ) ;
			break ;
		case CLIENTMASTER_JOB | CLIENTMASTER_DEL :
			rcvJobDel ( user ) ;
			break ;
		case CLIENTMASTER_JOB | CLIENTMASTER_INF :
			rcvJobInf ( user ) ;
			break ;
		case CLIENTMASTER_JOB | CLIENTMASTER_GET :
			rcvJobGetN ( user ) ;
			break ;
		case CLIENTMASTER_JOB | CLIENTMASTER_GEL :
			rcvJobGetL ( user ) ;
			break ;
		case CLIENTMASTER_JOB | CLIENTMASTER_LST :
			rcvJobLst ( user ) ;
			break ;
		case CLIENTMASTER_TASK | CLIENTMASTER_DEL :
			rcvTaskDel ( user ) ;
			break ;
		case CLIENTMASTER_TASK | CLIENTMASTER_INF :
			rcvTaskInf ( user ) ;
			break ;
		case CLIENTMASTER_TASK | CLIENTMASTER_LST :
			rcvTaskLst ( user ) ;
			break ;
		case CLIENTMASTER_GROUP | CLIENTMASTER_ADD :
			rcvGroupAdd () ;
			break ;
		case CLIENTMASTER_GROUP | CLIENTMASTER_SET :
			rcvGroupSet ( user ) ;
			break ;
		case CLIENTMASTER_GROUP | CLIENTMASTER_DEL :
			rcvGroupDel ( user ) ;
			break ;
		case CLIENTMASTER_GROUP | CLIENTMASTER_INF :
			rcvGroupInf ( user ) ;
			break ;
		case CLIENTMASTER_GROUP | CLIENTMASTER_LST :
			rcvGroupLst ( user ) ;
			break ;
		case CLIENTMASTER_USER | CLIENTMASTER_ADD :
			rcvUserAdd () ;
			break ;
		case CLIENTMASTER_USER | CLIENTMASTER_SET :
			rcvUserSet ( user ) ;
			break ;
		case CLIENTMASTER_USER | CLIENTMASTER_DEL :
			rcvUserDel () ;
			break ;
		case CLIENTMASTER_USER | CLIENTMASTER_LST :
			rcvUserLst () ;
			break ;
		case CLIENTMASTER_USER | CLIENTMASTER_INF :
			rcvUserInf () ;
			break ;
		case CLIENTMASTER_SLAVE | CLIENTMASTER_DEL :
			rcvSlaveDel () ;
			break ;
		case CLIENTMASTER_SLAVE | CLIENTMASTER_INF :
			rcvSlaveInf () ;
			break ;
		case CLIENTMASTER_SLAVE | CLIENTMASTER_LST :
			rcvSlaveLst () ;
			break ;
	}
}

void MasterClientCommunicator::rcvJobAdd ( const QString & user )
{
        QByteArray array ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	
        Job * job = MasterKernel::jobList.postJob ( user ) ;

        QFile file ( QString("jobs/input%1").arg ( job->id() ) ) ;
	if ( file.open ( QIODevice::WriteOnly ) )
	{
		stream >> array ;
		file.write( qUncompress ( array ) ) ;
		file.close() ;
	}
        else { delete job ; disconnectFromHost() ; return ; }
        qDebug("rcvJobAdd: User %s posted a job with id : %d" , user.toAscii().data() , job->id() ) ;
        MasterKernel::jobList.addJob ( job ) ;
        InputStream input ;
        input.in() << job->id() ;
	write ( input.data() ) ;
	disconnectFromHost() ;
}

void MasterClientCommunicator::rcvJobAddL( const QString & user ) {		
	if( !checkIncommingBytes() ) { 
		disconnectFromHost(); 
		return; 
	}
	
	QString fileName;
	stream >> fileName;

	Job *job = MasterKernel::jobList.postJob(user);
	int currentJob = job->id();

	if( ! QFile::rename( fileName, QString("jobs/input").append( QString::number( currentJob ) ) ) ) { 
		MasterKernel::jobList.clearJob( user, currentJob ); 
		disconnectFromHost(); 
		return; 
	}
        qDebug("rcvJobAddL: User %s posted a job with id: %d\n", user.toAscii().data(), currentJob);
	
	InputStream input;
	input.in() << currentJob;
	write( input.data() );
	disconnectFromHost();
}

void MasterClientCommunicator::rcvJobDel ( const QString & user )
{
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	qint32 idjob ;
	stream >> idjob ;
        qDebug( "User %s deleting job with id : %d" , user.toAscii().data() , idjob ) ;
	MasterKernel::jobList.clearJob ( user , idjob ) ;
	disconnectFromHost() ;	
}

void MasterClientCommunicator::rcvJobInf ( const QString & user )
{
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	qint32 idjob ;
	stream >> idjob ;
        qDebug( "User %s is getting info of job %d\n" , user.toAscii().data() , idjob ) ;
	InputStream input ;
	MasterKernel::jobList.infoJob ( user , idjob , input ) ;
	write ( input.data() ) ;	
	disconnectFromHost() ;
}

void MasterClientCommunicator::rcvJobGet ( const QString & user )
{
	qint32 idJob = 0 ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> idJob ;

	QByteArray data ;
	if ( ! MasterKernel::jobList.isJobCompleted( user , idJob ) ) { disconnectFromHost() ; return ; }
        qDebug( "User %s is getting output of job %d\n" , user.toAscii().data() , idJob ) ;
	QFile file ( QString("jobs/output").append ( QString::number( idJob ) ) ) ;
	if ( file.open ( QIODevice::ReadOnly ) )
	{
		data = qCompress ( file.readAll () ) ;
		file.close() ;
		
		InputStream input ;
		input.in() << data ;
		write ( input.data() ) ;		
	}
	disconnectFromHost() ;	
}

void MasterClientCommunicator::rcvJobGetN ( const QString & user )
{
	qint32 idJob = 0 ;
        if ( ! checkIncommingBytes() ) { qDebug( "masterclientcom : falcheck" ) ; disconnectFromHost() ; return ; }
	stream >> idJob ;

	QByteArray data ;
        if ( ! MasterKernel::jobList.isJobCompleted( user , idJob ) ) { qDebug( "masterclientcom : iscompletedfail" ) ; disconnectFromHost() ; return ; }
        qDebug( "User %s is getting output of job %d\n" , user.toAscii().data() , idJob ) ;
	QFile file ( QString("jobs/output").append ( QString::number( idJob ) ) ) ;
	if ( file.open ( QIODevice::ReadOnly ) )
	{
		stream << (qint64)file.size() ;
                
		while ( ! file.atEnd() )
		{
                        write ( file.read( 4000 ) ) ;
			waitForBytesWritten() ;

		}			
		file.close() ;		
	}
	else qDebug ( "Error opening file %d" , file.error() ) ;
	disconnectFromHost() ;	
}

void MasterClientCommunicator::rcvJobGetL ( const QString & user )
{
	qint32 idJob = 0 ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> idJob ;
	
	if ( ! MasterKernel::jobList.isJobCompleted( user , idJob ) ) { disconnectFromHost() ; return ; }
        qDebug( "User %s is getting output of local job %d\n" , user.toAscii().data() , idJob ) ;
	QFileInfo file ( QString("jobs/output").append ( QString::number( idJob ) ) ) ;
	if ( file.exists () )
	{	
		InputStream input ;
		input.in() << file.absoluteFilePath() ;
		write ( input.data() ) ;		
	}
	disconnectFromHost() ;	
	
}
void MasterClientCommunicator::rcvJobLst ( const QString & user )
{
	InputStream input ;
	MasterKernel::jobList.jobList ( user , input ) ;
        qDebug( "User %s is getting job list...\n" , user.toAscii().data() ) ;
	write ( input.data() ) ;
	disconnectFromHost() ;
}

void MasterClientCommunicator::rcvTaskDel ( const QString & user )
{
	qint32 idJob = 0 ;
	qint32 idTask = 0 ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> idJob >> idTask ;
        qDebug( "User %s deleting task %d-%d\n" , user.toAscii().data() , idJob , idTask ) ;
	MasterKernel::jobList.clearTask ( user , idJob , idTask ) ;
	disconnectFromHost() ;
}
void MasterClientCommunicator::rcvTaskInf ( const QString & user )
{
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	qint32 idjob ;
	qint32 idtask ;
	stream >> idjob >> idtask ;
        qDebug( "User %s is getting info of task %d-%d\n" , user.toAscii().data() , idjob , idtask ) ;
	InputStream input ;
	MasterKernel::jobList.infoTask ( user , idjob , idtask , input ) ;
	write ( input.data() ) ;	
	disconnectFromHost() ;
}

void MasterClientCommunicator::rcvTaskLst ( const QString & user )
{
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	qint32 idjob ;
	stream >> idjob ;
        qDebug( "User %s  is getting task list of job %d\n" , user.toAscii().data() , idjob ) ;
	InputStream input ;
	MasterKernel::jobList.taskList ( user , idjob , input ) ;
	write ( input.data() ) ;	
	disconnectFromHost() ;
}
void MasterClientCommunicator::rcvGroupAdd ( void )
{
	QString group ;
	QString url ;
	QString user ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> group >> url >> user ;
	if ( group == "general" ) return ;
	
        {
            QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
            db.setDatabaseName ( "bd.db" ) ;
            db.open() ;
            QSqlQuery query ( db ) ;
            query.prepare ( "INSERT INTO groups VALUES ( ? ,? ,? );" ) ;
            query.addBindValue ( QVariant ( group ) ) ;
            query.addBindValue ( QVariant ( url ) ) ;
            query.addBindValue ( QVariant ( user ) ) ;
            query.exec() ;
        }
        QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	MasterKernel::slaveList.setPageList ( group , url ) ;
	
        qDebug ( "Client create group %s\n" , group.toAscii().data() ) ;
	
}
void MasterClientCommunicator::rcvGroupSet ( const QString & user )
{
	QString group ;
	QString url ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> group >> url ;

	if ( group == "general" )
	{
		QSettings settings ;
		settings.setValue ( "generallist" , QVariant ( url ) ) ;	
		Slave::generalList = url ;
	}
	else
	{
            {
                QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
                db.setDatabaseName ( "bd.db" ) ;
                db.open() ;
                QSqlQuery query ( db ) ;
		if ( user == "root" ) query.prepare ( "UPDATE groups SET url = ? WHERE name = ? ;" ) ;
		else query.prepare ( "UPDATE groups SET url = ? WHERE user = ? AND name = ? ;" ) ;
		query.addBindValue ( QVariant ( url ) ) ;
		if ( user != "root" ) query.addBindValue ( QVariant ( user ) ) ;
		query.addBindValue ( QVariant ( group ) ) ;
		query.exec() ;
            }
            QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	}

	MasterKernel::slaveList.setPageList ( group , url ) ;
		
        QTextStream out ( stdout ) ;
        qDebug ( "User %s update group %s\n", user.toAscii().data() , group.toAscii().data() ) ;
	
}
void MasterClientCommunicator::rcvGroupDel ( const QString & user )
{
	QString group ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> group ;

	if ( group == "general" ) return ;

        {

            QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
            db.setDatabaseName ( "bd.db" ) ;
            db.open() ;
            QSqlQuery query ( db ) ;
            if ( user == "root" )
            {
                    query.prepare ( "DELETE FROM groups WHERE name = ? ;" ) ;
                    query.addBindValue ( QVariant ( group ) ) ;
            }
            else
            {
                    query.prepare ( "DELETE FROM groups WHERE user = ? AND name = ? ;" ) ;
                    query.addBindValue ( QVariant ( user ) ) ;
                    query.addBindValue ( QVariant ( group ) ) ;
            }
            query.exec() ;
        }
        QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	MasterKernel::slaveList.setPageList ( group , "" ) ;
	
        QTextStream out ( stdout ) ;
        out << "User " << user << " delete group " << group << endl ;
	
}
void MasterClientCommunicator::rcvGroupInf ( const QString & user )
{
	QString group ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> group ;

        {

            QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
            db.setDatabaseName ( "bd.db" ) ;
            db.open() ;

            QSqlQuery query ( db ) ;
            if ( user == "root" )
            {
                    query.prepare ( "SELECT url , user FROM groups WHERE name = ? ;" ) ;
                    query.addBindValue ( QVariant ( group ) ) ;

            }
            else
            {
                    query.prepare ( "SELECT url , user FROM groups WHERE user = ? AND name = ? ;" ) ;
                    query.addBindValue ( QVariant ( user ) ) ;
                    query.addBindValue ( QVariant ( group ) ) ;
            }
            if ( query.exec() && query.next() )
            {
                    InputStream input ;
                    input.in() << query.value(0).toString() << query.value(1).toString() ;
                    QTextStream out ( stdout ) ;
                    out << "User " << user << " is getting group information. " << endl ;
                    write ( input.data() ) ;
            }
        }
        QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	disconnectFromHost() ;
}

void MasterClientCommunicator::rcvGroupLst ( const QString & user )
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
        db.setDatabaseName ( "bd.db" ) ;
        db.open() ;
        QSqlQuery query ( db ) ;
	if ( user == "root" ) query.prepare ( "SELECT name FROM groups ;" ) ;
	else
	{
		query.prepare ( "SELECT name FROM groups WHERE user = ? ;" ) ;
		query.addBindValue ( QVariant ( user ) ) ;
	}
	if ( query.exec() )
	{
		InputStream input ;
		while ( query.next() )
		{
			input.in() << query.value(0).toString() ;
		}
                QTextStream out ( stdout ) ;
                out << "User " << user << " is getting group list " << endl ;
		write ( input.data() ) ;	
	}
    }
    QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
    disconnectFromHost() ;
}
void MasterClientCommunicator::rcvUserAdd ( void )
{
	QString newuser ;
	QString pass ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> newuser >> pass ;
	if ( newuser != "root" )
	{
            {
                QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
                db.setDatabaseName ( "bd.db" ) ;
                db.open() ;
                QSqlQuery query ( db ) ;
		query.prepare ( "INSERT INTO users VALUES ( ? , ? ) ;" ) ;
		query.addBindValue ( QVariant ( newuser ) ) ;
		query.addBindValue ( QVariant ( pass ) ) ;
                if ( query.exec() ) qDebug ( "Client created new user %s\n" , newuser.toAscii().data() ) ;
            }
            QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
        }
	disconnectFromHost() ;
}
void MasterClientCommunicator::rcvUserSet ( const QString & user )
{
	QString pass ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> pass ;
	if ( user != "root" )
	{
            {
                QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
                db.setDatabaseName ( "bd.db" ) ;
                db.open() ;
                QSqlQuery query ( db ) ;
		query.prepare ( "UPDATE users SET pass = ? WHERE user = ? ;" ) ;
		query.addBindValue ( QVariant ( pass ) ) ;
		query.addBindValue ( QVariant ( user ) ) ;
                if ( query.exec() ) qDebug ( "User %s change password.\n" , user.toAscii().data() ) ;
            }
            QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
        }
	else
	{
		MasterKernel::rootpassword = pass ;
		QSettings settings ;
		settings.setValue ( "rootpassword" , QVariant ( pass ) ) ;
                QTextStream out ( stdout ) ;
                out << "Root password changed " << endl ;
	}
	disconnectFromHost() ;
}
void MasterClientCommunicator::rcvUserDel ( void )
{
	QString user ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> user ;
	if ( user != "root" )
	{
            {
                QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
                db.setDatabaseName ( "bd.db" ) ;
                db.open() ;
                QSqlQuery query ( db ) ;
		query.prepare ( "DELETE FROM users WHERE user = ? ;" ) ;
		query.addBindValue ( QVariant ( user ) ) ;
		if ( query.exec() )
		{
                        QTextStream out ( stdout ) ;
                        out << "User " << user << " deleted." << endl ;
			MasterKernel::jobList.clearJob ( user ) ;
		}
            }
            QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	}
}
void MasterClientCommunicator::rcvUserLst ( void )
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
        db.setDatabaseName ( "bd.db" ) ;
        db.open() ;
        QSqlQuery query ( db ) ;
	query.prepare ( "SELECT user FROM users ;" ) ;
	if ( query.exec() )
	{
		InputStream input ;
		while ( query.next() )
		{
			input.in() << query.value(0).toString() ;
		}
                QTextStream out ( stdout ) ;
                out << "Client is getting user list " << endl ;
		write ( input.data() ) ;	
	}
    }
    QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	disconnectFromHost() ;
}
void MasterClientCommunicator::rcvUserInf ( void )
{
	QString user ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> user ;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
        db.setDatabaseName ( "bd.db" ) ;
        db.open() ;
        QSqlQuery query ( db ) ;
	query.prepare ( "SELECT pass FROM users WHERE user = ? ;" ) ;
	query.addBindValue ( QVariant ( user ) ) ;
	if ( query.exec() && query.next() )
	{
		InputStream input ;
		input.in() << query.value(0).toString() ;
                QTextStream out ( stdout ) ;
                out << "Client is getting user info of " << user << endl ;
		write ( input.data() ) ;	
	}
            }
    QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	disconnectFromHost() ;
}


void MasterClientCommunicator::rcvSlaveDel ( void )
{
	quint32 idSlave = 0 ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> idSlave ;
        QTextStream out ( stdout ) ;
        out << "Client deleting slave " << idSlave << "..." << endl ;
	MasterKernel::slaveList.deleteSlave ( idSlave ) ;
	disconnectFromHost() ;
	
}
void MasterClientCommunicator::rcvSlaveInf ( void )
{
	InputStream input ;
	quint32 idSlave = 0 ;
	if ( ! checkIncommingBytes() ) { disconnectFromHost() ; return ; }
	stream >> idSlave ;
	MasterKernel::slaveList.infoSlave ( idSlave , input ) ;
	write ( input.data() ) ;
	disconnectFromHost() ;
	
}
void MasterClientCommunicator::rcvSlaveLst ( void )
{
	InputStream input ;
	MasterKernel::slaveList.slaveList ( input ) ;
	write ( input.data() ) ;
	disconnectFromHost() ;
}
	
bool MasterClientCommunicator::checkPerms ( const QString & user , const QString & pass , quint8 order )
{
	quint8 response ;
	
	if ( user == "root" )
	{
		if ( pass == MasterKernel::rootpassword ) response = CLIENTMASTER_LOGIN_OK ;
		else response = CLIENTMASTER_LOGIN_INCORRECTPASS ;
	}
	else
	{
            {
                    QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString::number(socketDescriptor()) ) ;
                    db.setDatabaseName ( "bd.db" ) ;
                    db.open() ;
                    QSqlQuery query ( db ) ;
                    query.prepare ( "SELECT pass FROM users WHERE user = ? " ) ;
                    query.addBindValue ( QVariant ( user ) ) ;
                    query.exec() ;
                    if ( query.next() )
                    {
                            if ( query.value(0).toString() != pass ) response = CLIENTMASTER_LOGIN_INCORRECTPASS ;
                            else
                            {
                                    switch ( order )
                                    {
                                            case CLIENTMASTER_GROUP | CLIENTMASTER_ADD :
                                            case CLIENTMASTER_GROUP | CLIENTMASTER_DEL :
                                            case CLIENTMASTER_USER | CLIENTMASTER_ADD :
                                            case CLIENTMASTER_USER | CLIENTMASTER_DEL :
                                            case CLIENTMASTER_USER | CLIENTMASTER_LST :
                                            case CLIENTMASTER_USER | CLIENTMASTER_INF :
                                            case CLIENTMASTER_SLAVE | CLIENTMASTER_DEL :
                                            case CLIENTMASTER_SLAVE | CLIENTMASTER_INF :
                                            case CLIENTMASTER_SLAVE | CLIENTMASTER_LST :
                                                    response = CLIENTMASTER_LOGIN_NOTACTION ;
                                                    break ;
                                            default :
                                                    response = CLIENTMASTER_LOGIN_OK ;
                                                    break ;
                                    }
                            }
                    }
                    else response = CLIENTMASTER_LOGIN_NOTUSER ;
                }
                QSqlDatabase::removeDatabase ( QString::number(socketDescriptor()) ) ;
	}
	
	InputStream input ;
	input.in() << response ;
	
	if ( write ( input.data() ) == -1 ) return false ;
	while ( waitForBytesWritten() ) ;
	
	if ( response == CLIENTMASTER_LOGIN_OK ) return true ;
	else return false ;

}


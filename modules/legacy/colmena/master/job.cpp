#include <job.hpp>
#include <QSqlQuery>
#include <QVariant>
#include <masterkernel.hpp>
#include <message.hpp>
#include <QFile>
#include <inputstream.hpp>
#include <masterslavecommunicator.hpp>
#include <QSqlError>

int Job::count = 0 ;

Job::Job ( const QString & user ) : _user ( user ) , _completedTasks ( -2 ) , _totalTasks ( -1 )
{
	_id = Job::count ;
	Job::count ++ ;
}

Job::~Job ( void )
{

    QFile::remove ( QString ("jobs/output%1").arg ( _id ) ) ;
    QFile::remove ( QString ("jobs/input%1").arg ( _id ) ) ;
    QFile::remove ( QString ("jobs/works%1").arg ( _id ) ) ;
}

int Job::id ( void ) const { return _id ; } 

QString Job::user ( void ) const { return _user ; }

void Job::open ( void ) {
    {
        QSqlDatabase _outputDb = QSqlDatabase::addDatabase("QSQLITE" , QString ("output%1").arg ( _id ) ) ;
        _outputDb.setDatabaseName( QString ("jobs/output%1").arg ( _id ) ) ;
        QSqlDatabase _inputDb = QSqlDatabase::addDatabase("QSQLITE" , QString ("input%1").arg( _id ) ) ;
        _inputDb.setDatabaseName( QString ("jobs/input%1").arg ( _id ) ) ;
        _inputDb.open() ;
        _outputDb.open() ;
	
        QSqlQuery ( "CREATE TABLE files ( idtask INTEGER , filename TEXT , data TEXT ) ;" , _outputDb ) ;

        QSqlQuery query( _inputDb ) ;
        query.prepare("SELECT count(*) FROM tasks") ;
        if ( query.exec() && query.next() ) {
            _totalTasks = query.value(0).toInt() ;
            _completedTasks = 0 ;
        } else {
		QString tmp = QString("jobs/input%1").arg(_id);
		qDebug("BAD QUERY OPEN %s\n%s\n", query.lastError().text().toAscii().data(), tmp.toAscii().data() ) ;
	}

        query.prepare ( QString("ATTACH DATABASE 'jobs/works%1' as w").arg( _id ) ) ;
        query.exec() ;
        query.prepare ( "CREATE TABLE w.works ( idtask INTEGER , idslave INTEGER , PRIMARY KEY ( idtask ) ) ; " ) ; // idslave = -1 completed
        query.exec()  ;

    }
    QSqlDatabase::removeDatabase ( QString ("input%1").arg ( _id ) ) ;
    QSqlDatabase::removeDatabase ( QString ("output%1").arg ( _id ) ) ;
}

void Job::clearWorkingTasks ( void )
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString ("job%1").arg( _id ) ) ;
        db.setDatabaseName ( QString ("jobs/works%1").arg ( _id ) ) ;
        db.open() ;
        QSqlQuery query ( db ) ;
        query.prepare ( "DELETE FROM works ;" ) ;
	query.exec() ;
	dataSenderList.clear() ;
	dataReceiverList.clear() ;
    }
    QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
}

void Job::clearWorkingTask ( int task )
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString ("job%1").arg( _id ) ) ;
        db.setDatabaseName ( QString ("jobs/works%1").arg ( _id ) ) ;
        db.open() ;
        QSqlQuery query ( db ) ;
        query.prepare ( "DELETE FROM works WHERE idtask = ? AND idslave != -1 ;" ) ;
	query.addBindValue ( QVariant ( task ) ) ;
	query.exec() ;
	dataSenderList.clearTask( task ) ;
	dataReceiverList.clearTask( task ) ;
    }
    QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
}

void Job::clearWorkingSlave ( int slave )
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString ("job%1").arg( _id ) ) ;
        db.setDatabaseName ( QString ("jobs/works%1").arg ( _id ) ) ;
        db.open() ;
        QSqlQuery query ( db ) ;
        query.prepare ( "DELETE FROM works WHERE idslave = ?" ) ;
        query.addBindValue ( QVariant ( slave ) ) ;
        query.exec() ;
        dataSenderList.clearSlaves ( slave ) ;
        dataReceiverList.clearSlaves ( slave ) ;
    }
    QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;

}
bool Job::availableTask ( int slave , int * id , QString * program )
{
    if ( isCompleted() ) return false ;
    bool flag = false ;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE" , QString ("job%1").arg( _id ) ) ;
        db.setDatabaseName( QString ("jobs/input%1").arg ( _id ) ) ;
        db.open() ;

        QSqlQuery query( QString("ATTACH DATABASE 'jobs/works%1' as w").arg ( _id ) , db ) ;

        QString list = MasterKernel::slaveList.notHaveAppList ( slave ) ;
        list = QString ("SELECT rowid, program FROM tasks WHERE rowid NOT IN ( SELECT idtask FROM w.works ) AND program NOT IN ( ").append( list ).append(" ) LIMIT 1 ;") ;

        query.prepare ( list ) ;

        if ( query.exec() )
        {
            if ( query.next() )
            {
                *id = query.value(0).toInt() ;
                *program = query.value(1).toString() ;
                flag = true ;
            }
        }
    }
    QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
    return flag ;
}

bool Job::isCompleted ( void ) const { return _completedTasks >= _totalTasks ; }
int Job::completedTasks ( void ) const { return _completedTasks ; }
int Job::totalTasks ( void ) const { return _totalTasks ; }

bool Job::acceptedTaskArrived ( const Message & m )
{
    bool flag = false ;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE" , QString ("job%1").arg( _id ) ) ;
        db.setDatabaseName( QString ("jobs/input%1").arg ( _id ) ) ;
        db.open() ;
		
        QSqlQuery query ( QString("ATTACH DATABASE 'jobs/works%1' as w").arg ( _id ) , db ) ;

        query.prepare ( "INSERT INTO w.works VALUES ( ? , ? ) ;" ) ;
        query.addBindValue ( QVariant ( m.idtask() ) ) ;
        query.addBindValue ( QVariant ( m.idslave() ) ) ;
        if ( ! query.exec() )
        {
            query.prepare ( "SELECT idslave FROM w.works WHERE idtask = ? AND idslave = ? ;" );
            query.addBindValue ( QVariant ( m.idtask() ) ) ;
            query.addBindValue ( QVariant ( m.idslave() ) ) ;
            if ( query.exec() && query.next() ) flag = true ;
        }
        else
        {

            // CONSTRUIR EL BYTEARRAY A ENVIAR
            query.prepare ( "SELECT args , stdin , retrieve , files FROM tasks WHERE rowid = ? " ) ; //retrieve y files separados por comas
            query.addBindValue (  QVariant ( m.idtask() ) ) ;
            if ( query.exec() && query.next() )
            {
                //CONSTRUIR ARG
                InputStream input ( false ) ;
                input.in()	<< query.value(0).toString()
                                        << query.value(1).toString()
                                        << query.value(2).toString() ;		// ORDEN args , stdin , retrieve

                query.prepare ( QString ( "SELECT name , data FROM files WHERE id IN ( ").append( query.value(3).toString() ).append(" ) ;" ) ) ;
                if ( query.exec() )
                {
                    while ( query.next() ) input.in() << query.value(0).toString() << query.value(1).toString() ; // ADDING FILES
                }
                input.in() << QString(".") ;	// TERMINATOR
                qDebug ( "Task %d-%d accepted by slave %d\n" , m.idjob() ,  m.idtask() , m.idslave() ) ;

                dataSenderList.addSender ( m.createSend() , input.data( false ) ) ;
                flag = true ;
            }
        }
    }
    QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
    return flag ; ;
}


void Job::sendAckArrived ( const Message & m )
{
    if ( ! dataSenderList.sendAckArrived ( m ) ) freeTask ( m ) ;
}

void Job::sendArrived ( const Message & m )
{
	if ( ! dataReceiverList.sendArrived ( m ) ) 
	{
		if ( m.numpack() == 1 )
		{
                        qDebug ( "Task %d-%d starting retrieving output at slave %d.\n" , m.idjob() , m.idtask() , m.idslave() )  ;
                        {
                            QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString ("job%1").arg( _id ) ) ;
                            db.setDatabaseName ( QString ("jobs/works%1").arg ( _id ) ) ;
                            db.open() ;
                            QSqlQuery query ( db ) ;
                            query.prepare ( "SELECT idslave FROM works WHERE idtask = ? ;" );
                            query.addBindValue ( QVariant ( m.idtask() ) ) ;
                            if ( ! query.exec() ) MasterKernel::slaveCom->send ( m.createFreeTask() ) ;	// SI HAY ERROR EN LA CONSULTA
                            else if ( ! query.next() ) MasterKernel::slaveCom->send ( m.createFreeTask() ) ;	// SI NO EXISTE, BORRAR
                            else if ( query.value(0).toInt() != m.idslave() ) MasterKernel::slaveCom->send ( m.createFreeTask() ) ; // SI ESTA A CARGO DE OTRO SLAVE BORRAR
                            else dataReceiverList.addReceiver ( m ) ; // EN CASO CONTRARIO CARGAR
                        }
                        QSqlDatabase::removeDatabase ( QString ("job%1").arg ( _id ) ) ;
                }
		else MasterKernel::slaveCom->send ( m.createFreeTask() ) ;
	}
	else if ( m.numpack() == -1 )
	{
            {
                //qWarning("DEBUG %d :1\n", m.idtask() ) ;
		QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString ("job%1").arg( _id ) ) ;
                db.setDatabaseName ( QString ("jobs/works%1").arg ( _id ) ) ;
                db.open() ;
                QSqlQuery query ( db ) ;
		//qWarning("DEBUG %d :2\n", m.idtask() ) ;
                query.prepare ( "UPDATE works SET idslave = -1 WHERE idtask = ? ;" ) ;	//SETEAR KE ESE TASK TERMINO
                query.addBindValue ( QVariant ( m.idtask() ) ) ;
                query.exec() ;
		//qWarning("DEBUG %d :3\n", m.idtask() ) ;
            }
            QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
            {
                QString fileData ;
                //qWarning("DEBUG %d :4\n", m.idtask() ) ;

		QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , QString ("job%1").arg( _id ) ) ;
                db.setDatabaseName ( QString ("jobs/output%1").arg ( _id ) ) ;
                db.open() ;
		//qWarning("DEBUG %d :5\n", m.idtask() ) ;

                QDataStream stream ( dataReceiverList.data ( m.idtask() ) , QIODevice::ReadOnly ) ;
		//qWarning("DEBUG %d :6\n", m.idtask() ) ;

		stream.setVersion( QDataStream::Qt_4_1 ) ;
		QString fileName ;
		stream >> fileName ;
		//qWarning("DEBUG %d :7\n", m.idtask() ) ;

		while ( fileName != "." )
		{
			stream >> fileData ;
                        QSqlQuery query ( db ) ;
                        query.prepare ( "INSERT INTO files VALUES ( ? , ? , ? ) ;" ) ;
                        query.addBindValue ( QVariant ( m.idtask() ) ) ;
                        query.addBindValue ( QVariant ( fileName ) ) ;
                        query.addBindValue ( QVariant ( fileData ) ) ;
                        query.exec() ;
			stream >> fileName ;
		}	
		//qWarning("DEBUG %d :8\n", m.idtask() ) ;

		_completedTasks ++ ;
		//qWarning("DEBUG %d :9\n", m.idtask() ) ;

		dataReceiverList.clearTask ( m.idtask() ) ;
            }
		//qWarning("DEBUG %d :10\n", m.idtask() ) ;

            QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
            qDebug ( "Task %d-%d completed by slave %d\n" , m.idjob() , m.idtask() , m.idslave() ) ;
		//qWarning("DEBUG %d :11\n", m.idtask() ) ;

	}
}

void Job::clearTask ( int idtask )
{
	if ( isCompleted() ) return ;
        bool wasEnded = false ;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE" , QString ("job%1").arg( _id ) ) ;
            db.setDatabaseName( QString ("jobs/input%1").arg ( _id ) ) ;
            db.open() ;
            QSqlQuery query ( QString("ATTACH DATABASE 'jobs/works%1' as w").arg( _id ) , db ) ;
	
            query.prepare ( "DELETE FROM w.works WHERE idtask = ? ;" ) ;
            query.addBindValue ( QVariant ( idtask ) ) ;
            query.exec() ;

            query.prepare ( "DELETE FROM tasks WHERE rowid = ? ; " ) ;
            query.addBindValue ( QVariant ( idtask ) ) ;
            query.exec() ;
        }
        QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE" , QString ("job%1").arg( _id ) ) ;
            db.setDatabaseName( QString ("jobs/output%1").arg ( _id ) ) ;
            db.open() ;
            QSqlQuery query ( db ) ;
            query.prepare ( "SELECT idtask FROM files WHERE idtask = ? LIMIT 1 ;" ) ;
            query.addBindValue ( QVariant ( idtask ) ) ;

            if ( query.exec() && query.next() ) wasEnded = true ;

            query.prepare ( "DELETE FROM files WHERE idtask = ? ;" ) ;
            query.addBindValue ( QVariant ( idtask ) ) ;
            query.exec() ;
        }
        QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
	if ( wasEnded ) _completedTasks -- ;
	_totalTasks -- ;
	
}

void Job::infoTask ( int task , InputStream & input )
{
	if ( isCompleted() ) return ;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE" , QString ("job%1").arg( _id ) ) ;
            db.setDatabaseName( QString ("jobs/input%1").arg ( _id ) ) ;
            db.open() ;
            QSqlQuery query ( db ) ;
            query.prepare ( "SELECT rowid , program , args , retrieve  FROM tasks WHERE rowid = ? ; " ) ;
            query.addBindValue ( QVariant ( task ) ) ;
            if ( query.exec() && query.next() )
            {
                    input.in() << (qint32) query.value(0).toInt() <<  query.value(1).toString() << query.value(2).toString() << query.value(3).toInt() ;
            }
        }
        QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
}

void Job::taskList ( InputStream & input )
{
	if ( isCompleted() ) return ;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE" , QString ("job%1").arg( _id ) ) ;
            db.setDatabaseName( QString ("jobs/input%1").arg ( _id ) ) ;
            db.open() ;
            QSqlQuery query ( db ) ;
            query.prepare ( "SELECT rowid , program FROM tasks ; " ) ;
            query.exec() ;
            while ( query.next() )
            {
                input.in() << (qint32) query.value(0).toInt() <<  query.value(1).toString() ;
            }
        }
        QSqlDatabase::removeDatabase ( QString ("job%1").arg( _id ) ) ;
}

void Job::freeTask ( const Message & m )
{
        qDebug ( "Freeing task %d-%d in slave %d\n" , m.idjob() , m.idtask() , m.idslave() ) ;
	clearWorkingTask ( m.idtask() ) ;
	MasterKernel::slaveCom->send ( m.createFreeTask() ) ;
}

void Job::infoJob ( InputStream & input )
{
    input.in() << (qint32)_id << _user << (qint32)_completedTasks << (qint32)_totalTasks ;
}

void Job::jobList ( const QString & user , InputStream & input )
{
    if ( user == _user || user == "root" ) input.in() << (qint32)_id << _user ;
}


#include <slave.hpp>
#include <masterkernel.hpp>
#include <QSqlQuery>
#include <QVariant>
#include <message.hpp>
#include <QStringList>
#include <masterslavecommunicator.hpp>
#include <messagerepeater.hpp>
#include <inputstream.hpp>
#include <QDebug>

QList<int> Slave::freeIds ;
int Slave::count = 0 ;
int Slave::timeout = 30 ;
QString Slave::generalList = "" ;

Slave::Slave ( const Message & m )
{
	_maxTasks = 0 ;
	_host = m.ip() ;
	_port = m.port() ;
	_priority = m.priority() ;
	_currentTasks = m.numtasks() ;
	_maxTasks = 0 ;
	_lastUpdate = QDateTime::currentDateTime() ;
	if ( Slave::freeIds.isEmpty() )
	{
		_id = Slave::count ;
		Slave::count ++ ;
	}
	else _id = Slave::freeIds.takeFirst() ;
	pageListRepeater = 0L ;
}

Slave::~Slave ( void )
{
    MasterKernel::jobList.clearWorkingSlave ( _id ) ;
    if ( pageListRepeater ) delete pageListRepeater ;
    Slave::freeIds.push_back ( _id ) ;
}
		
void Slave::stateArrived ( const Message & m )
{
    _priority = m.priority() ;
    _currentTasks = m.numtasks() ;
    _lastUpdate = QDateTime::currentDateTime() ;

    int idjob ;
    int idtask ;
    QString program ;
    if ( _priority && _currentTasks < _maxTasks && MasterKernel::jobList.availableTask ( m.idslave() , &idjob , &idtask , &program ) ) //CHECKEAR TAREAS DISPONIBLES
    {
        qDebug ( "Trying to post task %d-%d program %s to slave %d...\n" , idjob, idtask , program.toAscii().data() , _id )  ;
        MasterKernel::slaveCom->send ( m.createPostTask ( idjob , idtask , program ) ) ;
    }
}

void Slave::fullStateArrived ( const Message & m )
{
    _group = m.group() ;
    _maxTasks = m.maxtasks() ;
    QString specificList = "" ;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , "slave" ) ;
        db.setDatabaseName ( "bd.db" ) ;
        db.open() ;

        QSqlQuery query ( db ) ;

        if ( _group.size() > 0 )
        {
            query.prepare ( "SELECT url FROM groups WHERE name = ? ;" ) ;
            query.addBindValue ( QVariant ( _group ) ) ;
            if ( query.exec() && query.next() ) specificList = query.value(0).toString() ;
        }
        else
        {
            query.prepare ( "SELECT name , url FROM groups ; " ) ;
            if ( query.exec() )
            {
                while ( query.next() )
                    if ( m.group().startsWith( m.ip().toString() ) )
                    {
                        _group = query.value(0).toString() ;
                        specificList = query.value(1).toString() ;
                        break ;
                    }
            }
        }
    }
    QSqlDatabase::removeDatabase ( "slave" ) ;
    qDebug ( "Slave %d update his group %s. Sending pageLists...\n" , _id , m.group().toAscii().data() ) ;
    MasterKernel::slaveCom->send ( m.createFullStateAck ( generalList , specificList ) ) ;
}

void Slave::notHaveAppArrived ( const Message & m )
{
        qDebug ( "Slave %d notified that not has program %s.\n" , _id , m.program().toAscii().data() ) ;
	apps [ m.program() ] = true ;
}

void Slave::pageListAckArrived ( const Message & m )
{
    if ( pageListRepeater && pageListRepeater->message().idpage() == m.idpage() )
    {
        qDebug ( "Slave %d succesful update pageList %d update .\n" , _id , m.idpage() ) ;
        delete pageListRepeater ;
        pageListRepeater = 0L ;
    }
}

void Slave::setPageList ( const QString & group , const QString url )
{
    if ( _maxTasks != 0 )
    {

        {
            QSqlDatabase db = QSqlDatabase::addDatabase ( "QSQLITE" , "slave" ) ;
            db.setDatabaseName ( "bd.db" ) ;
            db.open() ;
            if ( group == "general" )
            {
                    MasterKernel::pageId ++ ;
                    if ( pageListRepeater ) delete pageListRepeater ;
                    QSqlQuery query ( db ) ;
                    query.prepare ( "SELECT url FROM groups WHERE name = ? ;" ) ;
                    query.addBindValue ( QVariant ( _group ) )  ;
                    QString specificList = "" ;
                    if ( query.exec() && query.next() ) specificList = query.value(0).toString() ;
                    Message m ( Message::PAGELIST , _host , _port );
                    m.setIdslave ( _id ) ;
                    m.setIdpage ( MasterKernel::pageId ) ;
                    m.setGenerallist ( generalList ) ;
                    m.setSpecificlist ( specificList ) ;
                    pageListRepeater = new MessageRepeater ( m , MasterKernel::slaveCom ) ;
            }
            else if ( group == _group )
            {
                    MasterKernel::pageId ++ ;
                    if ( pageListRepeater ) delete pageListRepeater ;
                    Message m ( Message::PAGELIST , _host , _port );
                    m.setIdslave ( _id ) ;
                    m.setIdpage ( MasterKernel::pageId ) ;
                    m.setGenerallist ( generalList ) ;
                    m.setSpecificlist ( url ) ;
                    pageListRepeater = new MessageRepeater ( m , MasterKernel::slaveCom ) ;

            }
        }
        QSqlDatabase::removeDatabase ( "slave" ) ;
    }
}


bool Slave::isTimeouted ( void )
{
    return _lastUpdate.secsTo ( QDateTime::currentDateTime() ) >= 30 ;
}		

bool Slave::checkSame ( const QHostAddress host , int port ) const
{
    return ( _host == host ) && ( _port == port ) ;
}

int Slave::id ( void ) const { return _id ; }
QString Slave::group ( void ) const { return _group ; }

QString Slave::notHaveAppList ( void )
{
    QStringList list ;
    QMapIterator<QString, bool> i ( apps ) ;
    while (i.hasNext() )
    {
        i.next();
        list.push_back ( i.key() ) ;
    }
    QString string = list.join ( "','" ) ;
    if ( string.size() > 0 ) string.prepend("'").append("'" ) ;
    return string ;
}

void Slave::slaveList ( InputStream & input )
{
	input.in() << (qint32)_id << _host.toString() << (qint32)_port ;
}

void Slave::infoSlave ( InputStream & input )
{
	input.in() << (qint32)_id << _host.toString() << (qint32)_port << (quint8)_priority << _lastUpdate.toString( Qt::ISODate ) << (qint32)_currentTasks << (qint32)_maxTasks << _group ;
}


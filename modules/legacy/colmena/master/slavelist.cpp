#include <slavelist.hpp>
#include <masterslavecommunicator.hpp>
#include <slave.hpp>
#include <QList>
#include <QMapIterator>
#include <QListIterator>
#include <masterkernel.hpp>
#include <message.hpp>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QMutexLocker>

SlaveList::SlaveList ( void ) :QMap<int,Slave *> ()
{ ; }

void SlaveList::timeoutChecker ( void )
{
    QMutexLocker locker ( &mutex ) ;
    QList<int> toDelete ;

    QMapIterator<int, Slave *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        if ( i.value()->isTimeouted() ) toDelete.push_back ( i.key() ) ;
    }

    QListIterator<int> j ( toDelete ) ;

    int index ;

    while ( j.hasNext() )
    {
        index = j.next() ;
        delete value(index) ;
        remove ( index ) ;
        qDebug( "%s : Slave %d timeouted.\n" , QDateTime::currentDateTime().toString(Qt::ISODate).toAscii().data() , index ) ;
    }

    if ( isEmpty() ) Slave::count = 0 ;
}

void SlaveList::setPageList ( const QString & group , const QString url )
{
    QMutexLocker locker ( &mutex ) ;

    QMapIterator<int, Slave *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        i.value()->setPageList ( group , url ) ;
    }
}

void SlaveList::deleteSlave ( int idslave )
{
    QMutexLocker locker ( &mutex ) ;

    Slave * slave = value ( idslave ) ;
    if ( slave )
    {
        delete slave ;
        remove ( idslave ) ;
        qDebug ( "Slave %d deleted.\n" , idslave ) ;
    }
}

void SlaveList::deleteAll ( void )
{
    QMutexLocker locker ( &mutex ) ;

    QMapIterator<int, Slave *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        delete i.value() ;
    }
    clear() ;
    Slave::count = 0 ;
}

void SlaveList::stateArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;

    Slave * slave = value ( m.idslave() ) ;
    if ( slave && slave->checkSame ( m.ip() , m.port() ) ) slave->stateArrived ( m ) ;	//SI HAY EXISTE UN ESCLACO CON LA MISMA ID, IP y PUERTO , ENTONCES AVISE A ESE ESCLAVO KE ENTRO UN UPDATE
    else
    {
        slave = new Slave ( m ) ;
        insert ( slave->id() , slave ) ;
        qDebug ( "New slave %d arrived. Starting registering protocol.\n" , slave->id() ) ;
        MasterKernel::slaveCom->send ( m.createRegistered ( slave->id() ) ) ;
    }
}

void SlaveList::fullStateArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Slave * slave = value ( m.idslave() ) ;
    if ( slave ) slave->fullStateArrived( m ) ;
}

void SlaveList::notHaveAppArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Slave * slave = value ( m.idslave() ) ;
    if ( slave ) slave->notHaveAppArrived( m ) ;
}

QString SlaveList::notHaveAppList ( int idslave )
{
    Slave * slave = value ( idslave ) ;
    if ( slave ) return slave->notHaveAppList() ;
    return "" ;
}

void SlaveList::pageListAckArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Slave * slave = value ( m.idslave() ) ;
    if ( slave ) slave->pageListAckArrived( m ) ;
}

void SlaveList::infoSlave ( int idslave , InputStream & input )
{
    QMutexLocker locker ( &mutex ) ;
    Slave * slave = value ( idslave ) ;
    if ( slave ) slave->infoSlave( input ) ;
}

void SlaveList::slaveList ( InputStream & input )
{
    QMutexLocker locker ( &mutex ) ;
    QMapIterator<int, Slave *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        i.value()->slaveList ( input ) ;
    }
}

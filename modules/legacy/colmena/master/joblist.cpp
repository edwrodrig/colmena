#include <joblist.hpp>
#include <QMapIterator>
#include <job.hpp>
#include <masterkernel.hpp>
#include <QList>
#include <masterslavecommunicator.hpp>
#include <QMutexLocker>

JobList::JobList ( void ) : QMap<int,Job *> () { ; }

Job * JobList::postJob ( const QString & user )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = new Job ( user ) ;
    return job ;
}

void JobList::addJob ( Job * job )
{
    QMutexLocker locker ( &mutex ) ;
    insert ( job->id() , job ) ;
    job->open() ;
}

bool JobList::isJobCompleted ( const QString & user ,  int idjob )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( idjob ) ;
    if ( job && ( job->user() == user || user == "root" ) )
    {
            return job->isCompleted () ;
    }
    return false ;
}

void JobList::infoJob ( const QString & user , int idjob , InputStream & input )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( idjob ) ;
    if ( job && ( job->user() == user || user == "root" ) )
    {
    job->infoJob ( input ) ;
    }
}

void JobList::taskList ( const QString & user , int idjob , InputStream & input )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( idjob ) ;
    if ( job && ( job->user() == user || user == "root" ) )
    {
        job->taskList ( input ) ;
    }
}

void JobList::infoTask ( const QString & user , int idjob , int idtask , InputStream & input )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( idjob ) ;
    if ( job && ( job->user() == user || user == "root" ) )
    {
        job->infoTask ( idtask , input ) ;
    }
}

void JobList::jobList ( const QString & user , InputStream & input )
{
    QMutexLocker locker ( &mutex ) ;
    QMapIterator<int, Job *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        i.value()->jobList ( user , input ) ;
    }
}

void JobList::clearJob ( const QString & user , int idjob )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( idjob ) ;
    if ( job && ( job->user() == user || user == "root" ) )
    {
        delete job ;
        remove ( idjob ) ;
        qDebug ( "Job %d removed." , idjob ) ;
    }
}

void JobList::clearJob ( const QString & user )
{
    QMutexLocker locker ( &mutex ) ;
    QList<int> toDelete ;

    QMapIterator<int, Job *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        if ( i.value()->user() == user ) toDelete.push_back ( i.key() ) ;
    }

    QListIterator<int> j ( toDelete ) ;

    int index ;

    while ( j.hasNext() )
    {
        index = j.next() ;
        delete value ( index ) ;
        remove ( index ) ;
    }
}

void JobList::clearTask ( const QString & user , int idjob , int idtask )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( idjob ) ;
    if ( job && ( job->user() == user || user == "root" ) )
    {
        job->clearTask ( idtask ) ;
    }
}

void JobList::clearWorkingTasks ( void )
{
    QMutexLocker locker ( &mutex ) ;
    QMapIterator<int, Job *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        i.value()->clearWorkingTasks() ;
    }
}
void JobList::clearWorkingSlave ( int slave )
{
    QMutexLocker locker ( &mutex ) ;
    QMapIterator<int, Job *> i ( *this ) ;
    while (i.hasNext() )
    {
        i.next();
        i.value()->clearWorkingSlave ( slave ) ;
    }
}

bool JobList::availableTask ( int slave , int * idjob , int * idtask , QString * program )
{
    QMutexLocker locker ( &mutex ) ;
    QMapIterator<int, Job *> i ( *this ) ;

    while (i.hasNext() )
    {
        i.next();
        if ( i.value()->availableTask( slave , idtask , program ) )
        {
            *idjob = i.key() ;
            return true ;
        }
    }
    return false ;

}

void JobList::acceptedTaskArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( m.idjob() ) ;
    if ( ! ( job && job->acceptedTaskArrived ( m ) ) ) MasterKernel::slaveCom->send ( m.createFreeTask() ) ;
}

void JobList::sendAckArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( m.idjob() ) ;
    if ( job ) job->sendAckArrived ( m ) ;
    else MasterKernel::slaveCom->send ( m.createFreeTask() ) ;
}

void JobList::sendArrived ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( m.idjob() ) ;
    if ( job ) job->sendArrived ( m ) ;
    else MasterKernel::slaveCom->send ( m.createFreeTask() ) ;
}

void JobList::freeTask ( const Message & m )
{
    QMutexLocker locker ( &mutex ) ;
    Job * job = value ( m.idjob() ) ;
    if ( job ) job->freeTask ( m ) ;
    else MasterKernel::slaveCom->send ( m.createFreeTask() ) ;
}

void JobList::errorTaskArrived ( const Message & m )
{
    freeTask( m ) ;
}


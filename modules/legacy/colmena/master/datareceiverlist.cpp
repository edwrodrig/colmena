#include <datareceiverlist.hpp>
#include <datareceiver.hpp>
#include <QList>
#include <masterkernel.hpp>
#include <QSqlQuery>
#include <masterslavecommunicator.hpp>
#include <message.hpp>

DataReceiverList::DataReceiverList ( void ) : QMap<int,DataReceiver *> () { ; }
DataReceiverList::~DataReceiverList ( void ) { clear () ; }
bool DataReceiverList::addReceiver ( const Message & m )
{
	if ( ! contains( m.idtask() ) )
	{
		DataReceiver * receiver = new DataReceiver ( m , MasterKernel::slaveCom ) ;
		receiver->receivePart ( m ) ;
		insert ( m.idtask() , receiver ) ;
		return true ;
	}
	return false ;
}
bool DataReceiverList::sendArrived ( const Message & m )
{
	DataReceiver * receiver = value ( m.idtask() , 0L ) ;
	if ( receiver && receiver->idslave() == m.idslave() ) 
	{
		receiver->receivePart ( m ) ;
		return true ;
	}
	else return false ;
}

void DataReceiverList::clear ( void )
{
	QMapIterator<int, DataReceiver *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		i.value()->deleteLater() ;
    }
	QMap<int,DataReceiver *>::clear() ;
}

void DataReceiverList::clearSlaves ( int slave )
{
	QList<int> toDelete ;
    
	QMapIterator<int, DataReceiver *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		if ( i.value()->idslave() == slave ) toDelete.push_back ( i.key() ) ;
    }
	
	QListIterator<int> j ( toDelete ) ;
	
	int index ;
	
	while ( j.hasNext() )
	{
		index = j.next() ;
		(*this)[index]->deleteLater() ;
		remove ( index ) ;
	}

}

void DataReceiverList::clearTask ( int task )
{
	if ( contains ( task ) )
	{
		value ( task )->deleteLater() ;
		remove ( task ) ;
	}
}

QByteArray * DataReceiverList::data ( int task )
{
	if ( contains ( task ) )
	{
		return value ( task )->data() ;
	}
	return 0L ;
}


#include <datasenderlist.hpp>
#include <datasender.hpp>
#include <QList>
#include <masterkernel.hpp>
#include <masterslavecommunicator.hpp>
#include <message.hpp>

DataSenderList::DataSenderList ( void ) : QMap<int,DataSender *> () { ; }
DataSenderList::~DataSenderList ( void ) { clear() ; }
bool DataSenderList::addSender ( const Message & m , const QByteArray & data )
{
	if ( contains ( m.idtask() ) ) return false ;
	else
	{
		DataSender * sender = new DataSender ( m , data , MasterKernel::slaveCom ) ;
		insert ( m.idtask() , sender ) ;
		return true ;
	}
}
bool DataSenderList::sendAckArrived ( const Message & m )
{
	DataSender * sender = value ( m.idtask() ) ;
	if ( sender ) 
	{
		if ( m.numpack() == -1 )
		{
			sender->deleteLater() ;
			remove( m.idtask() ) ;
		}
		else sender->send( m ) ;
		return true ;
	}
	else return false ;
}

void DataSenderList::clearSlaves ( int slave )
{
	QList<int> toDelete ;
    
	QMapIterator<int, DataSender *> i ( *this ) ;
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

void DataSenderList::clearTask ( int task )
{
	if ( contains ( task ) )
	{
		value ( task )->deleteLater() ;
		remove ( task ) ;
	}
}


void DataSenderList::clear ( void )
{
	QMapIterator<int, DataSender *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		i.value()->deleteLater() ;
    }
	QMap<int,DataSender *>::clear() ;
}



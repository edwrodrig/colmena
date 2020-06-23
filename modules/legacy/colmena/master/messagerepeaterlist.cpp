#include <messagerepeaterlist.hpp>
#include <messagerepeater.hpp>
#include <masterkernel.hpp>
#include <masterslavecommunicator.hpp>
#include <QList>

MessageRepeaterList::MessageRepeaterList ( void ) : QMap<int,MessageRepeater *> () { ; }
MessageRepeaterList::~MessageRepeaterList ( void ) { clear () ; }
void MessageRepeaterList::addRepeater ( const Message & m  )
{
	insert ( m.idtask() , new MessageRepeater ( m , MasterKernel::slaveCom ) ) ;
}
bool MessageRepeaterList::clearMessage ( const Message & m )
{
	MessageRepeater * repeater = value ( m.idtask() ) ;
	if ( repeater ) 
	{
		delete repeater ;
		remove ( m.idtask() ) ;
		return true ;
	}
	else return false ;
}

void MessageRepeaterList::clearSlaves ( int slave )
{
	QList<int> toDelete ;
    
	QMapIterator<int, MessageRepeater *> i ( *this ) ;
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
		delete (*this)[index] ;
		remove ( index ) ;
	}

}

void MessageRepeaterList::clearTask ( int task )
{
	if ( contains ( task ) )
	{
		delete value ( task ) ;
		remove ( task ) ;
	}
}

void MessageRepeaterList::clear ( void )
{
	QMapIterator<int, MessageRepeater *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		delete i.value() ;
    }
	QMap<int, MessageRepeater *>::clear() ;
}

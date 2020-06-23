#include "messagerepeater.hpp"
#include "communicator.hpp"
#include <QApplication>

MessageRepeater::MessageRepeater ( const Message & m , Communicator * com ) : QTimer() , _message ( m ) , _com ( com )
{
        moveToThread( QApplication::instance()->thread() ) ;
        connect ( this , SIGNAL ( timeout() ) , this , SLOT ( sendMessage() ) ) ;
	start ( 1000 ) ;
	_com->send( _message ) ;
}

int MessageRepeater::idslave ( void ) const
{
	return _message.idslave() ;
}

int MessageRepeater::idtask ( void ) const
{
	return _message.idtask() ;
}

void MessageRepeater::sendMessage ( void )
{
	_com->send ( _message ) ;
}

Message MessageRepeater::message ( void ) const { return _message ; }

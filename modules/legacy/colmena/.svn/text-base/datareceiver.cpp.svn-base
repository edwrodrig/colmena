#include <datareceiver.hpp>
#include <communicator.hpp>
#include <QApplication>

DataReceiver::DataReceiver ( const Message & message , Communicator * com ) : QBuffer()
{
	this->message = message ;
	this->message.setNumpack( 0 ) ;
	this->com = com ;
	moveToThread ( QApplication::instance()->thread() ) ;
	setBuffer ( &_data ) ;
	open ( QIODevice::WriteOnly ) ;
}

DataReceiver::~DataReceiver ( void ) { ; }

QByteArray * DataReceiver::data ( void ) { return &_data ; }
	
void DataReceiver::receivePart ( const Message & m )
{
	if ( message.numpack() + 1 == m.numpack() )
	{
		message = m ;
		write ( m.data() ) ;
	}
	else if ( m.numpack() == -1 && message.numpack() != -1 )
	{
		message = m ;
		close() ;
		_data = qUncompress ( _data ) ;
	}
	com->send ( m.createAck() ) ;
}

int DataReceiver::idslave ( void ) const
{
	return message.idslave() ;
}

int DataReceiver::idtask ( void ) const
{
	return message.idtask ();
}



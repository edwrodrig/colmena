#include <datasender.hpp>
#include <communicator.hpp>
#include <QTextStream>
#include <QApplication>
DataSender::DataSender ( const Message & message , const QByteArray & data , Communicator * com ) : QBuffer() , timer ( this )
{
	this->message = message ;
	this->message.setNumpack ( 0 ) ;
	this->data = qCompress ( data ) ;
	this->com = com ;
	setBuffer ( & this->data ) ;
	moveToThread ( QApplication::instance()->thread() ) ;
	connect ( & timer , SIGNAL ( timeout() ) , this , SLOT ( sendPart() ) ) ;
	send( this->message ) ;
}

DataSender::~DataSender ( void ) { ; }

void DataSender::send ( const Message & m )
{
	if ( isOpen() || open ( QIODevice::ReadOnly ) )
	{
		if ( ! atEnd() )
		{	
			if ( m.numpack() == message.numpack() )
			{
				QByteArray array = read ( 400 ) ;
				message.setData ( array ) ;	
				message.setNumpack ( message.numpack() + 1 ) ;	
				timer.start( 10000 ) ;
				com->send ( message ) ;
			}
		}
		else
		{
			message.setNumpack ( -1 ) ;
			timer.start( 10000 ) ;
			com->send ( message ) ;	
		}
	
	}
}

int DataSender::idslave ( void ) const
{
	return message.idslave() ;
}

int DataSender::idtask ( void ) const
{
	return message.idtask () ;
}

void DataSender::sendPart ( void ) { if ( isOpen() ) com->send ( message ) ; }	

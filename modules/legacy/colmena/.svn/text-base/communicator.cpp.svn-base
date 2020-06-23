#include <QtNetwork>
#include <communicator.hpp>
#include <message.hpp>
#include <QTextStream>

Communicator::Communicator( int port ) : QObject() , _serverPort ( port )
{
	_socket.bind ( QHostAddress::Any , _serverPort , QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
	connect( & _socket , SIGNAL ( readyRead() ) , this , SLOT ( read() ) ) ;
	connect ( & _socket , SIGNAL ( error ( QAbstractSocket::SocketError ) ) , this , SLOT ( error ( QAbstractSocket::SocketError ) ) ) ;
	connect ( & _socket , SIGNAL ( stateChanged ( QAbstractSocket::SocketState ) ) , this , SLOT ( stateChanged ( QAbstractSocket::SocketState ) ) ) ;
}

Communicator::~Communicator ( void ) { ; }

int Communicator::serverPort ( void ) const { return _serverPort ; }

void Communicator::setServerPort ( int port )
{
	_serverPort = port ;
	_socket.bind ( QHostAddress::Any , _serverPort , QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint ) ;	
}

bool Communicator::send( const QByteArray & datagram , const QHostAddress & ip , int port )
{
	//QTextStream out ( stdout ) ;
	if ( _socket.writeDatagram( datagram, ip , port ) == -1 )
	{
		//out << "W state :" << _socket.state() << " error : " << _socket.error() << endl ; 
		return false ;
	}
	else
	{
		//out << "W state :" << _socket.state() << " error : " << _socket.error() << endl ; 
		return true ;
	}
}

bool Communicator::send ( const Message & message )
{
	return send ( message.toDatagram() , message.ip() , message.port() ) ;
}

void Communicator::read()
{
	//QTextStream out ( stdout ) ;
	while ( _socket.hasPendingDatagrams() )
	{
		QHostAddress senderIp ; 
		quint16 senderPort ;
		QByteArray datagram ;
		datagram.resize ( _socket.pendingDatagramSize() ) ;
		
		if ( _socket.readDatagram ( datagram.data() , datagram.size() , & senderIp , & senderPort) == -1 ) continue ;

		//out << "R state :" << _socket.state() << " error : " << _socket.error() << endl ; 
		processDatagram( datagram , senderIp , senderPort ) ;
	}
}

void Communicator::processDatagram ( const QByteArray & datagram , const QHostAddress & senderIp , int senderPort )
{
	(void) datagram ;
	(void) senderIp ;
	(void) senderPort ;
}

void Communicator::error ( QAbstractSocket::SocketError socketError )
{
	QTextStream out ( stdout ) ;
	out << "SocketError:" << socketError << endl ;
}

void Communicator::stateChanged ( QAbstractSocket::SocketState socketState )
{
	QTextStream out ( stdout ) ;
	out << "StateChanged:" << socketState << endl ;
}


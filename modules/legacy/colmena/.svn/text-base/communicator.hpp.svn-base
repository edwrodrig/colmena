#ifndef ___COMMUNICATOR___
#define ___COMMUNICATOR___

#include <QObject>
#include <QUdpSocket>

class Message ;

class Communicator : public QObject
{
	Q_OBJECT

	public:
		Communicator ( int port = 40000 ) ;
		virtual ~Communicator ( void ) ;
		int serverPort ( void ) const ;
		void setServerPort ( int port ) ;
		
	public slots :
		bool send ( const QByteArray & datagram , const QHostAddress & ip , int port ) ;
		bool send ( const Message & message ) ;
		
	private slots :
		void read ( void ) ;
		void error ( QAbstractSocket::SocketError socketError ) ;
		void stateChanged ( QAbstractSocket::SocketState socketState ) ;
		
	protected :
		virtual void processDatagram ( const QByteArray & datagram , const QHostAddress & senderIp , int senderPort ) ;
		
	protected :
		QUdpSocket _socket ;	
		int _serverPort;
 };

#endif

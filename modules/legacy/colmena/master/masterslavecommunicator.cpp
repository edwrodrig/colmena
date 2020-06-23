#include <QDateTime>
#include <masterslavecommunicator.hpp>
#include <message.hpp>
#include <masterkernel.hpp>
#include <masterslavethread.hpp>

MasterSlaveCommunicator::MasterSlaveCommunicator ( int port ) : Communicator ( port ) { ; }
MasterSlaveCommunicator::~MasterSlaveCommunicator ( void ) { ; }
void MasterSlaveCommunicator::processDatagram ( const QByteArray & datagram , const QHostAddress & senderIp , int senderPort )
{
	Message m ( datagram , senderIp , senderPort ) ;
        //if ( m.type() != Message::INVALID ) qDebug ("R %s : %s" , QDateTime::currentDateTime().toString( Qt::ISODate ).toAscii().data() , m.toString().toAscii().data() ) ;
	MasterSlaveThread * thread = new MasterSlaveThread ( m , static_cast<QObject*>(this) ) ;
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();

/*
	switch ( m.type() )
	{
		

		case Message::STATE :
			emit stateArrived ( m ) ;
			break ;
		case Message::FULLSTATE :
			emit fullStateArrived ( m ) ;
			break ;
		case Message::ACCEPTEDTASK :
			emit acceptedTaskArrived ( m ) ;
			break ;
		case Message::SEND :
			emit sendArrived ( m ) ;
			break ;
		case Message::SENDACK :
			emit sendAckArrived ( m ) ;
			break ;
		case Message::ERRORTASK :
			emit errorTaskArrived ( m ) ;
			break ;
		case Message::FREETASKACK :
			emit freeTaskAckArrived ( m ) ;
			break ;
		case Message::PAGELISTACK :
			emit pageListAckArrived ( m ) ;
			break ;
		case Message::NOTHAVEAPP :
			emit notHaveAppArrived ( m ) ;
			break ;
	}
*/
}

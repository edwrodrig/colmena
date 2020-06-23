#include <slavemastercommunicator.hpp>
#include <message.hpp>
#include <slavekernel.hpp>
#include <QDateTime>

SlaveMasterCommunicator::SlaveMasterCommunicator ( int port ) : Communicator ( port ) { ; }
SlaveMasterCommunicator::~SlaveMasterCommunicator ( void ) { ; }

void SlaveMasterCommunicator::processDatagram ( const QByteArray & datagram , const QHostAddress & senderIp , int senderPort )
{
	QTextStream out ( stdout ) ;
	Message m ( datagram , senderIp , senderPort ) ;
	SlaveKernel::out << "R " << QDateTime::currentDateTime().toString ( Qt::ISODate ) << " : " <<  m.toString() << endl ;
	if ( m.type() != Message::REGISTERED && m.idslave() != SlaveKernel::idslave ) return ;
	switch ( m.type() )
	{
		case Message::REGISTERED :
			emit registeredArrived ( m ) ;
			break ;
		case Message::FULLSTATEACK :
			emit fullStateAckArrived ( m ) ;
			break ;
		case Message::POSTTASK :
			emit postTaskArrived ( m ) ;
			break ;
		case Message::FREETASK :
			emit freeTaskArrived ( m ) ;
			break ;
		case Message::SEND :
			emit sendArrived ( m ) ;
			break ;
		case Message::SENDACK :
			emit sendAckArrived ( m ) ;
			break ;
		case Message::PAGELIST:
			emit pageListArrived ( m ) ;
			break ;
	}
}


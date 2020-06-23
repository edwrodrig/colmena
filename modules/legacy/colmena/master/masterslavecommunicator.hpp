#ifndef ___MASTERSLAVECOMMUNICATOR___
#define ___MASTERSLAVECOMMUNICATOR___

#include <communicator.hpp>
#include <message.hpp>

class MasterSlaveCommunicator : public Communicator
{
	Q_OBJECT
	public :
		MasterSlaveCommunicator ( int port = 40000 ) ;
		~MasterSlaveCommunicator ( void ) ;
	protected :
		virtual void processDatagram ( const QByteArray & datagram , const QHostAddress & senderIp , int senderPort ) ;
	signals :
		void stateArrived ( const Message & m ) ;
		void fullStateArrived ( const Message & m ) ;
		void freeTaskAckArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ;
		void acceptedTaskArrived ( const Message & m ) ;
		void sendArrived ( const Message & m ) ;
		void errorTaskArrived ( const Message & m ) ;
		void pageListAckArrived ( const Message & m ) ;
		void notHaveAppArrived ( const Message & m ) ;
} ;

#endif


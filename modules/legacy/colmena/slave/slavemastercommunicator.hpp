#ifndef ___SLAVEMASTERCOMMUNICATOR___
#define ___SLAVEMASTERCOMMUNICATOR___

#include <communicator.hpp>
#include <message.hpp>

class SlaveMasterCommunicator : public Communicator
{
	Q_OBJECT
	public :
		SlaveMasterCommunicator ( int port = 40000 ) ;
		~SlaveMasterCommunicator ( void ) ;
	
	signals :
		void registeredArrived ( const Message & m ) ;
		void fullStateAckArrived ( const Message & m ) ;
		void postTaskArrived ( const Message & m ) ;
		void freeTaskArrived ( const Message & m ) ;
		void sendArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ;
		void pageListArrived ( const Message & m ) ;
		
	protected :
		virtual void processDatagram ( const QByteArray & datagram , const QHostAddress & senderIp , int senderPort ) ;
		
} ;

#endif


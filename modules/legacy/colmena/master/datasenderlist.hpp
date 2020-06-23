#ifndef ___DATASENDERLIST___
#define ___DATASENDERLIST___

#include <QMap>
class DataSender ;
class Message ;

class DataSenderList : QMap<int,DataSender *>
{
	public :
		DataSenderList ( void ) ;
		~DataSenderList ( void ) ;
		bool addSender ( const Message & message , const QByteArray & data ) ;
		bool sendAckArrived ( const Message & m ) ;	// false , se ha llegado al final
		void clearSlaves ( int slave ) ;
		void clearTask ( int task ) ;
		void clear ( void ) ;
	
	private :
		

} ;

#endif




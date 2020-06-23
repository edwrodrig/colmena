#ifndef ___DATARECEIVERLIST___
#define ___DATARECEIVERLIST___

#include <QMap>

class DataReceiver ;
class QByteArray ;
class Message ;

class DataReceiverList : QMap<int,DataReceiver *>
{
	public :
		DataReceiverList ( void ) ;
		~DataReceiverList ( void ) ;
		bool addReceiver ( const Message & message ) ;
		bool sendArrived ( const Message & m ) ;	// false , se ha llegado al final
		QByteArray * data ( int task ) ;
		void clearSlaves ( int slave ) ;
		void clearTask ( int task ) ;
		void clear ( void ) ;
	
} ;

#endif



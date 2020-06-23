#ifndef ___DATARECEIVER___
#define ___DATARECEIVER___

#include <QBuffer>
#include <message.hpp>

class Communicator ;

class DataReceiver : public QBuffer
{
	public :
		DataReceiver ( const Message & message , Communicator * com ) ;
		~DataReceiver ( void ) ;
		
		QByteArray * data ( void ) ;	
		void receivePart ( const Message & m ) ;
		int idslave ( void ) const ;
		int idtask ( void ) const ;

	private :

		Message message ;
		QByteArray _data ;
		
		Communicator * com ;
} ;

#endif


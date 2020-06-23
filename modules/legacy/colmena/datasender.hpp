#ifndef ___DATASENDER___
#define ___DATASENDER___

#include <QBuffer>
#include <QTimer>
#include <message.hpp>

class Communicator ;
/**
El datasender se le entrega un bytearray a transmitir, el cual es comprimido.
Inmediatamente despues de ser creado, empieza a enviar mensajes
*/
class DataSender : public QBuffer
{
	Q_OBJECT	
	public :
		DataSender ( const Message & message , const QByteArray & data , Communicator * com ) ;
		~DataSender ( void ) ;

		int idslave ( void ) const ;
		int idtask ( void ) const ;

	public slots :
		void send ( const Message & m ) ;	
		
	private slots :		
		void sendPart ( void ) ;	

	private :

		Message message ;
		QByteArray data ;
		QTimer timer ;
		
		Communicator * com ;
} ;

#endif


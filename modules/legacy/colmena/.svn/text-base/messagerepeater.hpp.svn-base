#ifndef ___MESSAGEREPEATER___
#define ___MESSAGEREPEATER___

#include <message.hpp>
#include <QTimer>

class Communicator ;

class MessageRepeater : public QTimer
{
	Q_OBJECT
	public :
		MessageRepeater ( const Message & m , Communicator * com ) ;
		int idslave ( void ) const ;
		int idtask ( void ) const ;
		Message message ( void ) const ;
		
	private slots :
		void sendMessage ( void ) ;
		
	private :
		Message _message ;
		Communicator * _com ;
} ;

#endif


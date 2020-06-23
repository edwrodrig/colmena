#ifndef ___MESSAGEREPEATERLIST___
#define ___MESSAGEREPEATERLIST___

#include <QMap>

class QByteArray ;
class Message ;
class MessageRepeater ;

class MessageRepeaterList : QMap<int,MessageRepeater *>
{
	public :
		MessageRepeaterList ( void ) ;
		~MessageRepeaterList ( void ) ;
		void addRepeater ( const Message & message ) ;
		bool clearMessage ( const Message & m ) ;
		void clearSlaves ( int slave ) ;
		void clearTask  ( int task ) ;
		void clear ( void ) ;
	
} ;

#endif

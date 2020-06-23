#ifndef ___INPUTSTREAM___
#define ___INPUTSTREAM___

#include <QByteArray>
#include <QDataStream>

class InputStream
{
	public :
		InputStream ( bool count = true ) ;
		QDataStream & in ( void ) ;
		const QByteArray & data ( bool count = true ) ;
	
	private :
		QByteArray array ;
		QDataStream stream ;

} ;

#endif


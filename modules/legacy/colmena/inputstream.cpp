#include <inputstream.hpp>

InputStream::InputStream ( bool count ) : array () , stream ( &array , QIODevice::WriteOnly )
{
	stream.setVersion( QDataStream::Qt_4_1 ) ;
	if ( count ) stream << (quint32 ) 0 ;
}

QDataStream & InputStream::in ( void ) { return stream ; }

const QByteArray & InputStream::data ( bool count )
{
	if ( count )
	{
		stream.device()->seek(0) ;
		stream << (quint32) ( array.size() - sizeof(quint32) ) ;
	}
	return array ;
}


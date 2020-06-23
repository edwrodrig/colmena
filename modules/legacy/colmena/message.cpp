#include <QDataStream>
#include <QTextStream>
#include <message.hpp>

#define D_idslave  _value0
#define D_numtasks _value1
#define D_priority _bool0
#define D_maxtasks _value1
#define D_group	   _string0
#define D_generallist _string0
#define D_specificlist _string1
#define D_idjob _value1
#define D_idtask _value2
#define D_program _string0
#define D_numpack _value3
#define D_data _data0
#define D_idpage _value1



Message::Message ( void ) { ; }

Message::Message ( const QByteArray & data , const QHostAddress & ip , int port ) : _ip ( ip ) , _port ( port )
{
	QDataStream stream ( data ) ;
	stream >> _type  ;
	
	
	switch ( _type )
	{
		case STATE :
			stream >> D_idslave >> D_numtasks >> D_priority ; 
			if ( data.size() != (sizeof(quint8)+sizeof(qint32))*2 ) _type = INVALID ;
			break ;
		case REGISTERED :
			stream >> D_idslave ; break ;
		case PAGELISTACK :
			stream >> D_idslave >> D_idpage ; break ;
		case NOTHAVEAPP :
			stream >> D_idslave >> D_program ; break ;
		case FULLSTATE :
			stream >> D_idslave >> D_maxtasks >> D_group ; break ;
		case FULLSTATEACK :
		case PAGELIST:
			stream >> D_idslave >> D_idpage >> D_generallist >> D_specificlist ; break ;
		case POSTTASK :
			stream >> D_idslave >> D_idjob >> D_idtask >> D_program ; break ;
		case SEND :
			if ( numpack() == -1 ) stream >> D_idslave >> D_idjob >> D_idtask >> D_numpack ;
			else stream >> D_idslave >> D_idjob >> D_idtask >> D_numpack >> D_data ;
			break ;
		case SENDACK :
			stream >> D_idslave >> D_idjob >> D_idtask >> D_numpack ; break ;
		case ACCEPTEDTASK :
		case ERRORTASK :
		case FREETASK :
			stream >> D_idslave >> D_idjob >> D_idtask ; break ;
	}
} 
Message::Message ( quint8 type , const QHostAddress & ip , int port ) : _ip ( ip ) , _port ( port ) , _type ( type ) { ; }

Message::Message ( const Message & m ) :
_ip ( m._ip ) ,
_port ( m._port ) ,
_type ( m._type ) ,
_value0 ( m._value0 ) ,
_value1 ( m._value1 ) ,
_value2 ( m._value2 ) ,
_value3 ( m._value3 ) ,
_bool0 ( m._bool0 ) ,
_data0 ( m._data0 ) ,
_string0 ( m._string0 ) ,
_string1 ( m._string1 )
{ ; }

Message::~Message ( void ) { ; }

QHostAddress Message::ip ( void ) const { return _ip ; }
int Message::port ( void ) const { return _port ; }

void Message::setIp ( const QHostAddress & ip ) { _ip = ip ; }
void Message::setPort ( int port ) { _port = port ; }

quint8 Message::type ( void ) const { return _type ; }
qint32 Message::idslave ( void ) const { return D_idslave ; }
qint32 Message::numtasks ( void ) const { return D_numtasks ; }
qint32 Message::idjob ( void ) const { return D_idjob ; }
qint32 Message::idpage ( void ) const { return D_idpage ; }
qint32 Message::idtask ( void ) const { return D_idtask ; }
qint32 Message::numpack ( void ) const { return D_numpack ; }
qint32 Message::maxtasks ( void ) const { return D_maxtasks ; }
QString Message::group ( void ) const { return D_group ; }
QString Message::generallist ( void ) const { return D_generallist ; }
QString Message::specificlist ( void ) const { return D_specificlist ; }
QString Message::program ( void ) const { return D_program ; }
quint8 Message::priority ( void ) const { return D_priority ; }
QByteArray Message::data ( void ) const { return D_data ; }

void Message::setIdslave ( qint32 a ) { D_idslave = a ; }
void Message::setNumtasks ( qint32 a ) { D_numtasks = a ; }
void Message::setIdjob ( qint32 a ) { D_idjob = a ; }
void Message::setIdtask ( qint32 a ) {  D_idtask = a ; }
void Message::setIdpage  ( qint32 a ) { D_idpage = a ; }
void Message::setNumpack ( qint32 a ) { D_numpack = a ; }
void Message::setMaxtasks ( qint32 a ) { D_maxtasks = a ; }
void Message::setGroup ( const QString & a ) { D_group = a ; }
void Message::setGenerallist ( const QString & a ) { D_generallist = a ; }
void Message::setSpecificlist ( const QString & a ) { D_specificlist = a ; }
void Message::setProgram ( const QString & a ) { D_program = a ; }
void Message::setPriority ( quint8 a ) { D_priority = a ; }
void Message::setData ( const QByteArray & a ) { D_data = a ; }

Message Message::createNotHaveApp ( void ) const
{
	Message m = *this ;
	m._type = NOTHAVEAPP ;
	return m ;
}

Message Message::createRegistered ( int id ) const
{
	Message m = *this ;
	m._type = REGISTERED ;
	m.setIdslave ( id ) ;
	return m ;
}

Message Message::createFullState ( const QString & group , int maxtasks ) const
{
	Message m = *this ;
	m._type = FULLSTATE ;
	m.setGroup ( group ) ;
	m.setMaxtasks ( maxtasks ) ;
	return m ;
}

Message Message::createFullStateAck ( const QString & generalList , const QString & specificList ) const
{
	Message m = *this ;
	m._type = FULLSTATEACK ;
	m.setGenerallist ( generalList ) ;
	m.setSpecificlist ( specificList ) ;
	return m ;
}

Message Message::createPostTask ( int idjob , int idtask , const QString & program ) const
{
	Message m = * this ;
	m._type = POSTTASK ;
	m.setIdjob ( idjob ) ;
	m.setIdtask ( idtask ) ;
	m.setProgram ( program ) ;
	return m ;
}

Message Message::createAcceptedTask ( void ) const
{
	Message m = * this ;
	m._type = ACCEPTEDTASK ;
	return m ;
}

Message Message::createSend ( void ) const
{
	Message m = * this ;
	m._type = SEND ;
	m.setNumpack ( 0 ) ;
	return m ;
}

Message Message::createFreeTask ( void ) const
{
	Message m = *this ;
	m._type = FREETASK ;
	return m ;
}

Message Message::createErrorTask ( void ) const
{
	Message m = *this ;
	m._type = ERRORTASK ;
	return m ;
}

Message Message::createAck ( void ) const
{
	Message m = *this ;
	switch ( _type )
	{
		case SEND :
			m._type = SENDACK ;
			break ;
		case PAGELIST :
			m._type = PAGELISTACK ;
			break ;
		}
	return m ;
}

QByteArray Message::toDatagram ( void ) const
{
	QByteArray data ;
	QDataStream stream ( &data , QIODevice::WriteOnly ) ;
	stream << _type ;
	switch ( _type )
	{
		case STATE :
			stream << D_idslave << D_numtasks << D_priority ; break ;
		case REGISTERED :
			stream << D_idslave ; break ;
		case PAGELISTACK :
			stream << D_idslave << D_idpage ; break ;
		case NOTHAVEAPP :
			stream << D_idslave << D_program ; break ;
		case FULLSTATE :
			stream << D_idslave << D_maxtasks << D_group ; break ;
		case FULLSTATEACK :
		case PAGELIST:
			stream << D_idslave << D_idpage << D_generallist << D_specificlist ; break ;
		case POSTTASK :
			stream << D_idslave << D_idjob << D_idtask << D_program ; break ;
		case SEND :
			if ( numpack() == -1 ) stream << D_idslave << D_idjob << D_idtask << D_numpack ;
			else stream << D_idslave << D_idjob << D_idtask << D_numpack << D_data ;
			break ;
		case SENDACK :
			stream << D_idslave << D_idjob << D_idtask << D_numpack ; break ;
		case ACCEPTEDTASK :
		case ERRORTASK :
		case FREETASK :
			stream << D_idslave << D_idjob << D_idtask ; break ;
	}
	return data ;
}
QString Message::toString ( void ) const
{
	QString output ;
	QTextStream stream ( &output ) ;
	stream << "( " << _ip.toString() << ":" << _port << " , " ;
	switch ( _type )
	{
		case STATE :
			stream << "STATE slave:" << D_idslave << " tasks:" << D_numtasks << " prior:" << D_priority ; break ;
		case REGISTERED :
			stream << "REGISTERED slave:" << D_idslave ; break ;
		case PAGELISTACK :
			stream << "PAGELISTACK slave:" << D_idslave << " id: " << D_idpage ; break ;
		case NOTHAVEAPP :
			stream << "NOTHAVEAPP slave:" << D_idslave << " prog:" << D_program ; break ;
		case FULLSTATE :
			stream << "FULLSTATE slave:" << D_idslave << " maxt:" << D_maxtasks << " group:" << D_group ; break ;
		case FULLSTATEACK :
			stream << "FULLSTATEACK slave:" << D_idslave << " genlist:" << D_generallist << " speclist:" << D_specificlist ; break ;
		case PAGELIST:
			stream << "PAGELIST slave:" << D_idslave << " id: " << D_idpage << " genlist:" << D_generallist << " speclist:" << D_specificlist ; break ;
		case POSTTASK :
			stream << "POSTTASK slave:" << D_idslave << " task:" << D_idjob << "-" << D_idtask << " prog:" << D_program ; break ;
		case SEND :
			stream << "SEND slave:" << D_idslave << " task:" << D_idjob << "-" << D_idtask << " n:" << D_numpack ; break ;
		case SENDACK :
			stream << "SENDACK slave:" << D_idslave << " task:" << D_idjob << "-" << D_idtask << " n:" << D_numpack ; break ;
		case ACCEPTEDTASK :
			stream << "ACCEPTEDTASK slave:" << D_idslave << " task:" << D_idjob << "-" << D_idtask ; break ;
		case ERRORTASK :
			stream << "ERRORTASK slave:" << D_idslave << " task:" << D_idjob << "-" << D_idtask ; break ;
		case FREETASK :
			stream << "FREETASK slave:" << D_idslave << " task:" << D_idjob << "-" << D_idtask ; break ;
	}
	stream << " )" ;
	return output ;
	
}

void Message::operator= ( const Message & m )
{
	_ip = m._ip ;
	_port = m._port ;
	_type = m._type ;
	_value0 = m._value0 ;
	_value1 = m._value1 ;
	_value2 = m._value2 ;
	_value3 = m._value3 ;
	_bool0 = m ._bool0 ;
	_data0 = m._data0 ;
	_string0 = m._string0 ;
	_string1 = m._string1 ;
}
bool Message::operator< ( const Message & m ) const
{
	if ( D_idslave < m.D_idslave ) return true ;
	if ( D_idslave > m.D_idslave ) return false ;
	if ( D_idjob < m.D_idjob ) return true ;
	if ( D_idjob > m.D_idjob ) return false ;
	return D_idtask < m.D_idtask ;
} 
bool Message::operator> ( const Message & m ) const
{
	if ( D_idslave > m.D_idslave ) return true ;
	if ( D_idslave < m.D_idslave ) return false ;
	if ( D_idjob > m.D_idjob ) return true ;
	if ( D_idjob < m.D_idjob ) return false ;
	return D_idtask > m.D_idtask ;
} 
bool Message::operator== ( const Message & m ) const
{
	return ( D_idslave == m.D_idslave ) && ( D_idjob == m.D_idjob ) && ( D_idtask == m.D_idtask ) ;
}
bool Message::operator!= ( const Message & m ) const { return ! operator!= ( m ) ; }
bool Message::operator<= ( const Message & m ) const { return ! operator> ( m ) ; } 
bool Message::operator>= ( const Message & m ) const { return ! operator< ( m ) ; }


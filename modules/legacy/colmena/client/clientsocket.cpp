#include <clientsocket.hpp>
#include <QFile>
#include <QFileInfo>
#include <inputstream.hpp>
#include <clientmasterdef.hpp>
#include <stdio.h>
ClientSocket::ClientSocket ( void ) : QTcpSocket () , out ( stdout ) { ; }

void ClientSocket::openStream ( void )
{
	_stream.setDevice ( this ) ;
	_stream.setVersion ( QDataStream::Qt_4_1 ) ;
} 

bool ClientSocket::checkIncommingBytes ( void )
{
	quint32 size ;
	if ( ! waitForIncommingBytes( (int)sizeof(quint32) ) ) return false ;
	_stream >> size ;
	if ( ! waitForIncommingBytes( (int)size ) )	return false ;
	return true ;
}

bool ClientSocket::waitForIncommingBytes ( int bytes )
{
	while ( bytesAvailable() < bytes ) if ( ! waitForReadyRead( 600000 ) ) return false ;
	return true ;
}

bool ClientSocket::login ( quint8 action , const QString & user , const QString & pass )
{
	quint8 login ;
	InputStream input ;
	input.in() << action << user << pass ;
	if ( write ( input.data() ) == -1 || ! checkIncommingBytes() )
	{
		out << "Error sending the login information." << endl ;
		return false ;
	}
	_stream >> login ;
	switch ( login )
	{
		case CLIENTMASTER_LOGIN_OK :
			return true ;
		case CLIENTMASTER_LOGIN_NOTUSER :
			out << "Login Error : User not exists." << endl ;
			return false ;
		case CLIENTMASTER_LOGIN_INCORRECTPASS :
			out << "Login Error : Incorrect pass." << endl ;
			return false ;
		case CLIENTMASTER_LOGIN_NOTACTION :
			out << "Login Error : User not allowed to do this action." << endl ;
			return false ;
	}
	return false ;
}

bool ClientSocket::sendJobAdd ( const QString & file )
{
	InputStream input ;

	QFile job ( file ) ;
	if ( job.open ( QIODevice::ReadOnly ) )
	{	
		input.in() << qCompress ( job.readAll() ) ;
		job.close() ;						
	}								
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while ( waitForBytesWritten () ) ;
	return true ;
}

void ClientSocket::getJobAdd ( void )
{
	qint32 temp ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	
	_stream >> temp ;
	out << "ID:" << temp << endl ;
	
	return ;
}

bool ClientSocket::sendJobAddL ( const QString & file )
{
	InputStream input ;

	QFileInfo job ( file ) ;
	if ( job.exists () )
	{	
		input.in() << job.absoluteFilePath() ;				
	}								
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten () ) ;
	return true ;
}

void ClientSocket::getJobAddL ( void )
{
	qint32 temp ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	_stream >> temp ;
	out << "ID:" << temp << endl ;
}
bool ClientSocket::sendJobDel ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getJobDel ( void ) { ; }

bool ClientSocket::sendJobInf ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getJobInf ( void )
{
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	qint32 id ;
	QString user ;
	qint32 completed  ;
	qint32 total ;
	_stream >> id >> user >> completed >> total ;
	out << "id        : " << id << endl
	    << "user      : " << user << endl
	    << "completed : " << completed << endl
	    << "total     : " << total << endl ;
}

bool ClientSocket::sendJobGet ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getJobGet( void ) {
	QByteArray file;
	if( ! checkIncommingBytes() ) {
		out << "Error receiving information" << endl;
		return;
	}
	_stream >> file;
	if( file.size() <= 0 ) {
		out << "Job not ready yet" << endl;
		return;
	}
	QFile output( QString("output") );
	if( output.open( QIODevice::WriteOnly ) ) {
		output.write( qUncompress( file ) );
		output.close();
	}
}

bool ClientSocket::sendJobGetN ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getJobGetN ( void )
{
	qint64 size ;
	if ( ! waitForIncommingBytes( (int)sizeof(qint64) ) )
	{
		printf ( "No response\n" ) ;
		 return ;
	}
	_stream >> size ;
	if ( size <= 0 )
	{
		printf ( "Error receiving information :File size 0\n" ) ;
		return ;
	}
	QFile output ( QString("output") ) ;
	if ( output.open( QIODevice::WriteOnly ) )
	{
		while ( size > 0 )
		{
			if ( size >= 4000 )
			{
				if ( ! waitForIncommingBytes( 4000 ) )
				{
					printf ( "Error receiving information\n" ) ;
					return ;
				}
				output.write ( read ( 4000 ) ) ;
				size -= 4000 ;
			}
			else
			{
				if ( ! waitForIncommingBytes( (int)size ) )
				{
					printf ( "Error receiving information\n" ) ;
					return ;
				}
				output.write ( read ( size ) ) ;
				size = 0 ;
			}
			
		}
		output.close() ;	
	}
}


bool ClientSocket::sendJobGetL ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getJobGetL( void ) {
	QByteArray file;
	if( ! checkIncommingBytes() ) {
		out << "Error receiving information" << endl;
		return;
	}
	_stream >> file;
	if( file.size() <= 0 ) {
		out << "Job not ready yet" << endl;
		return;
	}
        QFile output( QString("output") );
        if( output.open( QIODevice::WriteOnly ) ) {
                output.write( qUncompress( file ) );
                output.close();
        }
}

bool ClientSocket::sendJobLst ( void ) { return true ; }
void ClientSocket::getJobLst ( void )
{
	qint32 id ;
	QString user ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	while ( bytesAvailable() > 0 )
	{
		_stream >> id >> user ;
		out << id << "\t" << user << endl ;
	}
}

bool ClientSocket::sendTaskDel ( const QString & s , const QString & ss )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() << ( qint32 )ss.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getTaskDel ( void ) { ; }

bool ClientSocket::sendTaskInf ( const QString & s , const QString & ss )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() << ( qint32 )ss.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getTaskInf ( void )
{
	qint32 number ;
	QString string ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	_stream >> number ;
	out << "id : " << number << endl ;
	_stream >> string ;
	out << "program : " << string << endl ;
	_stream >> string ;
	out << "args : " << string << endl ;
	_stream >> string ;
	out << "retrieve : " << string << endl ;

}

bool ClientSocket::sendTaskLst ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getTaskLst ( void )
{
	qint32 id ;
	QString program ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	while ( bytesAvailable() > 0 )
	{
		_stream >> id >> program ;
		out << id << "\t" << program << endl ;
	}
}

bool ClientSocket::sendGroupAdd ( const QString & s , const QString & ss , const QString & sss )
{
	InputStream input ;
	input.in() << s << ss << sss ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getGroupAdd ( void ) { ; }

bool ClientSocket::sendGroupSet ( const QString & s , const QString & ss )
{
	InputStream input ;
	input.in() << s << ss ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getGroupSet ( void ) { ; }

bool ClientSocket::sendGroupDel ( const QString & s )
{
	InputStream input ;
	input.in() << s ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getGroupDel ( void ) { ; }

bool ClientSocket::sendGroupInf ( const QString & s )
{
	InputStream input ;
	input.in() << s ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getGroupInf ( void )
{
	QString url ;
	QString user ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	_stream >> url >> user ;
	out << "url : " << url << endl << "user : " << user << endl ;
}
bool ClientSocket::sendGroupLst ( void ) { return true ; }
void ClientSocket::getGroupLst ( void ) 
{
	QString group ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	while ( bytesAvailable() > 0 )
	{
		_stream >> group ;
		out << group << endl ;
	}
}

bool ClientSocket::sendUserAdd ( const QString & s , const QString & ss )
{
	InputStream input ;
	input.in() << s << ss ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getUserAdd ( void ) { ; }
bool ClientSocket::sendUserSet ( const QString & s )
{
	InputStream input ;
	input.in() << s ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}
void ClientSocket::getUserSet ( void ) { ; }

bool ClientSocket::sendUserDel ( const QString & s )
{
	InputStream input ;
	input.in() << s ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}
void ClientSocket::getUserDel ( void ) { ; }
bool ClientSocket::sendUserInf ( const QString & s )
{
	InputStream input ;
	input.in() << s ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getUserInf ( void )
{
	QString pass;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	_stream >> pass ;
	out << pass << endl ;
}

bool ClientSocket::sendUserLst ( void ) { return true ; }

void ClientSocket::getUserLst ( void )
{
	QString user;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	while ( bytesAvailable() > 0 )
	{
		_stream >> user ;
		out << user << endl ;
	}
}

bool ClientSocket::sendSlaveDel ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}

void ClientSocket::getSlaveDel ( void ) { ; }
bool ClientSocket::sendSlaveInf ( const QString & s )
{
	InputStream input ;
	input.in() << ( qint32 )s.toInt() ;
	if ( write ( input.data() ) == -1 )
	{
		out << "Error sending information." << endl ;
		return false ;
	}
	while( waitForBytesWritten ( ) ) ;
	return true ;
}
void ClientSocket::getSlaveInf ( void )
{
	qint32 number ;
	quint8 number2 ;
	QString string ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	_stream >> number ;
	out << "id : " << number << endl ;
	_stream >> string ;
	out << "ip : " << string << endl ;
	_stream >> number ;
	out << "port : " << number << endl ;
	_stream >> number2 ;
	out << "priority : " << number2 << endl ;
	_stream >> string ;
	out << "lastupdate : " << string << endl ;
	_stream >> number ;
	out << "current : " << number << endl ;
	_stream >> number ;
	out << "max : " << number << endl ;
	_stream >> string ;
	out << "group : " << string << endl ;
}

bool ClientSocket::sendSlaveLst ( void ) { return true ; }
void ClientSocket::getSlaveLst ( void )
{
	qint32 id ;
	QString ip ;
	quint32 port ;
	if ( ! checkIncommingBytes() )
	{
		out << "Error receiving information" << endl ;
		return ;
	}
	while ( bytesAvailable() > 0 )
	{
		_stream >> id >> ip >> port  ;
		out << id << "\t" << ip << "\t" << port << endl ;
	}
}

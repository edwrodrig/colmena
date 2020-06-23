#ifndef ___MESSAGE___
#define ___MESSAGE___

#include <QHostAddress>
#include <QByteArray>
#include <QString>

class Message
{
	public :

		//TYPE
		static const quint8 STATE 	= 0 ;
		static const quint8 REGISTERED	= 1 ;
		static const quint8 FULLSTATE = 2 ;
		static const quint8 FULLSTATEACK = 3 ;
		
		static const quint8 POSTTASK	= 4 ;
		static const quint8 ACCEPTEDTASK = 5 ;
		static const quint8 SEND	= 6 ;
		static const quint8 SENDACK	= 7 ;

		static const quint8 ERRORTASK	= 8 ;

		static const quint8 FREETASK	= 9 ;
		static const quint8 FREETASKACK	= 10 ;

		static const quint8 PAGELIST   = 11 ;
		static const quint8 PAGELISTACK = 12 ;
		
		static const quint8 NOTHAVEAPP = 13 ;
		static const quint8 INVALID = 14 ;
		
		
		//PRIORIDAD DE PROCESO DE ESCLAVO
		static const quint8 LOW = 0 ;
		static const quint8 HIGH = 1 ;
		
		Message ( void ) ;
		Message ( const QByteArray & data , const QHostAddress & ip , int port ) ;
		Message ( quint8 type , const QHostAddress & ip , int port ) ;
		Message ( const Message & m ) ;
		~Message ( void ) ;

		QHostAddress ip ( void ) const ;
		int port ( void ) const ;
		
		void setIp ( const QHostAddress & ip ) ;
		void setPort ( int port ) ;

		quint8 type ( void ) const ;
		
		qint32 idslave ( void ) const ;
		qint32 numtasks ( void ) const ;
		qint32 idjob ( void ) const ;
		qint32 idtask ( void ) const ;
		qint32 idpage ( void ) const ;
		qint32 numpack ( void ) const ;
		qint32 maxtasks ( void ) const ;
		QString group ( void ) const ;
		QString generallist ( void ) const ;
		QString specificlist ( void ) const ;
		QString program ( void ) const ;
		quint8 priority ( void ) const ;
		QByteArray data ( void ) const ;
		
		void setIdslave ( qint32 a ) ;
		void setNumtasks ( qint32 a ) ;
		void setIdjob ( qint32 a ) ;
		void setIdtask ( qint32 a ) ;
		void setIdpage ( qint32 a ) ;
		void setNumpack ( qint32 a ) ;
		void setMaxtasks ( qint32 a ) ;
		void setGroup ( const QString & a ) ;
		void setGenerallist ( const QString & a ) ;
		void setSpecificlist ( const QString & a ) ;
		void setProgram ( const QString & a ) ;
		void setPriority ( quint8 a ) ;
		void setData ( const QByteArray & a ) ;

		Message createRegistered ( int id ) const ;
		Message createFullStateAck ( const QString & generalList , const QString & specificList ) const ;
		Message createPostTask ( int idjob , int idtask , const QString & program ) const ;
		Message createSend ( void ) const ;
		Message createNotHaveApp ( void ) const ;
		Message createFreeTask ( void ) const ;
		Message createAcceptedTask ( void ) const ;
		Message createErrorTask ( void ) const ;

		Message createAck ( void ) const ;
		Message createFullState ( const QString & group , int maxtasks ) const ;

		QByteArray toDatagram ( void ) const ;
		QString toString ( void ) const ;

		void operator= ( const Message & m ) ;
		bool operator< ( const Message & m ) const ;
		bool operator> ( const Message & m ) const ;
		bool operator== ( const Message & m ) const ;
		bool operator!= ( const Message & m ) const ;
		bool operator<= ( const Message & m ) const ;
		bool operator>= ( const Message & m ) const ;
		
	
	private :
		QHostAddress _ip ;
		int _port ;

		quint8 _type ;

		qint32 _value0 ;
		qint32 _value1 ;
		qint32 _value2 ;
		qint32 _value3 ;
		
		quint8 _bool0 ;
		
		QByteArray _data0 ;
		QString _string0 ;
		QString _string1 ;

		
		
} ;

#endif


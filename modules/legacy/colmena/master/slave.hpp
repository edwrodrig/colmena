#ifndef ___SLAVE___
#define ___SLAVE___

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QHostAddress>
#include <QList>

class Message ;
class MessageRepeater ;
class InputStream ;

class Slave
{

	public :
		
		Slave ( const Message & m ) ;
		~Slave ( void ) ;	//delete this slave from the database
			
		bool isTimeouted ( void ) ;		
		bool checkSame ( const QHostAddress host , int port ) const ;
		int id ( void ) const ;
		QString group ( void ) const ;
		void stateArrived ( const Message & m ) ;
		void fullStateArrived ( const Message & m ) ;
		void notHaveAppArrived ( const Message & m ) ;
		void pageListAckArrived ( const Message & m ) ;
		QString notHaveAppList ( void ) ;
		
		void slaveList ( InputStream & input ) ;
		void infoSlave ( InputStream & input ) ;
		void setPageList ( const QString & group , const QString url ) ;
		
		static QString generalList ;
		static int count ;
		static QList<int> freeIds ;
	private :
		static int timeout ; // in seconds

		
		int _id ;
		QHostAddress _host ;
		int _port ;
		bool _priority ; // 0 LOW , 1 HIGH
		QDateTime _lastUpdate ;
		int _currentTasks ;
		int _maxTasks ;
		QString _group ;

		QMap<QString,bool>  apps ;
		
		MessageRepeater * pageListRepeater ;
		
} ;

#endif

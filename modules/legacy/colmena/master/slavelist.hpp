#ifndef ___SLAVELIST___
#define ___SLAVELIST___

#include <QMap>
#include <QString>
#include <QMutex>

class Slave ;
class Message ;
class InputStream ;

class SlaveList : public QMap<int,Slave *>
{
	public :
		
		SlaveList ( void ) ;
		
		void timeoutChecker ( void ) ;
		void stateArrived ( const Message & m ) ;
		void fullStateArrived ( const Message & m ) ;
		void notHaveAppArrived ( const Message & m ) ;
		void pageListAckArrived ( const Message & m ) ;
		
		void deleteSlave ( int slave ) ;
		void deleteAll ( void ) ;
		void slaveList ( InputStream & input ) ;
		void infoSlave ( int slave , InputStream & input ) ;
		void setPageList ( const QString & group , const QString url ) ;
		
                QString notHaveAppList ( int slave ) ;
	
        private :
                QMutex mutex ;
} ;

#endif

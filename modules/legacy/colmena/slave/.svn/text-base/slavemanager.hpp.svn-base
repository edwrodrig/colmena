#ifndef ___SLAVEMANAGER___
#define ___SLAVEMANAGER___

#include <QObject>
#include <QLocalServer>
#include <QSqlDatabase>
#include <QTimer>
#include <QMap>
#include <QDir>

class SlaveTask ;
class SlaveMasterCommunicator ;
class MessageRepeater ;
class Message ;

class SlaveManager : public QObject
{
	Q_OBJECT
	public :
		
		SlaveManager ( void ) ;
		~SlaveManager ( void ) ;
		bool init ( void ) ;

	public slots :
		
		void ssaverStarted ( void ) ;
		void ssaverClosed ( void ) ;
		void ssaverGenerateHtml ( void ) ;
		
	private slots :

		void sendState ( void ) ;
		void registeredArrived ( const Message & m ) ;
		void fullStateAckArrived ( const Message & m ) ;
		void postTaskArrived ( const Message & m ) ;
		void freeTaskArrived ( const Message & m ) ;
		void sendArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ;
		void pageListArrived ( const Message & m ) ;

	private :

		void checkApps ( QDir dir ) ;
		void checkEnvVars ( QDir dir ) ;
		void clearJobs ( void ) ;
		void sendSSaverPages ( void ) ;
	
		void start ( void ) ;
	
		bool initConfig ( void ) ;
		bool initConnections ( void ) ;
		bool initSSaverListener ( void ) ;
		bool initDatabase ( void ) ;
		bool initApps ( void ) ;

		MessageRepeater * fullStateRepeater ;
		
		QTimer stateTimer ;
	
} ;

#endif


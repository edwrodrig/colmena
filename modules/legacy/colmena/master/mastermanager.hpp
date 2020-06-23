#ifndef ___MASTERMANAGER___
#define ___MASTERMANAGER___

#include <QObject>
#include <QTimer>
#include <QString>

/**
*	Clase Master Manager
*	Es la clase principal de la aplicaci�n maestra del cluster virtual.
*/
class MasterSlaveCommunicator ;
class MasterClientListener ;
class Message ;


class MasterManager : public QObject
{
	Q_OBJECT

	public :
		MasterManager ( void ) ;
		~MasterManager ( void ) ;
		bool init ( void ) ;

	public slots :

		void stateArrived ( const Message & m ) ;
		void fullStateArrived ( const Message & m ) ;
		void acceptedTaskArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ;
		void sendArrived ( const Message & m ) ;
		void pageListAckArrived ( const Message & m ) ;
		void errorTaskArrived ( const Message & m ) ;
		void notHaveAppArrived ( const Message & m ) ;


	private slots :

		void slaveTimeout ( void ) ;
		
	private :

		bool initConfig ( void ) ;
		bool initDatabase ( void ) ;
		bool initConnections ( void ) ;

		int slaveport ;
		int clientport ;
		int slavetimeout ;

		QTimer slaveTimeoutTimer ;
		

} ;

#endif

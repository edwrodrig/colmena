#ifndef ___MASTERCLIENTLISTENER___
#define ___MASTERCLIENTLISTENER___

#include <QTcpServer>

class MasterClientListener : public QTcpServer
{
	Q_OBJECT
	public :
		MasterClientListener ( void ) ;
	protected :
		void incomingConnection ( int socketDescriptor ) ;

} ;

#endif

#ifndef ___MASTERCLIENTCOMMUNICATOR___
#define ___MASTERCLIENTCOMMUNICATOR___

#include <QTcpSocket>
#include <QDataStream>

class MasterClientCommunicator : public QTcpSocket
{
	Q_OBJECT
	public :
		MasterClientCommunicator ( int socketDescriptor ) ;
                ~MasterClientCommunicator ( void ) ;
	public slots :
		void readClient ( void ) ;
	private :
		void rcvJobAdd ( const QString & user ) ;
		void rcvJobAddL ( const QString & user ) ;
		void rcvJobDel ( const QString & user ) ;
		void rcvJobInf ( const QString & user ) ;
		void rcvJobGet ( const QString & user ) ;
		void rcvJobGetN ( const QString & user ) ;
		void rcvJobGetL ( const QString & user ) ; 
		void rcvJobLst ( const QString & user ) ;
		void rcvTaskDel ( const QString & user ) ;
		void rcvTaskInf ( const QString & user ) ;
		void rcvTaskLst ( const QString & user ) ;
		void rcvGroupAdd ( void ) ;
		void rcvGroupSet ( const QString & user ) ;
		void rcvGroupDel ( const QString & user ) ;
		void rcvGroupInf ( const QString & user ) ;
		void rcvGroupLst ( const QString & user ) ;
		void rcvUserAdd ( void ) ;
		void rcvUserSet ( const QString & user ) ;
		void rcvUserDel ( void ) ;
		void rcvUserLst ( void ) ;
		void rcvUserInf ( void ) ;
		void rcvSlaveDel ( void ) ;
		void rcvSlaveInf ( void ) ;
		void rcvSlaveLst ( void ) ;

		bool checkIncommingBytes ( void ) ;
		bool waitForIncommingBytes ( int bytes ) ;

		bool checkPerms ( const QString & user , const QString & pass , quint8 order ) ;

		QDataStream stream ;
} ;


#endif

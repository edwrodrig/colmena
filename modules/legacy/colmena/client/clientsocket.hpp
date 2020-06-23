#ifndef ___CLIENTSOCKET___
#define ___CLIENTSOCKET___

#include <QTcpSocket>
#include <QTextStream>
#include <QDataStream>

class ClientSocket : public QTcpSocket
{
	public :
		ClientSocket ( void ) ;
		void openStream ( void ) ;
		bool login ( quint8 action , const QString & user , const QString & pass ) ;
		
		bool sendJobAdd ( const QString & s ) ;
		void getJobAdd ( void ) ;
		bool sendJobAddL ( const QString & s ) ;
		void getJobAddL ( void ) ;
		bool sendJobDel ( const QString & s ) ;
		void getJobDel ( void ) ;
		bool sendJobInf ( const QString & s ) ;
		void getJobInf ( void ) ;
		bool sendJobGet ( const QString & s ) ;
		void getJobGet ( void ) ;
		bool sendJobGetN ( const QString & s ) ;
		void getJobGetN ( void ) ;
		bool sendJobGetL ( const QString & s ) ;
		void getJobGetL ( void ) ;
		bool sendJobLst ( void ) ;
		void getJobLst ( void ) ;
		bool sendTaskDel ( const QString & s , const QString & ss ) ;
		void getTaskDel ( void ) ;
		bool sendTaskInf ( const QString & s , const QString & ss ) ;
		void getTaskInf ( void ) ;
		bool sendTaskLst ( const QString & s ) ;
		void getTaskLst ( void ) ;

		bool sendGroupAdd ( const QString & s , const QString & ss , const QString & sss ) ;
		void getGroupAdd ( void ) ;
		bool sendGroupSet ( const QString & s , const QString & ss ) ;
		void getGroupSet ( void ) ;
		bool sendGroupDel ( const QString & s ) ;
		void getGroupDel ( void ) ;
		bool sendGroupInf ( const QString & s ) ;
		void getGroupInf ( void ) ;
		bool sendGroupLst ( void ) ;
		void getGroupLst ( void ) ;
		
		
		bool sendUserAdd ( const QString & s , const QString & ss ) ;
		void getUserAdd ( void ) ;
		bool sendUserSet ( const QString & s ) ;
		void getUserSet ( void ) ;
		bool sendUserDel ( const QString & s ) ;
		void getUserDel ( void ) ;
		bool sendUserInf ( const QString & s ) ;
		void getUserInf ( void ) ;
		bool sendUserLst ( void ) ;
		void getUserLst ( void ) ;
		bool sendSlaveDel ( const QString & s ) ;
		void getSlaveDel ( void ) ;
		bool sendSlaveInf ( const QString & s ) ;
		void getSlaveInf ( void ) ;
		bool sendSlaveLst ( void ) ;
		void getSlaveLst ( void ) ;
		
		
		bool checkIncommingBytes ( void ) ;
		bool waitForIncommingBytes ( int bytes ) ;

		
	private :
		QDataStream _stream ;
		QTextStream out ;

} ;


#endif


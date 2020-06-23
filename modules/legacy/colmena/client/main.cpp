#include <QCoreApplication>
#include <QStringList>
#include <QTextStream>
#include <QDataStream>
#include <QTcpSocket>
#include <QByteArray>
#include <QHostAddress>
#include <QFile>
#include <clientmasterdef.hpp>
#include <clientsocket.hpp>
#include <QUrl>

int main ( int argc , char *argv[] )
{
	ClientSocket socket ; 
	QTextStream out ( stdout ) ;
	QCoreApplication app( argc , argv ) ;
	QStringList arg = app.arguments() ;

	if ( arg.size() >= 7 )
	{		
		QHostAddress ip ( arg.at(3) ) ;
		int port = arg.at(4).toInt() ;
		
		socket.connectToHost ( ip , port ) ;
		if ( socket.waitForConnected ( 20000 ) )
		{	
			socket.openStream() ;
			QString user = arg.at(5) ;
			QString pass = arg.at(6) ;
			QString elem = arg.at(1) ;
			QString act = arg.at(2) ;
			int size = arg.size() ;
			quint8 action = 0 ;
			
			
			if ( elem == "job" )
			{
				action = CLIENTMASTER_JOB ;
				if ( size == 8 && act == "add" )
				{
					if ( ip == QHostAddress::LocalHost )
					{
						action |= CLIENTMASTER_ADL ;
						if ( socket.login ( action , user, pass ) && socket.sendJobAddL ( arg.at(7) ) )
							socket.getJobAddL () ;
					}
					else
					{
						action |= CLIENTMASTER_ADD ;
						if ( socket.login ( action , user, pass ) && socket.sendJobAdd ( arg.at(7) ) )
							socket.getJobAdd () ;
					}
				}
				else if ( size == 8 && act == "del" )
				{
					action |= CLIENTMASTER_DEL ;
					if ( socket.login ( action , user, pass ) && socket.sendJobDel ( arg.at(7) ) )
						socket.getJobDel () ;
				}
				else if ( size == 8 && act == "inf" )
				{
					action |= CLIENTMASTER_INF ;
					if ( socket.login ( action , user, pass ) && socket.sendJobInf ( arg.at(7) ) )
						socket.getJobInf () ;
				}
				else if ( size == 8 && act == "get" )
				{
					if ( ip == QHostAddress::LocalHost ) 
					{
						action |= CLIENTMASTER_GEL ;
						if ( socket.login ( action , user, pass ) && socket.sendJobGetL ( arg.at(7) ) )
							socket.getJobGetL () ;
					}
					else
					{
						action |= CLIENTMASTER_GET ;
						if ( socket.login ( action , user, pass ) && socket.sendJobGetN ( arg.at(7) ) )
							socket.getJobGetN () ;
					}
				}
				else if ( size == 7 && act == "lst" )
				{
					action |= CLIENTMASTER_LST ;
					if ( socket.login ( action , user, pass ) && socket.sendJobLst () )
						socket.getJobLst () ;
				}
				return 0 ;				
			}
			else if ( elem == "task" )
			{
				action = CLIENTMASTER_TASK ;
				if ( size == 9 && act == "del" )
				{
					action |= CLIENTMASTER_DEL ;
					if ( socket.login ( action , user, pass ) && socket.sendTaskDel ( arg.at(7) , arg.at(8) ) )
						socket.getTaskDel () ;
				}
				else if ( size == 9 && act == "inf" )
				{
					action |= CLIENTMASTER_INF ;
					if ( socket.login ( action , user, pass ) && socket.sendTaskInf ( arg.at(7) , arg.at(8) ) )
						socket.getTaskInf () ;
				}
				else if ( size == 8 && act == "lst" )
				{
					action |= CLIENTMASTER_LST ;
					if ( socket.login ( action , user, pass ) && socket.sendTaskLst ( arg.at(7) ) )
						socket.getTaskLst () ;
				}
				return 0 ;
				
			}
			else if ( elem == "group" )
			{
				action = CLIENTMASTER_GROUP ;
				if ( size == 10 && act == "add" )
				{
					action |= CLIENTMASTER_ADD ;
					if ( socket.login ( action , user, pass ) && socket.sendGroupAdd ( arg.at(7) , arg.at(8) , arg.at(9) ) )
						socket.getGroupAdd () ;
				}
				else if ( size == 9 && act == "set" )
				{
					action |= CLIENTMASTER_SET ;
					if ( socket.login ( action , user, pass ) && socket.sendGroupSet ( arg.at(7) , arg.at(8) ) )
						socket.getGroupSet () ;
				}
				else if ( size == 8 && act == "del" )
				{
					action |= CLIENTMASTER_DEL ;
					if ( socket.login ( action , user, pass ) && socket.sendGroupDel ( arg.at(7) ) )
						socket.getGroupDel () ;
				}
				else if ( size == 8 && act == "inf" )
				{
					action |= CLIENTMASTER_INF ;
					if ( socket.login ( action , user, pass ) && socket.sendGroupInf ( arg.at(7) ) )
						socket.getGroupInf () ;
				}
				else if ( size == 7 && act == "lst" )
				{
					action |= CLIENTMASTER_LST ;
					if ( socket.login ( action , user, pass ) && socket.sendGroupLst () )
						socket.getGroupLst () ;
				}
				return 0 ;				
			}
			else if ( elem == "user" )
			{
				action = CLIENTMASTER_USER ;
				if ( size == 9 && act == "add" )
				{
					action |= CLIENTMASTER_ADD ;
					if ( socket.login ( action , user, pass ) && socket.sendUserAdd ( arg.at(7) , arg.at(8) ) )
						socket.getUserAdd () ;
				}
				else if ( size == 8 && act == "set" )
				{
					action |= CLIENTMASTER_SET ;
					if ( socket.login ( action , user, pass ) && socket.sendUserSet ( arg.at(7) ) )
						socket.getUserSet () ;
				}
				else if ( size == 8 && act == "del" )
				{
					action |= CLIENTMASTER_DEL ;
					if ( socket.login ( action , user, pass ) && socket.sendUserDel ( arg.at(7) ) )
						socket.getUserDel () ;
				}
				else if ( size == 8 && act == "inf" )
				{
					action |= CLIENTMASTER_INF ;
					if ( socket.login ( action , user, pass ) && socket.sendUserInf ( arg.at(7) ) )
						socket.getUserInf () ;
				}
				else if ( size == 7 && act == "lst" )
				{
					action |= CLIENTMASTER_LST ;
					if ( socket.login ( action , user, pass ) && socket.sendUserLst () )
						socket.getUserLst () ;
				}
				return  0 ;
				
			}
			else if ( elem == "slave" )
			{
				action = CLIENTMASTER_SLAVE ;
				if ( size == 8 && act == "del" )
				{
					action |= CLIENTMASTER_DEL ;
					if ( socket.login ( action , user, pass ) && socket.sendSlaveDel ( arg.at(7) ) )
						socket.getSlaveDel () ;
				}
				else if ( size == 8 && act == "inf" )
				{
					action |= CLIENTMASTER_INF ;
					if ( socket.login ( action , user, pass ) && socket.sendSlaveInf ( arg.at(7) ) )
						socket.getSlaveInf () ;
				}
				else if ( size == 7 && act == "lst" )
				{
					action |= CLIENTMASTER_LST ;
					if ( socket.login ( action , user, pass ) && socket.sendSlaveLst () )
						socket.getSlaveLst () ;
				}
				return 0 ;
				
			}

		}
		else
		{
			out << "Can't connect to master: " << ip.toString() << ":" << port << endl ;			
			return 0 ;		
		}	
	}
	out 	<< "Usage :" << endl
		<< "job add host port user pass file  : Post a job." << endl
		<< "job del host port user pass idjob : Delete a job." << endl   
		<< "job inf host port user pass idjob : Get job information." << endl
		<< "job get host port user pass idjob : Get job result." << endl
		<< "job lst host port user pass       : List jobs." << endl << endl
		<< "task del host port user pass idjob idtask : Delete a task." << endl
		<< "task inf host port user pass idjob idtask : Get task information." << endl
		<< "task lst host port user pass idjob        : List task of a job." << endl << endl
		<< "group add host port user pass group url user : Add group." << endl
		<< "group set host port user pass group url      : Set group." << endl
		<< "group del host port user pass group          : Delete group." << endl
		<< "group inf host port user pass group          : Information of a group." << endl
		<< "group lst host port user pass                : List groups." << endl << endl
		<< "user add host port user pass user pass : Add an user." << endl
		<< "user set host port user pass newpass   : Set user password." << endl
		<< "user del host port user pass user      : Delete an user." << endl
		<< "user lst host port user pass           : List users." << endl << endl 
		<< "user inf host port user pass user      : Users info." << endl << endl 
		<< "slave del host port user pass idslave : Reset a slave." << endl 
		<< "slave inf host port user pass idslave : Slave info." << endl 
		<< "slave lst host port user pass         : List slaves." << endl ;
	return 0 ;	
}


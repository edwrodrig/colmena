#include <slavekernel.hpp>
#include <QTcpSocket>
#include <QSqlQuery>
#include <QVariant>
#include <QFile>
#include <QTime>

int SlaveKernel::idslave = -1 ;
QHostAddress SlaveKernel::masterip ;
int SlaveKernel::masterport ;
int SlaveKernel::listenport ;
int SlaveKernel::maxtasks ;
bool SlaveKernel::alwayshigh ;
QString SlaveKernel::group ;
QString SlaveKernel::generalPageList ;
QString SlaveKernel::specificPageList ;
QSqlDatabase SlaveKernel::db ;
SlaveMasterCommunicator * SlaveKernel::masterCom = 0L ;
bool SlaveKernel::priority = false ;
TaskList SlaveKernel::taskList ;
QStringList SlaveKernel::envVars ;
QTextStream SlaveKernel::out ( stdout ) ;
int SlaveKernel::pageId = 0 ;
QTcpSocket * SlaveKernel::ssaverSocket = 0L ;
QTcpServer SlaveKernel::ssaverListener ;
quint16 SlaveKernel::ssaverport ;

void SlaveKernel::addHistoryEntry ( const QString & app , int action  , int secs )
{
	int count ;
	int time ;

	QSqlQuery query ( SlaveKernel::db ) ;
	if ( action == HISTORYACTION_COMPLETED )
	{
		query.prepare ( "SELECT completed , completedtime FROM history WHERE app = ? ; " ) ;
		query.addBindValue ( QVariant ( app ) ) ;
		query.exec() ;
		if ( query.next() )
		{
			count = query.value(0).toInt() + 1 ;
			time = query.value(1).toInt() + secs ;
			query.prepare ( "UPDATE history SET completed = ? , completedtime = ?  WHERE app = ? ;" ) ;
			query.addBindValue ( QVariant ( count ) ) ;
			query.addBindValue ( QVariant ( time ) ) ;
			query.addBindValue ( QVariant ( app ) ) ;
			query.exec() ;
		}
		else
		{
			query.prepare ( "INSERT INTO history VALUES (?,1,0,0,?,0,0) ;" ) ;
			query.addBindValue ( QVariant ( app ) ) ;
			query.addBindValue ( QVariant ( secs ) ) ;
			query.exec() ;			
		}
	}
	else if ( action == HISTORYACTION_ERROR )
	{
		query.prepare ( "SELECT error , errortime FROM history WHERE app = ? ; " ) ;
		query.addBindValue ( QVariant ( app ) ) ;
		query.exec() ;
		if ( query.next() )
		{
			count = query.value(0).toInt() + 1 ;
			time = query.value(1).toInt() + secs ;
			query.prepare ( "UPDATE history SET error = ? , errortime = ?  WHERE app = ? ;" ) ;
			query.addBindValue ( QVariant ( count ) ) ;
			query.addBindValue ( QVariant ( time ) ) ;
			query.addBindValue ( QVariant ( app ) ) ;
			query.exec() ;			
		}
		else
		{
			query.prepare ( "INSERT INTO history VALUES (?,0,1,0,0,?,0) ;" ) ;
			query.addBindValue ( QVariant ( app ) ) ;
			query.addBindValue ( QVariant ( secs ) ) ;
			query.exec() ;			
		}	
	}
	else if ( action == HISTORYACTION_OTHER )
	{
		query.prepare ( "SELECT other , othertime FROM history WHERE app = ? ; " ) ;
		query.addBindValue ( QVariant ( app ) ) ;
		query.exec() ;
		if ( query.next() )
		{
			count = query.value(0).toInt() + 1 ;
			time = query.value(1).toInt() + secs ;
			query.prepare ( "UPDATE history SET other = ? , othertime = ?  WHERE app = ? ;" ) ;
			query.addBindValue ( QVariant ( count ) ) ;
			query.addBindValue ( QVariant ( time ) ) ;
			query.addBindValue ( QVariant ( app ) ) ;
			query.exec() ;			
		}
		else
		{
			query.prepare ( "INSERT INTO history VALUES (?,0,0,1,0,0,?) ;" ) ;
			query.addBindValue ( QVariant ( app ) ) ;
			query.addBindValue ( QVariant ( secs ) ) ;
			query.exec() ;			
		}
	}
}

void SlaveKernel::generateHtml ( void )
{
	QFile file ( "stats/index.html") ;
	QTextStream stream ;
	int seconds ;
	int hours ;
	
	if ( file.open ( QIODevice::WriteOnly ) )
	{
		stream.setDevice ( &file ) ;
		stream << "<html><link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" /><body><div class=\"title\"></div><table class=\"main\"><tr><td class=\"tasks\" valign=top><div class=\"tasktitle\">Tareas Actuales</div>" ;
		SlaveKernel::taskList.generateHtml ( stream ) ;
		stream << "</td><td class=\"history\" valign=top><div class=\"historytitle\">Historial</div><table class=\"apps\"><tr><th class=\"program\">Programa</th><th class=\"completed\">Completados</th><th class=\"error\">Error</th><th class=\"other\">Otros</th><th class=\"completedtime\">Tiempo</th></tr>" ;
		
		QSqlQuery query ( SlaveKernel::db ) ;
		query.prepare ( " SELECT app , completed , error , other , completedtime FROM history GROUP BY app ORDER BY completedtime , completed , app DESC LIMIT 10 ;" ) ;
		if ( query.exec() )
		{
			while ( query.next() )
			{
				stream << "<tr><td>" << query.value(0).toString()
				       << "</td><td class=\"number\">" << query.value(1).toInt()
					   << "</td><td class=\"number\">" << query.value(2).toInt()
					   << "</td><td class=\"number\">" << query.value(3).toInt() ;
						seconds = query.value(4).toInt() ;
						hours = seconds / 3600 ;
						QTime time ( 0 , 0 , 0 );
				stream << "</td><td class=\"number\">" << hours << ":" << time.addSecs ( seconds % 3600 ).toString ( "mm:ss" )	
					   << "</td></tr>" ;
			}

		}
		stream  << "</table></td></tr></table><div class=\"infologo\"></div><div class=\"info\">Universidad de Concepción<br>Colmena Virtual Cluster(2009)<br>http://colmena.udec.cl</div></body></html>" ;
		file.close() ;

	}
	generateHtmlLogo () ;
}

void SlaveKernel::generateHtmlLogo ( void )
{
	QFile file ( "stats/alt.html") ;
	QTextStream stream ;
	if ( file.open ( QIODevice::WriteOnly ) )
	{
		stream.setDevice ( &file ) ;
		stream << "<html><body><center><img src=\"udeclogobig.gif\"/><h1>Universidad de Concepción</h1></center></body></html>" ;
		file.close() ;

	}
}

void SlaveKernel::generateHtmlRes ( void )
{
	QFile::copy( ":/data/style.css" , "stats/style.css" ) ;
	QFile::copy( ":/data/udeclogo.jpg" , "stats/udeclogo.jpg" ) ;
	QFile::copy( ":/data/colmenalogo.jpg" , "stats/colmenalogo.jpg" ) ;
	QFile::copy( ":/data/udeclogobig.gif" , "stats/udeclogobig.gif" ) ;
}


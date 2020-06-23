#ifndef ___SLAVEKERNEL___
#define ___SLAVEKERNEL___

#include <QHostAddress>
#include <QString>
#include <QSqlDatabase>
#include <tasklist.hpp>
#include <QStringList>
#include <QTextStream>
#include <QTcpServer>
#include <QDateTime>

#define HISTORYACTION_COMPLETED 0
#define HISTORYACTION_ERROR 1
#define HISTORYACTION_OTHER 2

class SlaveMasterCommunicator ;
class QTcpSocket ;

class SlaveKernel
{
	public :
	
		static void addHistoryEntry ( const QString & app , int action , int secs ) ;
		static void generateHtml ( void ) ;
		static void generateHtmlRes ( void ) ;
		static void generateHtmlLogo ( void ) ;
	
		static int idslave ;
		static QHostAddress masterip ;
		static int masterport ;
		static int listenport ;
		static int maxtasks ;
		static bool alwayshigh ;
		static QString group ;
		static QString generalPageList ;
		static QString specificPageList ;
		static QSqlDatabase db ;
		static SlaveMasterCommunicator * masterCom ;
		static bool priority ;
		static TaskList taskList ;
		static QStringList envVars ;
		static QTextStream out ;
		static int pageId ;
		static QTcpSocket * ssaverSocket ;
		static QTcpServer ssaverListener ;
		static quint16 ssaverport ;
		
} ;


#endif

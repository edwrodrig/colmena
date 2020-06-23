#ifndef ___TASKLIST___
#define ___TASKLIST___

#include <QMap>
#include <taskindex.hpp>

class Message ;
class Task ;
class QTextStream ;

class TaskList : public QMap<TaskIndex,Task *>
{
	public :
		TaskList ( void ) ;
		void clear ( void ) ;
		void clearTask ( const TaskIndex & i ) ;
		void postTaskArrived ( const Message & m ) ;
		void sendArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ;
		
		void highPriority ( void ) ;
		void lowPriority ( void ) ;
		
		void generateHtml ( QTextStream & stream ) ;
} ;

#endif

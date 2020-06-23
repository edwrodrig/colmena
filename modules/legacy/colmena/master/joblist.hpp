#ifndef ___JOBLIST___
#define ___JOBLIST___

#include <QMap>
#include <QMutex>

class Job ;
class Message ;
class QByteArray ;
class InputStream ;

class JobList : public QMap<int,Job *>
{
	public :
		
		JobList ( void ) ;
                Job * postJob ( const QString & user ) ;
                void addJob ( Job * job ) ;
		
		bool availableTask ( int slave , int * idjob , int * idtask , QString * program ) ;
		
		//MESSAGES
		void acceptedTaskArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ;
		void sendArrived ( const Message & m ) ;
		void errorTaskArrived ( const Message & m ) ;

		// CLEAR

		void clearJob ( const QString & user , int job ) ;
		void clearTask ( const QString & user , int job , int task ) ;
		void clearJob ( const QString & user ) ;
                void clearWorkingTasks ( void ) ;
                void clearWorkingSlave ( int slave ) ;
		void freeTask ( const Message & m ) ;

		// OTHER
		void infoJob ( const QString & user , int job , InputStream & input ) ;
		void infoTask ( const QString & user , int job , int task , InputStream & input )  ;
		void jobList ( const QString & user , InputStream & input )  ;
		void taskList ( const QString & user , int job , InputStream & input ) ;
		bool isJobCompleted ( const QString & user ,  int job ) ;

        private :
                QMutex mutex ;
} ;

#endif

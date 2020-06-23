#ifndef ___JOB___
#define ___JOB___

#include <QSqlDatabase>
#include <QByteArray>
#include <datasenderlist.hpp>
#include <datareceiverlist.hpp>
#include <messagerepeaterlist.hpp>
#include <QMap>
#include <QMutex>

class Message ;
class InputStream ;
class Job
{
	public :
		Job ( const QString & user ) ;
		~Job ( void ) ;
		int id ( void ) const ;
                void open ( void ) ;
		bool isReady ( void ) const ;
		QString user ( void ) const ;
		bool availableTask ( int slave , int * id , QString * program ) ;				//
		bool isCompleted ( void ) const ;
		int completedTasks ( void ) const ;
		int totalTasks ( void ) const ;
		void clearWorkingTasks ( void ) ;				// resetea los tasks a disponibles que estan siendo procesados
		void clearWorkingSlave ( int slave ) ;			// resetea los tasks a disponibles que estan siendo procesados por un slave
		void clearWorkingTask ( int task ) ;			// resetea un task a disponible que esta siendo procesado
		QByteArray taskData ( int id ) ; //return the array of data compresed of the input task
		void sendAckArrived ( const Message & m ) ;
		bool acceptedTaskArrived ( const Message & m ) ;	//REGISTRA LA TASK A UN ESCLAVO Y DEVUELVE EL BYTEARRAY A ENVIAR
		void sendArrived ( const Message & m ) ;
		void freeTask ( const Message & m ) ;
		void infoJob ( InputStream & input ) ;
		void infoTask ( int task , InputStream & input ) ;
		void jobList ( const QString & user , InputStream & input ) ;
		void taskList ( InputStream & input ) ;
		void clearTask ( int task ) ;
	private :
		
		static int count ;

		int _id ;
		QString _user ;
		
		int _completedTasks ;
		int _totalTasks ;

		DataSenderList dataSenderList ;
		DataReceiverList dataReceiverList ;

            } ;

#endif




#ifndef ___TASK___
#define ___TASK___

#include <QObject>
#include <QDir>
#include <message.hpp>
#include <QDateTime>
#include <QProcess>

class MessageRepeater ;
class DataReceiver ;
class DataSender ;
class QProcess ;


class Task : public QObject
{
	Q_OBJECT
	public :
		static const quint8 WAITING 	= 0 ;
		static const quint8 RECEIVINGINPUT	= 1 ;
		static const quint8 PROCESSING = 2 ;
		static const quint8 ERROR = 3 ;
		
		static const quint8 FINISHED	= 4 ;
		static const quint8 SENDINGOUTPUT = 5 ;


		Task ( const Message & m , const QString & filePath , const QString & app ) ;
		~Task ( void ) ;
		void sendArrived ( const Message & m ) ;
		void sendAckArrived ( const Message & m ) ; 
		void highPriority ( void ) ;
		void lowPriority ( void ) ;

		QString app ( void ) const ;
		int idjob ( void ) const ;
		int idtask ( void ) const ;
		int state ( void ) const ;
		
	private slots :

		void finished ( int , QProcess::ExitStatus status ) ;
		void error ( void ) ;

	private :

		void deleteDir ( void ) ;
	
		Message message ;
		MessageRepeater * repeater ;
		DataReceiver * dataReceiver ;
		DataSender * dataSender ;
		QDateTime startTime ;
		
		QString program ;
		QString command ;
		QStringList retrieve ;
		
		QDir dir ;
		
		quint8 _state ;
		QProcess * process ;
} ;


#endif




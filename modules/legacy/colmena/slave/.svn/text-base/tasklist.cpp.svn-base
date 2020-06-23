#include <tasklist.hpp>
#include <message.hpp>
#include <slavekernel.hpp>
#include <slavemastercommunicator.hpp>
#include <QSqlQuery>
#include <QVariant>
#include <task.hpp>
#include <QMapIterator>
#include <QTextStream>

TaskList::TaskList ( void ) :QMap<TaskIndex , Task *>() {; }

void TaskList::clear ( void )
{
	QMapIterator<TaskIndex, Task *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		delete i.value() ;
		SlaveKernel::out << "Task deleted" << endl ;
    }
	QMap<TaskIndex, Task *>::clear() ;
}

void TaskList::clearTask ( const TaskIndex & i )
{
	Task * task = value ( i ) ;
	if ( task )
	{
		delete task ;
		remove ( i ) ;
		SlaveKernel::out << "Task " << i.idjob << "-" << i.idtask << " deleted." << endl ;
	}
}

void TaskList::postTaskArrived ( const Message & m )
{
	QSqlQuery query ( SlaveKernel::db ) ;
	query.prepare ( "SELECT app FROM apps WHERE name = ? ; " ) ;
	query.addBindValue ( QVariant ( m.program() ) ) ;
	if ( query.exec() && query.next() )
	{
		TaskIndex index ( m.idjob() , m.idtask() ) ;
		if ( ! contains( index ) )
		{
			insert ( index , new Task ( m , query.value(0).toString() , m.program() ) ) ;
			SlaveKernel::out << "Accepting tasks " << m.idjob() << "-" << m.idtask() << " program " << m.program() << endl ;
		}
	}
	else
	{
		SlaveKernel::out << "Not have program " << m.program() << ". Notifying to Master..." << endl ;
		SlaveKernel::masterCom->send ( m.createNotHaveApp() ) ;
	}
	
}

void TaskList::sendArrived ( const Message & m )
{
	TaskIndex index ( m.idjob() , m.idtask() ) ;
	if ( contains( index ) ) value ( index )->sendArrived ( m ) ;
}

void TaskList::sendAckArrived ( const Message & m )
{
	TaskIndex index ( m.idjob() , m.idtask() ) ;
	if ( contains( index ) )
	{
		if ( m.numpack() == -1 ) clearTask ( index ) ;
		else value ( index )->sendAckArrived ( m ) ;
	}
}


void TaskList::highPriority ( void )
{
	QMapIterator<TaskIndex, Task *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		i.value()->highPriority() ;
    }
}

void TaskList::lowPriority ( void )
{
	QMapIterator<TaskIndex, Task *> i ( *this ) ;
    while (i.hasNext() )
	{
		i.next();
		i.value()->lowPriority() ;
    }
}

void TaskList::generateHtml ( QTextStream & stream )
{
	if ( isEmpty() )
	{
		stream << "<div class=\"notask\">No hay tareas actualmente</div>" ;
	}
	else
	{
		QMapIterator<TaskIndex, Task *> i ( *this ) ;
		while (i.hasNext() )
		{
			i.next();
			stream << "<div class=\"task\"><ul><li>Tarea : " << i.value()->idjob() << "-" << i.value()->idtask() << "<li>Programa : " << i.value()->app() << "<li>Estado : " ;
			switch ( i.value()->state() )
			{
				case Task::WAITING :
					stream << "Esperando confirmación" ;
					break ;
				case Task::RECEIVINGINPUT :
					stream << "Reciviendo entrada" ;
					break ;
				case Task::PROCESSING :
					stream << "Procesando" ;
					break ;
				case Task::ERROR :
					stream << "Error" ;
					break ;
				case Task::FINISHED	:
					stream << "Terminado" ;
					break ;
				case Task::SENDINGOUTPUT :
					stream << "Enviando salida" ;
					break ;
			}
			stream << "</ul></div>" ;
		}
    }
}


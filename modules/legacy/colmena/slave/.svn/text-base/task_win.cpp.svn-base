#include <task.hpp>
#include <QProcess>
#include <windows.h>

void Task::highPriority ( void )
{
	if ( process )
	{
		QProcess::execute ( QString("setpriority.exe -p128 -t2 ") + QString::number( (quint64)( ( process->pid() )->dwProcessId ) ) ) ;
	}
}
void Task::lowPriority ( void )
{
	if ( process )
	{
		QProcess::execute ( QString("setpriority.exe -lowest " ) + QString::number( (quint64)( ( process->pid() )->dwProcessId ) ) ) ;
	}
}


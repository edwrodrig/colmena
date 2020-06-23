#ifndef MASTERCLIENTTHREAD_HPP
#define MASTERCLIENTTHREAD_HPP

#include <QThread>
#include <message.hpp>
class MasterSlaveThread : public QThread
{
Q_OBJECT
public:
    MasterSlaveThread( const Message & m , QObject * parent);
    void run() ;
private :
	Message m ;
};

#endif // MASTERCLIENTTHREAD_HPP

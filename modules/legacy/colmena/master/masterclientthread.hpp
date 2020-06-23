#ifndef MASTERCLIENTTHREAD_HPP
#define MASTERCLIENTTHREAD_HPP

#include <QThread>

class MasterClientThread : public QThread
{
Q_OBJECT
public:
    MasterClientThread( int socketDescriptor , QObject * parent);
    void run() ;
private :
    int socketDescriptor ;
};

#endif // MASTERCLIENTTHREAD_HPP

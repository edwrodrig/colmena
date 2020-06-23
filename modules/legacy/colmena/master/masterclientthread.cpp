#include "masterclientthread.hpp"
#include <masterclientcommunicator.hpp>
#include <masterkernel.hpp>
MasterClientThread::MasterClientThread( int s , QObject * parent) : QThread ( parent ) , socketDescriptor ( s )
{
}

void MasterClientThread::run()
{
        MasterClientCommunicator * com = new MasterClientCommunicator ( socketDescriptor ) ;
        com->waitForReadyRead() ;
        com->readClient() ;
        com->waitForDisconnected() ;
        delete com ;
}

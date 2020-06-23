#include <masterclientlistener.hpp>
#include <masterclientthread.hpp>

MasterClientListener::MasterClientListener( void ) : QTcpServer() { ; }

void MasterClientListener::incomingConnection ( int socketDescriptor )
{
        MasterClientThread *thread = new MasterClientThread(socketDescriptor ,this);
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
}

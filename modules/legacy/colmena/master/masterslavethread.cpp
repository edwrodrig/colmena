#include "masterslavethread.hpp"
#include <masterkernel.hpp>

MasterSlaveThread::MasterSlaveThread( const Message & message , QObject * parent) : QThread ( parent ) , m ( message )
{
}

void MasterSlaveThread::run()
{
        switch ( m.type() )
        {


                case Message::STATE :
                        MasterKernel::slaveList.stateArrived ( m ) ; 
                        break ;
                case Message::FULLSTATE :
                        MasterKernel::slaveList.fullStateArrived ( m ) ;
                        break ;
                case Message::ACCEPTEDTASK :
                        MasterKernel::jobList.acceptedTaskArrived ( m ) ;
                        break ;
                case Message::SEND :
                        MasterKernel::jobList.sendArrived ( m ) ;
                        break ;
                case Message::SENDACK :
                        MasterKernel::jobList.sendAckArrived ( m ) ;
                        break ;
                case Message::ERRORTASK :
                        MasterKernel::jobList.errorTaskArrived ( m ) ;
                        break ;
                case Message::PAGELISTACK :
                        MasterKernel::slaveList.pageListAckArrived ( m ) ;
                        break ;
                case Message::NOTHAVEAPP :
                        MasterKernel::slaveList.notHaveAppArrived ( m ) ;
                        break ;
        }


}

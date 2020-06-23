#include <masterkernel.hpp>
#include <masterclientcommunicator.hpp>
#include <masterslavecommunicator.hpp>

MasterClientListener * MasterKernel::clientCom = 0L ;
MasterSlaveCommunicator * MasterKernel::slaveCom = 0L ;
SlaveList MasterKernel::slaveList ;
JobList MasterKernel::jobList ;
QString MasterKernel::rootpassword ; 
int MasterKernel::pageId = 0 ;


#ifndef ___MASTERKERNEL___
#define ___MASTERKERNEL___

#include <slavelist.hpp>
#include <joblist.hpp>
#include <QString>

class MasterClientListener ;
class MasterSlaveCommunicator ;

class MasterKernel 
{
	public :
		
		static SlaveList slaveList ;
		static JobList jobList ;
			
                static MasterClientListener * clientCom ;
                static MasterSlaveCommunicator * slaveCom ;
		static QString rootpassword ;
		static int pageId ;

} ;


#endif


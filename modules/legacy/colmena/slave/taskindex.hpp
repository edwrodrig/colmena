#ifndef ___TASKINDEX___
#define ___TASKINDEX___

class TaskIndex
{
	public :
		inline TaskIndex ( int job , int task ) : idjob ( job ) , idtask ( task ) { ; }
		inline TaskIndex ( const TaskIndex & a ) : idjob ( a.idjob ) , idtask ( a.idtask ) { ; } 
		inline void operator=( const TaskIndex & a ) { idjob = a.idjob ; idtask = a.idtask ; }
		inline bool operator==( const TaskIndex & a ) const { return ( idjob == a.idjob ) && ( idtask == a.idtask ) ; }
		inline bool operator<=( const TaskIndex & a ) const
		{
			if ( idjob < a.idjob ) return true ;
			if ( idjob > a.idjob ) return false ;
			return ( idtask <= a.idtask ) ;
		}
		inline bool operator>=( const TaskIndex & a ) const 
		{
			if ( idjob > a.idjob ) return true ;
			if ( idjob < a.idjob ) return false ;
			return ( idtask >= a.idtask ) ;
		}
		inline bool operator<( const TaskIndex & a ) const
		{
			if ( idjob < a.idjob ) return true ;
			if ( idjob > a.idjob ) return false ;
			return ( idtask < a.idtask ) ;
		}
		inline bool operator>( const TaskIndex & a ) const
		{
			if ( idjob > a.idjob ) return true ;
			if ( idjob < a.idjob ) return false ;
			return ( idtask > a.idtask ) ;
		}
		
		int idjob ;
		int idtask ;

} ;

#endif


#include <iostream>
#include "solving_engine.h"

int main ( void ) 
{
	char zk [16] = { 3,6,10,4,2,9,8,12,1,14,7,16,5,13,11,15 };
	
	Engine solving_engine;
	solving_engine.set_up_and_run ( zk );
	return 0;

}

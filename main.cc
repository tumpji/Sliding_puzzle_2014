#include <iostream>
#include "solving_engine.h"
#include "comunication.h"

int main ( void ) 
{
	//char zk [16] = { 9,5,15,12,14,2,11,7,4,8,6,3,13,10,0,1 };
	const char * predane;
	
	//char zk [16] = { 3,6,10,4,2,9,8,12,1,14,7,0,5,13,11,15 }; // gprof v 1
	//char zk [16] = {12,15,2,6,1,14,4,8,5,3,7,0,10,13,9,11}; // gprof v 1
	
	char zk [9] = {0,1,2,3,4,5,6,7,8}; // 
	
	
	Engine solving_engine( 3 );
	solving_engine.run ((unsigned char * ) zk );
	
exit(0);
	Comunication rozhrani;
	std::cin.get();
	

	std::cout << "rozhrani.preber_usporadani()" << std::endl;
	
	std::cin.get();

	while ( 1 )
	{
		predane = rozhrani.preber_usporadani();
		if ( predane )
		{
			for( int y = 0; y < 4 ; ++y) 
			{
				for( int x = 0; x < 4 ; ++x) 
				{
					std::cout << (int)(predane[x + y*4]) << " " ;
					if ( predane[x + y*4] < 10 )
						std::cout << " ";
				}
				std::cout << std::endl;
			}
		}
		else
		{ 
			std::cout << "Chyba" << std::endl;
			exit( 1 );
		}

		std::cin.get();
	}


	return 0;

}

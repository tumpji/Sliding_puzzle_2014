#include <iostream>
#include "solving_engine.h"
#include "comunication.h"
#include <unistd.h>// us

int main ( void ) 
{
	//char zk [16] = { 9,5,15,12,14,2,11,7,4,8,6,3,13,10,0,1 };
	const char * predane;
	//char zk [16] = { 3,6,10,4,2,9,8,12,1,14,7,0,5,13,11,15 }; // gprof v 1
	//char zk [16] = {12,15,2,6,1,14,4,8,5,3,7,0,10,13,9,11}; // gprof v 1
	//char zk [9] = {0,1,2,3,4,5,6,7,8}; // 
	//char zk [9] = {2,6,7,5,3,8,4,0,1}; // 
	//char zk [25] = {1,2,3,0,4,6,20,22,7,5,11,8,23,24,10,21,17,19,12,14,9,16,13,15,18};

	std::vector< std::pair< int , OBJECT > > postup;
	Engine solving_engine( 4 );
	Comunication rozhrani;

while(1){
	std::cout << "Hotovo cekam na spusteni ... " << std::endl;
	std::cin.get();

	predane = rozhrani.preber_usporadani();

	assert( predane );
	postup = solving_engine.run ((unsigned char *)predane);
	

	for ( std::pair< int , OBJECT > aktualni : postup )
	{
		rozhrani.click_on_area ( aktualni.first );
		usleep( 50000 ); // 0.05 s
	}
}

	return 0;

}

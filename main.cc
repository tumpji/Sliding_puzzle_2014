#include <iostream>
#include "solving_engine.h"
#include "comunication.h"
#include <unistd.h>// us

int main ( void ) 
{
	char zk [16] = { 9,5,15,12,14,2,11,7,4,8,6,3,13,10,0,1 };
	const char * predane;
	//char zk [16] = { 3,6,10,4,2,9,8,12,1,14,7,0,5,13,11,15 }; // gprof v 1
	//char zk [16] = {12,15,2,6,1,14,4,8,5,3,7,0,10,13,9,11}; // gprof v 1
	//char zk [9] = {0,1,2,3,4,5,6,7,8}; // 
	//char zk [9] = {2,6,7,5,3,8,4,0,1}; // 
	//char zk [25] = {1,2,3,0,4,6,20,22,7,5,11,8,23,24,10,21,17,19,12,14,9,16,13,15,18};
	
	//char test_data[25] = {
	//{4,8,18,10,6,16,2,17,11,15,22,0,5,3,13,1,7,19,20,9,21,14,23,12,24},
	//{6,8,7,15,4,5,16,11,1,9,12,17,21,10,3,22,23,13,20,0,2,18,14,19,24},
	//{13,7,5,23,17,1,2,9,10,4,3,6,19,0,15,11,16,8,24,20,21,22,14,12,18},
	//{13,7,5,23,17,1,2,9,10,4,3,6,19,0,15,11,16,8,24,20,21,22,14,12,18},
	//{1,2,9,5,10,7,11,19,24,8,14,23,4,20,3,17,6,0,13,18,16,21,15,22,12},
	//1,2,3,5,10,12,6,19,14,8,21,9,23,17,13,16,15,4,24,22,7,20,18,0,11
	//};

	Engine solving_engine( 4 );
	//for ( int x = 0 ; x < 6 ; ++x )
	//{
		solving_engine.run ((unsigned char *)zk);
	//}

exit ( 0 );

	std::vector< std::pair< int , OBJECT > > postup;
	//Engine solving_engine( 4 );
	Comunication rozhrani;

	while ( 1 )
	{
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

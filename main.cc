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
	char zk [25] = {1,2,3,0,4,6,20,22,7,5,11,8,23,24,10,21,17,19,12,14,9,16,13,15,18};
/*	
	char test_data[6][25]= {
	{4,8,18,10,6,16,2,17,11,15,22,0,5,3,13,1,7,19,20,9,21,14,23,12,24},
	{6,8,7,15,4,5,16,11,1,9,12,17,21,10,3,22,23,13,20,0,2,18,14,19,24},
	{13,7,5,23,17,1,2,9,10,4,3,6,19,0,15,11,16,8,24,20,21,22,14,12,18},
	{13,7,5,23,17,1,2,9,10,4,3,6,19,0,15,11,16,8,24,20,21,22,14,12,18},
	{1,2,9,5,10,7,11,19,24,8,14,23,4,20,3,17,6,0,13,18,16,21,15,22,12},
	{1,2,3,5,10,12,6,19,14,8,21,9,23,17,13,16,15,4,24,22,7,20,18,0,11},
	};
*/
/*	
	char test_data[6][25]= {
	char test_data[18][25]= {
	{1,3,10,4,9,0,15,6,14,2,5,11,7,8,12,13},
	{1,7,2,4,5,3,14,0,11,8,15,12,10,9,6,13},
	{3,9,12,8,6,2,11,7,13,10,1,15,14,0,5,4},
	{6,1,7,12,5,0,2,8,4,10,14,3,13,11,9,15},
	{3,14,7,4,1,13,11,2,6,15,12,8,9,0,10,5},
	{6,13,12,4,14,0,2,8,1,9,5,15,7,3,10,11},
	{9,14,8,4,6,12,7,0,5,2,1,3,13,11,10,15},
	{8,7,1,12,2,0,5,3,9,6,11,15,13,10,14,4},
	{2,7,9,8,12,0,4,1,6,15,14,5,13,10,3,11},
	{2,5,3,4,15,9,1,8,6,11,13,10,14,7,12,0},
	{3,12,14,1,2,7,10,4,6,15,11,9,13,0,5,8},
	{8,4,1,5,9,2,6,10,14,13,3,12,7,0,11,15},
	{5,7,12,4,14,0,1,2,6,13,15,3,9,8,10,11},
	{1,4,12,8,10,0,5,9,2,11,6,15,13,14,3,7},
	{2,4,8,3,6,0,10,5,1,11,14,9,13,12,7,15},
	{1,14,8,3,9,10,12,0,6,7,15,2,5,4,13,11},
	{1,2,4,8,7,12,14,15,5,11,6,10,13,0,3,9},
	{9,12,8,3,2,0,1,5,11,7,10,4,14,6,15,13}
	};
*/	

	//Engine solving_engine( 4 );
//	std::cout << "size = " << sizeof(OBJECT ) << std::endl;

	//for ( int x = 0 ; x < 18 ; ++x )
	//{
		//solving_engine.run ((unsigned char *)test_data[x]);
	//}

//exit ( 0 );

	std::vector< std::pair< int , OBJECT > > postup;
	Engine solving_engine( 4 );
	Comunication rozhrani;
	int pocet_neuspesnych = 0;
	int pocet_uspesnych = 0;

	while ( 1 )
	{
		std::cout << "Hotovo cekam na spusteni ... " << std::endl;
		std::cin.get();

		predane = rozhrani.preber_usporadani();

		assert( predane );
		postup = solving_engine.run ((unsigned char *)predane);
		

		//for ( std::pair< int , OBJECT > aktualni : postup )
		for ( unsigned pozice = 0 ; pozice < postup.size()-1 ; ++pozice )
		{
		std::pair< int , OBJECT >&  aktualni = postup[pozice];
		int pocet_hu_baba = 0;
		int pocet_neuspesnych_in_row = 0;

			do{
				rozhrani.click_on_area ( aktualni.first );
				usleep( 200000 ); // 0.05 s
				predane = rozhrani.preber_usporadani();

				if ( predane == nullptr )
				{
					std::cerr << " Hu baba ... " << std::endl;
					sleep( 1 );
					if ( pocet_hu_baba++ > 1 ) break;
					continue;
				}

				if  ( memcmp( (void*)predane , (void*)aktualni.second.get_usporadani() ,
				4*4 ) 	== 0 )
				{
					++pocet_neuspesnych;
					usleep( 10000 );
					++pocet_neuspesnych_in_row;
				}
					
			}while( !predane || memcmp( (void*)predane , (void*)aktualni.second.get_usporadani() ,
				4*4 ) 	== 0		);
		
				pocet_neuspesnych += pocet_neuspesnych_in_row;
				std::cout << "Neuspel jsem " << pocet_neuspesnych_in_row << "x\n";
				if ( pocet_neuspesnych_in_row == 0 )
					++pocet_uspesnych;
			
		}

		std::cout << "Neuspesnych celkem : " << pocet_neuspesnych << std::endl;
		std::cout << "Uspesnych celkem   : " << pocet_uspesnych << std::endl;
		std::cout << "Posunuti celkem    : " << postup.size() << std::endl;
	
		pocet_neuspesnych = 0;
		pocet_uspesnych = 0;
	}

	return 0;

}

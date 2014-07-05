#include <iostream>
#include <cassert>
#include <unistd.h>// us
#include <time.h>
#include <cmath>
#include <climits>

#include "solving_engine.h"
#include "comunication.h"

#ifndef NDEBUG
#define PRINT( x ) std::cout << #x "  =   " << x << std::endl;
#else
#define PRINT( x ) 
#endif

struct 
{
	timespec time_beg;
	timespec time_end;
}time_str;

// klikne na poskladej puzzle a returnuje nasledujici usporadani
static const char * klik_poskladej_puzzle ( int argc , Comunication& rozhrani );

// uspe program na zadany pocet sec.
static void my_sleep ( float time );

// posklada policka dle predaneho postupu
static void skladani_policek ( Comunication& rozhrani, std::vector<std::pair<int, OBJECT>>& postup, const char * );


static void thinking_simulation ( unsigned size, unsigned pos );

static void thinking_simulation_last ( 	double & to_end, 
					float& optimum, 
					float& last_wait,
					unsigned vzdalenost_od_posledniho )
{
	std::cout << "th_sim  ";
	switch ( vzdalenost_od_posledniho )
	{
		case 0 :  // posledni
			if ( to_end >= ( 1/3.5 ) ) // je to vetsi nez nejrychlejsi co se da stihnout ?
			{
				last_wait = to_end; // presne
				my_sleep( last_wait );
			}
			else // nelze to stihnout 
			{
				last_wait = 1/3.5; // nejrychlejsi vyprodukovany
				my_sleep( last_wait );
			}
			break;

		case 1 : // predposledni
			if ( to_end/2. >= ( 1/3.5 ) )
			{
				// minimum + 2/3 toho co je navic
				last_wait = 1/3.5 + (to_end/2. - (1/3.5))*2./3. ;
				my_sleep( last_wait );
			}
			else  
			{	// jsme pozadu musi se dohnat to co jde ale nenapadne
				last_wait = 1/3.5;
				my_sleep( last_wait );
			}
			break;
		case 2 : // predpredposledni
			if ( to_end/3. >= ( 1/3.5 ) )
			{
				last_wait = 1/3.5 + (to_end/3. - (1/3.5))*3./5.;
				my_sleep( last_wait );
			}
			else
			{
				last_wait = 1/3.4;
				my_sleep ( last_wait );
			}
			break;
	}
	return;
}

static void thinking_simulation ( unsigned size , unsigned pos )
{
#ifndef NDEBUG
	std::cout << "thinking_simulation : " << size << "  " << pos << "   " << std::endl;
#endif

	static bool message_imposible = false;
	static float last_wait = 0;
	if ( pos == 0 ) message_imposible = false;

	const static float minimum_time = 1.f/( 3.5f );	// maximalni frekvence ( click/sec )
	const static float minimum_opt = 1.f/( 3.25f );	// nejvetsi rychlost prumerna

	timespec now; // aktualni cas
	clock_gettime ( CLOCK_REALTIME, &now ); 

	double to_end =	( - now.tv_sec + time_str.time_end.tv_sec ) +
			( - now.tv_nsec + time_str.time_end.tv_nsec )/1e9 ;

	float optimum = to_end/(float)( size-(pos+1) ); // sec/click
	float diff = optimum - minimum_time;
	int rand_modulo = (diff*2.*1e7)*5./6.;

	PRINT( to_end );
	PRINT( diff );
	PRINT( optimum );

	if ( size - ( pos + 1) < 3 && !message_imposible ) // predposledni a posledni cekani
	{	thinking_simulation_last ( to_end , optimum, last_wait, size - (pos+1) );
		return;
	}

	if ( optimum < minimum_opt || message_imposible ) {
		rand_modulo = 2.*((1./3.2) - minimum_time)*1e7; // velke opt
		if ( !message_imposible ) {
			if ( size - ( pos + 1 ) > 4 ) // neplati kdyz se uz blizime 
				message_imposible = true;
			std::cout << "Nelze stihnout " << std::endl;
		}
	}
	
	PRINT( rand_modulo );

	assert( rand_modulo  > 0 );
	rand_modulo = (rand()%rand_modulo);
	float time_wait = rand_modulo/1e7 + minimum_time;
	last_wait = time_wait;
	my_sleep( last_wait );
}

inline static void init_timer ( double time )
{
	clock_gettime( CLOCK_REALTIME , &time_str.time_beg ); // zacne se odpocitavat

	time_str.time_end.tv_sec = time_str.time_beg.tv_sec + (unsigned)time;

	time_str.time_end.tv_nsec = time_str.time_beg.tv_nsec + 
		(long)(( time - floor(time) )*1e9);

	if ( time_str.time_end.tv_nsec < 0 ) {// preteceni
		++time_str.time_end.tv_sec;
		time_str.time_end.tv_nsec -= LONG_MAX;
	}

	if ( time_str.time_end.tv_nsec >= 1e9 ) {
		++time_str.time_end.tv_sec;
		time_str.time_end.tv_nsec -= 1e9;
	}
	 

}


int main ( int argc , char* argv[] ) 
{
	const char * predane;
	double pozadovany_cas;
	float nej_cas = INFINITY;
	std::cout << "Pozadovany cas = " << std::flush;
	std::cin >> pozadovany_cas;
	std::cin.ignore( 100 , '\n' );
	if ( pozadovany_cas <= 10. )  exit( 1 );

	std::vector< std::pair< int , OBJECT > > postup;
	Engine solving_engine( 4 );
	Comunication rozhrani;

	// klikne na poskladej puzzle a prebere prvni rozpolozeni
   while( 1 ){ 
	std::cout << "Click -> poskladej puzzle " << std::endl;
	predane = klik_poskladej_puzzle ( argc , rozhrani );

	for ( int x = 0 ; x < 4*4 ; ++x )
		std::cout << (int)predane[x] << "  " ;
	std::cout << std::endl;
	
	my_sleep ( 20 ); // delete

	init_timer ( pozadovany_cas );
	// zacina se odpocitavat
	std::cout << "Zacinam pocitat ... " << std::endl;
	postup = solving_engine.run ((unsigned char *)predane);

#ifndef NDEBUG
	for ( std::pair<unsigned,OBJECT> p : postup )
		p.second.print();
#endif

	std::cout << "Vypocet skoncil, zacina skladani ..." << std::endl;
	skladani_policek ( rozhrani, postup, predane ); // posklada policka
	
// zaverecne zhodnoceni
	clock_gettime( CLOCK_REALTIME , &time_str.time_end ); // zacne se odpocitavat
	double cas = ( time_str.time_end.tv_sec - time_str.time_beg.tv_sec) +
		( time_str.time_end.tv_nsec - time_str.time_beg.tv_nsec )/1e9;
	std::cout << "Hotovo za : " << cas << std::endl;
	if ( cas < nej_cas ) nej_cas = cas;
	std::cout << "Nejlepsi je : " << nej_cas << std::endl;
	std::cout << "Cekam 3s" << std::endl;
	sleep ( 3 );
  }

	return 0;
}




// klikne na poskladej puzzle a pocka na odpoved a lag
// vraci aktualni usporadani
static const char * klik_poskladej_puzzle ( int argc, Comunication& rozhrani )
{
	const char * predane = nullptr;	
	bool complete = false;

	if ( argc < 2 )  { // jestli je to bez arg.
		std::cout << "Cekam na start ... " << std::endl;
		std::cin.clear(); // predchozi vstupy
		std::cin.ignore(  100 , '\n' );
		std::cin.get(); 
	}

	rozhrani.poskladej_puzzle_click ();

	do { // cekej dokud se nenachysta hraci pole
		usleep( 10000 );
		predane = rozhrani.preber_usporadani();
		if ( predane == nullptr ) continue;
		
		for ( int x = 0 ; x < 4*4 - 1 ; ++x )
			if ( predane[x] != x + 1 ) complete = true; // neni to or. obrazovka
	}
	while( !complete );
	return predane;
}

static void skladani_policek ( 
			Comunication& rozhrani, 
			std::vector<std::pair<int, OBJECT>>& postup,
			const char * predane )
{
	std::pair< int, OBJECT>  aktualni; 	// aktualni prvek ze ktereho se chceme dostat
						// kliknuti na index aktualni.first
						// se dostaneme z neho na dalsi v rade 

	std::cout << "pocet posunu = " << postup.size() << std::endl;

	for ( unsigned pozice = 0; pozice < postup.size(); ++pozice ) // iterace po vsech mez.
	{
		aktualni = postup[pozice]; // aktualni mezikrok

		rozhrani.click_on_area ( aktualni.first ); // klikne na index
		thinking_simulation( postup.size() , pozice );

		std::cout << '-' << std::flush;
		predane = rozhrani.preber_usporadani(); // zjisti jestli se posunulo

		// posledni -> potom nelze detekovat konec jinak nez cekanim
		if ( postup.size() - 1 == pozice ) break;

		while ( predane == nullptr || // uprostred animace -> cekej
			memcmp ( predane , aktualni.second.get_usporadani() , 4*4 ) == 0 )  // na stejnem obr.
		{
			if ( predane == nullptr ) {
				std::cout << "speedup..." << std::endl; break; }
			std::cout << '-' << std::flush; // aby se obnovil obraz
			predane = rozhrani.preber_usporadani(); my_sleep( 0.001 ); 
		}
		
		

	} // end for 

// zaverecne statistiky
	std::cout << "Konec hry " << std::endl;
}

// uspe program 
static void my_sleep ( float time )
{
#ifndef NDEBUG
	std::cout << "sleep : " << time << std::endl;
#endif
	unsigned int sec = (int)floor(time);
	unsigned int usec = (int)floor( (time - floor(time)) * 1e6 );

	assert ( sec < 30 );
	assert ( usec < 1000000 );

	if ( sec )
		sleep ( sec );
	if ( usec > 100 ) // minimum
		usleep( usec );
}

#include <cassert>
#include <iostream>
#include <new>
#include <set>

#include "solving_engine.h"
#include "object.h"

#include <pthread.h>

#define THREAD_COUNT 8



//static std::vector<OBJECT>* predej_vysledek ( const OBJECT * data, int position );
static bool zkontroluj_vstup ( const unsigned char * predane , unsigned int );
static std::vector<OBJECT>* predej_vysledek ( OBJECT * data , int position );

struct Setup_str
{
	Engine * engine;
	unsigned size;
	unsigned char * predane_usporadani;
	pthread_t threads[THREAD_COUNT];
	volatile unsigned enabled_computation;
	pthread_mutex_t enabled_c_mux;
}aktualni_vypocet;

// implementace IDA*

#if 0
#define PRINT_WAIT 	array_obj[position].print();\
			std::cin.get()
#else
#define PRINT_WAIT do{}while(0)
#endif

#define LOCK pthread_mutex_lock( &aktualni_vypocet.enabled_c_mux ); // lock
#define UNLOCK pthread_mutex_unlock( &aktualni_vypocet.enabled_c_mux ); // unlock

std::vector<OBJECT>*
Engine::run_ida ( const unsigned char * predane , unsigned size , unsigned thread_id )
{
	std::set<OBJECT> zasobnik_pouzivanych;
	unsigned int children_generated = 0;
	unsigned int depth_heur;
	int position = 0;
	OBJECT * array_obj = this->array_obj + 100*thread_id;
	
	//std::cout << "Zahajen vypocet " << size << "x" << size << std::endl;
	new ( array_obj ) OBJECT ( predane, size ); // inicializace korene
	depth_heur = array_obj->get_heuristic() + 2 + thread_id; // puvodni hloubka prohledavani

	for ( ; depth_heur < 100 ; depth_heur += THREAD_COUNT )
	{
		new ( array_obj ) OBJECT ( predane , size ); // inicializace korene
		position = 0;
		//LOCK;
		//std::cout << depth_heur << "\tvlakno :" << thread_id << std::endl; //<< "\t" << children_generated << std::endl;
		//UNLOCK;
		zasobnik_pouzivanych.insert ( *array_obj );
		assert ( zasobnik_pouzivanych.size () <= 1 );
	//PRINT_WAIT; 


		while ( position >= 0 )
		{
			// omezeni na hloubku
			while ( position + array_obj[position].get_heuristic() < depth_heur )
			{
				array_obj[position].generate_best_children ();
				// nemuze nastat chyba
				++position;
	//PRINT_WAIT; 
				if ( !zasobnik_pouzivanych.insert( array_obj[position] ).second )
					break; // jestlize jsem toto uz videl

	//			++children_generated;
				// je to cilovy obrazec ?
				if ( array_obj[position].get_heuristic() == 0 )
					return predej_vysledek ( array_obj, position );
			}

		if ( !aktualni_vypocet.enabled_computation )
			return nullptr;

			while ( --position >= 0 ) // backtrace jen do korene
			{
				// vyjmout z pouzivanych dat
				zasobnik_pouzivanych.erase ( array_obj[position+1] );
				
				// pokud neni mozne vytvorit dalsi dite
				if ( array_obj[position].generate_best_children () )
					continue;
				// je tento vygenerovany obj. mensi nez omezeni
				else if ( position + array_obj[1+position].get_heuristic() < depth_heur )
				{	
					++position; // ano -> do zasobniku
	//PRINT_WAIT; 
					if ( !zasobnik_pouzivanych.insert( array_obj[position] ).second )
						continue; // pokud uz je v zasobniku

	//				++children_generated;
					// je to cilovy obrazec ?
					if ( array_obj[position].get_heuristic () == 0 )
						return predej_vysledek ( array_obj, position );
					break;
				}

			}

		} // 1. while
	} // for end
	return nullptr;
}


// prevede array objektu na vektor objektu
// druhe predane cislo je velikost arraye
static std::vector<OBJECT>* predej_vysledek ( OBJECT * data , int position )
{
	if ( aktualni_vypocet.enabled_computation == false ) return nullptr; // check

	pthread_mutex_lock( &aktualni_vypocet.enabled_c_mux ); // lock

	if ( aktualni_vypocet.enabled_computation == false )
	{ UNLOCK; return nullptr;} // check

	aktualni_vypocet.enabled_computation = false; // set up
	std::cout << "Jsem prvni ... ukoncuji vypocty ostatnim .. " << std::endl;
	
	std::vector< OBJECT >* ret_obj = new std::vector< OBJECT >;

	for ( int x = 0 ; x <= position ; ++x )
	{
		ret_obj->push_back ( data[x] );
		data[x].print();
	}

	pthread_mutex_unlock( &aktualni_vypocet.enabled_c_mux ); // unlock
	return ret_obj;
}



Engine::Engine ( unsigned int size ) : size( size )
{
	assert ( size >= 3 && size <= 5 );
	// melo by stacit
	//array_obj = new 0BJECT ( );// [ 500 ];
	array_obj = (OBJECT*)malloc( sizeof(OBJECT) * 100 * THREAD_COUNT );
	//aloc_mem = (OBJECT*)malloc( sizeof(OBJECT) * 500 );

	//array_obj = aloc_mem;
	//array_obj = (OBJECT*)((char*)aloc_mem + 64 - ((unsigned long)(char*)aloc_mem)%64);
}

Engine::~Engine ()
{
	//delete [] array_obj;
	free ( array_obj );
}


// spolupracuje s pthread.h
void* callHandle ( void * data )
{
	return (void*)aktualni_vypocet.engine->run_ida ( 	
					aktualni_vypocet.predane_usporadani ,
					aktualni_vypocet.size , 
					(int)(long)data );
	
}

// spusti cely vypocet a vrati vektor paru
// .first = kam se ma kliknout ( index )
// .second = jak ma po tomto kliku vypadat plocha
std::vector<std::pair<int,OBJECT>> Engine::run ( const unsigned char * predane_usporadani )
{
	std::vector < std::pair<int, OBJECT> > vysledek;
	std::vector < OBJECT >* ida_return = nullptr;
	OBJECT now, next;
	if ( zkontroluj_vstup( predane_usporadani , size ) ) // kontrola vstupu
	{ std::cerr << "Neplatny vstup \n"; exit(1); }

	pthread_mutex_init( &aktualni_vypocet.enabled_c_mux , 0 ); // mutex init

	for ( unsigned actual_size = size ; actual_size >= 3 ; --actual_size )
	{
		aktualni_vypocet.engine = this;
		aktualni_vypocet.predane_usporadani = (unsigned char *)predane_usporadani;
		aktualni_vypocet.size = actual_size;	
		aktualni_vypocet.enabled_computation = true;	
		// vypocet v predane_usporadani je ukazatel na data
		for ( int x = 0; x < THREAD_COUNT ; ++x )
			pthread_create( &(aktualni_vypocet.threads[x]), 0 , &callHandle, (void*)(long)x );

		std::vector < OBJECT > * temp;
		for ( int x = 0; x < THREAD_COUNT ; ++x ) // sbirani vyrobenych
		{
			pthread_join (  aktualni_vypocet.threads[x] , (void**)&temp );
			if ( temp != nullptr ) ida_return = temp;
		}
		//ida_return = run_ida ( predane_usporadani , actual_size ); 
		
		assert( ida_return->size() >= 2 );


		// ida_return muze byt 3x3 a nebo 4x4 musime prevest na size*size
		// nasledne se zjisti index
		for ( unsigned pos = 0; pos < ida_return->size(); ++pos )
		{
			now = next;
			next = (*ida_return)[pos];
			next.convert_up(size); // konverze na velikost size*size
			if ( pos == 0 /* && size == actual_size */ ) continue;
			int index = now.get_change( next ); // where to click
			vysledek.push_back( // container
				std::make_pair( index , now ) );
		}
		
		now = *ida_return->rbegin(); // last element
		ida_return->clear(); // clear
		predane_usporadani = now.convert(); // convert down and return 
	}
/*
	for ( std::pair< int , OBJECT > elem : vysledek )
	{
		std::cout << "Klikni na : " << elem.first << std::endl;
		elem.second.print();
	}
*/

	pthread_mutex_destroy ( &aktualni_vypocet.enabled_c_mux ); // mutex destroy
	return vysledek;

}

//**************************************************************************************
//

static bool zkontroluj_vstup ( const unsigned char * predane , unsigned int size )
{
	for ( unsigned hledane = 0; hledane < size * size ; ++hledane )
	{
		unsigned int iter = 0;
		for ( ; iter < size * size ; ++iter )
			if ( predane[iter] == hledane )
				break;
		if ( iter == size * size )
			return true;
	}
	return false;
}



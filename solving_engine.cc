#include <cassert>
#include <iostream>
#include <new>
#include <set>

#include "solving_engine.h"
#include "object.h"

static std::vector<OBJECT> predej_vysledek ( const OBJECT * data, int position );
static bool zkontroluj_vstup ( const unsigned char * predane , unsigned int );
static std::vector<OBJECT> predej_vysledek ( OBJECT * data , int position );

// implementace IDA*

#if 0
#define PRINT_WAIT 	array_obj[position].print();\
			std::cin.get()
#else
#define PRINT_WAIT do{}while(0)
#endif

std::vector<OBJECT>
Engine::run_ida ( const unsigned char * predane , unsigned size )
{
	std::set<OBJECT> zasobnik_pouzivanych;
	unsigned int children_generated = 0;
	const int max_depth = 498;
	unsigned int depth_heur;
	int position = 0;

	new ( array_obj ) OBJECT ( predane, size ); // inicializace korene
	depth_heur = array_obj->get_heuristic() + 2; // puvodni hloubka prohledavani

	for ( ; depth_heur <= max_depth ; ++depth_heur )
	{
		new ( array_obj ) OBJECT ( predane , size ); // inicializace korene
		position = 0;
		std::cout << depth_heur << std::endl; //<< "\t" << children_generated << std::endl;
		zasobnik_pouzivanych.insert ( *array_obj );
		assert ( zasobnik_pouzivanych.size () <= 1 );
	PRINT_WAIT; 

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
	return std::vector<OBJECT>();
}


// prevede array objektu na vektor objektu
// druhe predane cislo je velikost arraye
static std::vector<OBJECT> predej_vysledek ( OBJECT * data , int position )
{
	std::vector< OBJECT > ret_obj;

	for ( int x = 0 ; x <= position ; ++x )
	{
		ret_obj.push_back ( data[x] );
		//data[x].print();
	}


	return ret_obj;
}



Engine::Engine ( unsigned int size ) : size( size )
{
	assert ( size >= 3 && size <= 5 );
	// melo by stacit
	//array_obj = new 0BJECT ( );// [ 500 ];
	array_obj = (OBJECT*)malloc( sizeof(OBJECT) * 500 );
}

Engine::~Engine ()
{
	//delete [] array_obj;
	free ( array_obj );
}

// spusti cely vypocet a vrati vektor paru
// .first = kam se ma kliknout ( index )
// .second = jak ma po tomto kliku vypadat plocha
std::vector<std::pair<int,OBJECT>> Engine::run ( const unsigned char * predane_usporadani )
{
	std::vector < std::pair<int, OBJECT> > vysledek;
	std::vector < OBJECT > ida_return;
	OBJECT now, next;
	if ( zkontroluj_vstup( predane_usporadani , size ) ) // kontrola vstupu
	{ std::cerr << "Neplatny vstup \n"; exit(1); }


	
	for ( unsigned actual_size = size ; actual_size >= 3 ; --actual_size )
	{
		
		// vypocet v predane_usporadani je ukazatel na data
		ida_return = run_ida ( predane_usporadani , actual_size ); 
		assert( ida_return.size() >= 2 );
		// ida_return muze byt 3x3 a nebo 4x4 musime prevest na size*size
		// nasledne se zjisti index
		for ( unsigned pos = 0; pos < ida_return.size(); ++pos )
		{
			now = next;
			next = ida_return[pos];
			next.convert_up(size); // konverze na velikost size*size
			if ( pos == 0 /* && size == actual_size */ ) continue;
			int index = now.get_change( next ); // where to click
			vysledek.push_back( // container
				std::make_pair( index , now ) );
		}
		
		now = *ida_return.rbegin(); // last element
		ida_return.clear(); // clear
		predane_usporadani = now.convert(); // convert down and return 
	}
/*
	for ( std::pair< int , OBJECT > elem : vysledek )
	{
		std::cout << "Klikni na : " << elem.first << std::endl;
		elem.second.print();
	}
*/
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



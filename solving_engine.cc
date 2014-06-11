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
std::vector<OBJECT>
Engine::run_ida ( const unsigned char * predane , unsigned size )
{
	std::set<OBJECT> zasobnik_pouzivanych;
	unsigned int children_generated = 0; 
	const int max_depth = 498;
	unsigned int depth_heur;
	int position = 0;
	
	new ( array_obj ) OBJECT ( predane, size ); // inicializace korene 
	depth_heur = array_obj->get_heuristic(); // puvodni hloubka prohledavani

	for ( ; depth_heur <= max_depth ; ++depth_heur )
	{
		new ( array_obj ) OBJECT ( predane , size ); // inicializace korene 
		position = 0;
		std::cout << depth_heur  << "\t" << children_generated << std::endl;
		assert ( zasobnik_pouzivanych.size ()  <= 1 );
		zasobnik_pouzivanych.insert ( *array_obj );

		while ( position >= 0 )
		{
			// omezeni na hloubku
			while ( position + array_obj[position].get_heuristic() < depth_heur ) 
			{
				array_obj[position].generate_best_children ();
				// nemuze nastat chyba
				++position;
				if ( !zasobnik_pouzivanych.insert( array_obj[position] ).second )
					break; // jestlize jsem toto uz videl

				++children_generated;
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
					if ( !zasobnik_pouzivanych.insert( array_obj[position] ).second )
						continue; // pokud uz je v zasobniku
				
					++children_generated;
					// je to cilovy obrazec ?
					if ( array_obj[position].get_heuristic () == 0 ) 
						return predej_vysledek ( array_obj, position );
					break;
				}
	
			} 
			
		} // 1. while 
	} // for end 
//	return std::vector<OBJECT>();
}


// prevede array objektu na vektor objektu 
// druhe predane cislo je velikost arraye
static std::vector<OBJECT> predej_vysledek ( OBJECT * data , int position )
{
	std::vector< OBJECT > ret_obj;

	for ( int x = 0 ; x < position ; ++x )
	{
		ret_obj.push_back ( data[x] );
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
        std::vector < OBJECT > ida_return;

        if ( zkontroluj_vstup( predane_usporadani , size ) ) // kontrola vstupu
        {  std::cerr << "Neplatny vstup \n"; exit(1); }


	// zatim
         assert ( size == 3 );

	ida_return = run_ida ( predane_usporadani , size ); // prvni nemusime deformovat
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





#include "solving_engine.h"
#include <cassert>
#include <iostream>
#include <new>
#include <set>

static bool zkontroluj_vstup ( const char predane [ CONST::policek ] )
{
	for ( int hledane = 0 ; hledane < CONST::policek ; ++hledane )
	{
		int iter = 0 ;
		for ( ; iter < CONST::policek ; ++iter )
			if ( predane[iter] == hledane )
				break;
		if ( iter == CONST::policek )
			return false;
	}

	return true;
}

// je potreba alokovat dostatek pameti
Engine::Engine ()
{
	array_obj = new OBJECT[500];	
}

Engine::~Engine ()
{
	delete [] array_obj;
}

// implementace IDA*
int Engine::set_up_and_run ( const char predane [CONST::policek] )
{
	std::set<OBJECT> zasobnik_pouzivanych;
	unsigned int children_generated = 0; 
	const int max_depth = 498;
	int depth_heur;
	int position = 0;
	
	if ( !zkontroluj_vstup ( predane ) )
		return 0;
//	OBJECT best_heur_obj ( predane );

	new ( array_obj ) OBJECT ( predane ); // inicializace korene 
	depth_heur = array_obj->get_heuristic(); // puvodni hloubka prohledavani

	for ( ; depth_heur <= max_depth ; ++depth_heur )
	{
		new ( array_obj ) OBJECT ( predane ); // inicializace korene 
		position = 0;
		std::cout << depth_heur  << "\t" << children_generated << std::endl;
		assert ( zasobnik_pouzivanych.size ()  <= 1 );
		zasobnik_pouzivanych.insert ( *array_obj );

		while ( position >= 0 )
		{
			// omezeni na hloubku
			while ( position + array_obj[position].get_heuristic() < depth_heur ) 
			{
				array_obj[position].generate_best_children_next_to ();
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
				if ( array_obj[position].generate_best_children_next_to () )
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

	return 0; // nepravdepodobne
}

int Engine::predej_vysledek ( OBJECT* start, int& position )
{

	for ( int x = 0 ; x < 200 ; ++x )
		std::cout << "*";
	std::cout << std::endl;

	for ( int x = 0 ; x < position ; ++x )
		start[x].print_obj ();
	
 	return position;
}








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

int Engine::set_up_and_run ( const char predane [CONST::policek] )
{
	int max_depth = 498;
	int depth_heur = 1;
	int position = 0;
	unsigned int __children_generated = 0;
	std::set<OBJECT> zasobnik_pouzivanych;
	
	
	if ( !zkontroluj_vstup ( predane ) )
		return 0;
//	OBJECT best_heur_obj ( predane );

	new ( array_obj ) OBJECT ( predane ); // inicializace korene 
	depth_heur = array_obj->get_heuristic(); // puvodni hloubka prohledavani

	for ( ; depth_heur <= max_depth ; ++depth_heur )
	{
		new ( array_obj ) OBJECT ( predane ); // inicializace korene 
		position = 0;
		std::cout << depth_heur  << "\t" << __children_generated << std::endl;
		assert ( zasobnik_pouzivanych.size ()  <= 1 );
		zasobnik_pouzivanych.insert ( *array_obj );

//	best_heur_obj.print_obj();

		while ( position >= 0 )
		{
			// omezeni na hloubku
			while ( position + array_obj[position].get_heuristic() < depth_heur ) 
			{
				array_obj[position].generate_best_children_next_to ();
				// predpoklada se ze zde se nemuze nic stat 
				++position;
				if ( !zasobnik_pouzivanych.insert( array_obj[position] ).second )
					break;

//				for ( int pos = 0 ; pos < position - 1 ; ++pos )
//					if ( array_obj[pos] == array_obj[position] )
//						break;
#if 0
	if ( best_heur_obj.get_heuristic () >= array_obj[position].get_heuristic() )
		best_heur_obj = array_obj[position];
				++__children_generated;
#endif
				if ( array_obj[position].is_sorted () ) // je to cilovy obrazec ?
					return predej_vysledek ( array_obj, position );
//				if ( __children_generated  )
//					best_heur_obj.print_obj ();
			}

#if 0
		static int uz_byl = 0;
			if ( uz_byl )
				{ 
					array_obj[position].print_obj (); 
					std::cout << array_obj[position].get_heuristic () << std::endl;
					--uz_byl; 
				}
#endif
			while ( --position >= 0 ) // backtracking jen do korene
			{
				zasobnik_pouzivanych.erase ( array_obj[position+1] );
				if ( array_obj[position].generate_best_children_next_to () )
					continue;
				else if ( position + array_obj[1+position].get_heuristic() < depth_heur )
				{	
					++position;

					if ( !zasobnik_pouzivanych.insert( array_obj[position] ).second )
						continue;
				
//					for ( int pos = 0 ; pos < position - 1 ; ++pos ) // kdyz se najde v seznamu
//						if ( array_obj[pos] == array_obj[position] )
//							continue;	
				++__children_generated;
#if 0
					
	if ( best_heur_obj.get_heuristic () >= array_obj[position].get_heuristic() )
		best_heur_obj = array_obj[position];
//				if ( __children_generated  )
//					best_heur_obj.print_obj ();
#endif
					if ( array_obj[position].is_sorted () ) // je to cilovy obrazec ?
						return predej_vysledek ( array_obj, position );
					break;
				}
	
			} 
			
		} // 1. while 
	} // for end 
	return 0;
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








#include "solving_engine.h"
#include <iostream>
#include <new>

inline static void copy_children ( const OBJECT * const source,
					OBJECT * const destination )
{
	new ( destination ) OBJECT ( *source );
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
	int max_depth = 499;
	int depth = 1;
	int position = 0;
	OBJECT * returned = nullptr;
	unsigned int __children_generated = 0;

	for ( ; depth <= max_depth ; ++depth )
	{
		new ( array_obj ) OBJECT ( predane ); // inicializace korene 
		returned = array_obj; // nastavi se aktualni na koren
		position = 0;
		std::cout << depth  << "\t" << __children_generated << std::endl;

		while ( position >= 0 )
		{
			while ( position < depth ) // omezeni na hloubku
			{
				returned = array_obj[position].generate_best_children ();
				// predpoklada se ze zde se nemuze nic stat 
				++position;
				copy_children ( returned , array_obj + position );
				++__children_generated;
			}

			if ( array_obj[position].is_sorted () )
				return predej_vysledek ( position );

			while ( --position >= 0 ) // backtracking
			{
				returned = array_obj[position].generate_best_children ();
				if ( returned == nullptr )
					continue;
				else
				{	
					++position;
					copy_children ( returned , array_obj + position );
					++__children_generated;
					if ( (position == depth) // jestli je na konci 
						&& array_obj[position].is_sorted () )
						return predej_vysledek ( position );
					break;
				}
			} 
			
		} // 1. while 
	} // for end 
	return 0;
}

int Engine::predej_vysledek ( int& position )
{
 	return position;
}







#include "solving_engine.h"
#include <new>

// je potreba alokovat dostatek pameti
Engine::Engine ()
{
	array_obj = new OBJECT[500];	

}

Engine::~Engine ()
{
	delete [] array_obj;
}

void Engine::set_up_and_run ( const char predane [CONST::policek] )
{
	int max_depth = 499;
	int depth = 1;
	int position = 0;
	OBJECT * returned = NULL;


	for ( ; depth <= max_depth ; ++depth )
	{
		array_obj->OBJECT ( predane ); // inicializace prvniho
		returned = array_obj; // nastavi se aktualni na koren

		while ( 1 )
		{
			while ( position < depth ) // omezeni na hloubku
			{
				returned = array_obj[position].generate_best_children ();
				// predpoklada se ze zde se nemuze nic stat 
				++position;
				copy_children ( returned , array_obj + position );
			}

			returned = NULL;

			while ( returned == NULL ) // backtracking
			{
				--position;
				returned = array_obj[position].generate_best_children ();
				if ( returned !
			}

		}
		

	}
}

inline static void copy_children ( const OBJECT * const restrict source,
					OBJECT * const restrict destination )
{
	destination->OBJECT ( *source );
}






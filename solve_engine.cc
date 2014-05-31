#include "solve_engine.h"
#include<iostream>
using namespace std;
ENGINE::ENGINE ( )
{
	nodes = 0;
}

ENGINE::~ENGINE () 
{

}

void ENGINE::start ( const char zacatek [ DIAMETER ] )
{
	
	OBJECT start ( zacatek );
//	start.print_obj ();

	OBJECT_DISTANCE prom ( start );
	mapa_heur.insert ( prom );

	OBJECT_DISTANCE temp;  

while
	temp = prom.generate_children() ;

	while ( temp.is_valid () )
	{ 
		mapa_heur.insert ( prom );
		temp.print_obj ();
		temp = prom.generate_children() ;
	}
	
	temp.print_obj ();
//	mapa_heur.insert ( prom );

	

}

int main ( void ) 
{
	ENGINE engine;
	static char x [ DIAMETER ] = { 1,16,3,4,6,2,11,10,5,8,7,9,14,12,15,13 };

	engine.start (  x );
}


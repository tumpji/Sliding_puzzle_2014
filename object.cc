#include <vector>
#include <stdio.h>
#include "object.h"	


/* **********************************************************************************************
 * 				OBJECT
 *
 * 	tenhle objekt je jeden stav sliding puzzle 
 *
 * 	konstruktor - 	a) kopirovaci
 * 			b) konstuktor komunikujici s detekci obrazu ( rozhranim )
 * 				prebira pole o CONST::policek x char, kde CONST::POLICEK + 1  je mezera
 *
 * 	
 *
 * ******************************************************************************************** */

//  destruktor
OBJECT::~OBJECT () 
{
}

// kopirovaci konstruktor
OBJECT::OBJECT ( const OBJECT & objekt )
{
	for ( int x = 0 ; x < CONST::policek ; ++x ) // kopiruje usporadani
		usporadani[x] = objekt.usporadani[x];

	value = objekt.value; 		// kopiruje ohodnoceni
	children = objekt.children;	// kopiruje rozmnozovani
}

OBJECT::OBJECT ( const char predana [ CONST::policek ] )
{
	for ( int x = 0; x < CONST::policek ; ++x )
		usporadani[x] = predana[x];

	children = 0;
	evaluate (); // doplni value
}

/* zjisti "vzdalenost k idealnimu stavu"
 * dale doplni children promennnou
 */
char OBJECT::evaluate ( ) // ohodnoti objekt ( naplni value )
{
	int evaluate_v1 = 0; // jak daleko jsou ti co jsou mimo vzdaleni od cile
	int evaluate_v2 = 0; // kolik jich je mimo
	
	int temp;
	for ( int x = 0; x < CONST::policek; ++x )
	{
		if ( usporadani[x] == CONST::spravne_usporadani[x] ) // je na dobrem miste ?
			continue;
		else
		{
			++evaluate_v2; 
		
			temp = ( x + 1 )%CONST::sloupcu - ( usporadani[x]  )%CONST::sloupcu; // vypocet souradnice x
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;

			temp = x/CONST::radku - ( usporadani[x] - 1 )/CONST::radku;
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;
		}
	}
	
	value = ( evaluate_v1 >= evaluate_v2 )?( evaluate_v1 ):( evaluate_v2 );

	// doplneni children
	int void_position = 0;
	for (; void_position < CONST::policek; ++void_position )
		if ( usporadani[void_position] == CONST::policek )
			break;

	// oznaci "1" ty co nelze uz vytvorit - okraje |xx| _^ // snad 1x posun a minus / deleni
	if ( void_position%4 == 0 ) children |= 1;
	if ( void_position%4 == 3 ) children |= 2;
	if ( void_position/4 == 0 ) children |= 4;
	if ( void_position/4 == 3 ) children |= 8;
}

// prehodi 2 hodnoty
static inline void char_swap ( char& x1 , char& x2)
{
	char temp = x1;
	x1 = x2;
	x2 = temp;
} 

/* 
 * generuje potomka a vrati ho
 * jestlize potomek uz nelze vytvorit vrati objekt ktery ma children == -1
 */
OBJECT OBJECT::generate_children ( ) 
{
	int void_position;

	OBJECT generovany ( *this ); // vytvori potomka

	// 1. najdeme ' ' == 16
	for ( int x = 0; x < CONST::policek ; ++x )
		if ( generovany.usporadani[x] == CONST::policek )
			void_position = x; // najde aktualni
	
	int& x  = void_position;
	// 1. do leva ? == > modulo == 0
	if ( (~children) & 1 )
	{
		children |= 1;
		char_swap ( generovany.usporadani[x] , generovany.usporadani[x-1] );
		generovany.evaluate (); // spocitej si kolik stojis
		generovany.children |= 2; // zabrani zpetnemu vytvoreni 
	}
	// 2. do prava ? == > modulo == 3
	else if ( ~children & 2 ) 
	{
		children |= 2;
		char_swap ( generovany.usporadani[x] , generovany.usporadani[x+1] );
		generovany.evaluate (); // spocitej si kolik stojis
		generovany.children |= 1;// zabrani zpetnemu vytvoreni 
	}
	// 3. nahoru ? == > > /4 != 0
	else if ( ~children & 4 ) 
	{
		children |= 4;
		char_swap ( generovany.usporadani[x] , generovany.usporadani[x-4] );
		generovany.evaluate (); // spocitej si kolik stojis
		generovany.children |= 8;// zabrani zpetnemu vytvoreni 

	}
	// 4. dolu ? ==> /4 != 3
	else if ( ~children & 8 ) 
	{
		children |= 8;
		char_swap ( generovany.usporadani[x] , generovany.usporadani[x+4] );
		generovany.evaluate (); // spocitej si kolik stojis
		generovany.children |= 4;// zabrani zpetnemu vytvoreni 
	}
	else
	{
		// neco udelat
		generovany.children = -1;
		return generovany;
	}

	return generovany;
}

bool OBJECT::is_valid ( )
{
	return (children != -1);
}

void OBJECT::print_obj ( )
{

	printf ( "--- object_distance -- children: %i --- value: %i--------\n" , children , value );
	for ( int radek = 0 ; radek < CONST::radku ; ++radek )
	{
		for ( int sloupec = 0; sloupec < CONST::sloupcu ; ++sloupec )
		{
			int cislo = usporadani[radek*CONST::sloupcu + sloupec];
			if (  cislo <= 9 )
				putc( ' ', stdout  );
			printf ( " %i " , cislo  );
		}
		printf ( "\n" );
		fflush ( stdin );
	}
	printf ( "-------------------------\n" );
}













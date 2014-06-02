#include <stdio.h> // depr.
#include <iostream>
#include <climits>
#include "object.h"	


/* **********************************************************************************************
 * 				OBJECT
 *
 * 	tenhle objekt je jeden stav sliding puzzle / fifteen puzzle / jina varianta
 *
 * 	konstruktor - 	a) kopirovaci
 * 			b) konstuktor komunikujici s detekci obrazu ( rozhranim )
 * 				prebira pole o CONST::policek x char, 
 * 					kde (CONST::POLICEK + 1)  je mezera
 * 	generate_children_all -
 * 		a ) vygeneruje na predane misto array o 4 stavech posunuti 
 * 		b ) vraci cislo podle poctu vygenerovanych ( validnich )
 * 			I) to jsou takove co jiz tento obj. nevygeneroval
 * 		c ) dale seradi obj. tak, aby byli na konci tohoto array nevyuzite
 * 			I) tem nastavi -1 do children = not valid
 *
 * 	generate_best_children
 * 		a ) vraci ukazatel na nejlepe ohodnoceny vygenerovatelny obj.
 * 		b ) tento ukazatel se dalsim volanim teto funkce znehodnoti
 * 			I ) nutno zkopirovat 
 * 		c ) generuje jen to co uz predtim nevygeneroval
 * 		d ) kdyz nelze nic vygenerovat vraci NULL
 *
 * 	
 *
 * 	
 *
 ********************************************************************************************** */

// prehodi 2 hodnoty
template< class T >
void swap ( T& x1 , T& x2)
{
	T temp = x1;
	x1 = x2;
	x2 = temp;
} 

OBJECT::OBJECT ( )
{
	for ( int x = 0 ; x < CONST::policek ; ++x )
		usporadani[x] = 0;
	children = 0;
	value = 0; // v pripada chyby se obj.
}

// prevede pole typu char na objekt OBJECT
// dale inicializuje promennou value & children
//
OBJECT::OBJECT ( const char predana [ CONST::policek ] )
{
	for ( int x = 0; x < CONST::policek ; ++x )
		usporadani[x] = predana[x]; // kopirovani

	children = 0; // upravi pozdeji
	evaluate (); // doplni value
	inspect_near_elements ();
}

// kopirovaci konstruktor
OBJECT::OBJECT ( const OBJECT & objekt )
{
	for ( int x = 0 ; x < CONST::policek ; ++x ) // kopiruje usporadani
		usporadani[x] = objekt.usporadani[x];

	value = objekt.value; 		// kopiruje ohodnoceni
	children = objekt.children;	// kopiruje rozmnozovani
}

OBJECT::~OBJECT () 
{
}

// najde prazdne misto a zasle jeho index
int OBJECT::get_whitespace_pos ( )
{
	int void_position = 0;
	for (; void_position < CONST::policek; ++void_position ) // prochazi vsechno
		if ( usporadani[void_position] == CONST::policek ) // prazdne misto ?
			return void_position;

	std::cerr << "Nenasel jsem bily znak\n";
	exit ( 1 );
}

// zkontroluje, jestli lze vytvorit okolni posunuti - vlevo,vpravo...
void OBJECT::inspect_near_elements ()
{
	// oznaci "1" ty co nelze uz vytvorit 
	// zaroven jim da flag "jiz vygenerovany"
	int void_position = get_whitespace_pos ();
	children = 0;
	if ( void_position%4 == 0 ) children |= 0x11; // doleva nelze
	if ( void_position%4 == 3 ) children |= 0x22; // vpravo
	if ( void_position/4 == 0 ) children |= 0x44; // nahoru
	if ( void_position/4 == 3 ) children |= 0x88; // dolu
}


/*   ma za ukol ohodnotit stav dle heuristiky */
void OBJECT::evaluate ( ) 
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
			++evaluate_v2; // heuristika n1

			// vypocet souradnice x
			temp = ( x + 1 )%CONST::sloupcu - ( usporadani[x]  )%CONST::sloupcu; 
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;

			// vypocet souradnice y
			temp = x/CONST::radku - ( usporadani[x] - 1 )/CONST::radku;
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;
		}
	}

	// nejhorsi se zapocitava
	value = ( evaluate_v1 >= evaluate_v2 )?( evaluate_v1 ):( evaluate_v2 );
	return;
}

// object * dynamicky nealokuje -> nutne zkopirovat ukazatel, je nepouzitelny nasledne
OBJECT* OBJECT::generate_best_children ()
{
	static OBJECT temp [ 4 ]; // zasobnik 
	int white_space_pos = get_whitespace_pos (); // kde je mezera
	int best_value_index = -1; // nejlepsi z potomku

	for ( int obj = 0 ; obj < 4 ; ++obj ) // pro vsechny objekty
	{ 
		// nema smysl delat to co je jiz vytvoreno
		if ( children & ( 0x10 << obj ) ) continue; 

		// nejprve kopie usporadani do temp obj.
		for ( int pos = 0 ; pos < CONST::policek ; ++pos )
			temp[obj].usporadani[pos] = usporadani[pos]; 
		
		switch ( obj ) // modifikace 
		{
		case 0: // 1 -- leva
			swap ( 	temp[obj].usporadani[white_space_pos] ,
				temp[obj].usporadani[white_space_pos -1] );
			temp[obj].inspect_near_elements (); // kam se jde dal
			temp[obj].children |= 0x20; // zpetna zarazka
			break;
		case 1: // 2 -- pravo
			swap ( 	temp[obj].usporadani[white_space_pos] ,
				temp[obj].usporadani[white_space_pos +1] );
			temp[obj].inspect_near_elements (); // kam se jde dal
			temp[obj].children |= 0x10; // zpetna zarazka
			break;
		case 2: // 4 -- nahoru
			swap ( 	temp[obj].usporadani[white_space_pos] ,
				temp[obj].usporadani[white_space_pos - CONST::sloupcu] );
			temp[obj].inspect_near_elements (); // kam se jde dal
			temp[obj].children |= 0x80; // zpetna zarazka
			break;
		case 3: // 8 -- dolu 
			swap ( 	temp[obj].usporadani[white_space_pos] ,
				temp[obj].usporadani[white_space_pos + CONST::sloupcu] );
			temp[obj].inspect_near_elements (); // kam se jde dal
			temp[obj].children |= 0x40; // zpetna zarazka
			break;
		}
		
		temp[obj].evaluate (); // aplikuje heuristiku

		// porovna s ostatnimi vysledky heuristiky
		if ( 	best_value_index == -1 || 
			( temp[obj].value < temp[best_value_index].value ) )
			best_value_index = obj;
	}

	if (  best_value_index == -1 )
		return NULL;
	else
	{
		children |= 0x10 << best_value_index; // tohle uz nikdy nevytvori
		return &( temp[best_value_index] );
	}
}

int OBJECT::generate_children_all ( OBJECT data [4] )
{
	int offset = 0;
	int white_space_pos = get_whitespace_pos (); // kde je mezera

	for ( int obj = 0 ; obj < 4 ; ++obj )
	{
		if ( children & ( 0x10 << obj ) ) // nelze vytvorit
		{	
			offset -= 1;
			continue;
		}
		
		// kopie sebe do dat
		for ( int pos = 0; pos < CONST::policek; ++pos )
			data[offset + obj].usporadani[pos] = usporadani[pos];

		switch ( obj ) // modifikace 
		{
		case 0: // 1 -- leva
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos -1] );
			data[offset + obj].inspect_near_elements (); // kam se jde dal
			data[offset + obj].children |= 0x20; // zpetna zarazka
			break;
		case 1: // 2 -- pravo
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos +1] );
			data[offset + obj].inspect_near_elements (); // kam se jde dal
			data[offset + obj].children |= 0x10; // zpetna zarazka
			break;
		case 2: // 4 -- nahoru
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos - CONST::sloupcu] );
			data[offset + obj].inspect_near_elements (); // kam se jde dal
			data[offset + obj].children |= 0x80; // zpetna zarazka
			break;
		case 3: // 8 -- dolu 
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos + CONST::sloupcu] );
			data[offset + obj].inspect_near_elements (); // kam se jde dal
			data[offset + obj].children |= 0x40; // zpetna zarazka
			break;
		}

		data[offset + obj].evaluate ();
	}

	children |= 0xf0;
	return offset + 4;	
}


/*****************************************************************************/
/********************************** DEBUG ************************************/

// converts to binary
template< class T >
char * byte_to_binary ( T cislo )
{
	static char data [sizeof(cislo) * 8 + 1] = {0};

	for ( int x = 0; x < sizeof(cislo)*8; ++x )
		if ( cislo & ( 1 << x ) )
			data[x] = '1';
		else
			data[x] = '0';
	return data;
} 

// vytiskne informace o OBJEKTU posklada do citelneho poradi
void OBJECT::print_obj ( )
{
	printf ("------------ object_distance  ----------------"
		"children: 	%i %s\n"
		"value:		%i\n", 
		children, byte_to_binary(children), value );

	for ( int radek = 0 ; radek < CONST::radku ; ++radek )
	{
		for ( int sloupec = 0; sloupec < CONST::sloupcu ; ++sloupec )
		{
			int cislo = usporadani[radek*CONST::sloupcu + sloupec];
			if (  cislo <= 9 )
				putc( ' ', stdout );
			printf ( " %i " , cislo  );
		}
		printf ( "\n" );
	}
	printf ( "--------------------------------------------\n" );
	fflush ( stdin );
}













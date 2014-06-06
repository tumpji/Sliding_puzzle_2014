#include <cassert>
#include <iostream>
#include <climits>
#include <new>
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

// converts to binary
template< class T >
inline static const char (*to_binary ( T cislo ))[ sizeof(T)*8 + 1]
{
	static char data [ sizeof(T)*8 + 1 ];

	for ( int x = 0; x < sizeof(T)*8; ++x )
		if ( cislo & ( 1 << x ) )
			data[x] = '1';
		else
			data[x] = '0';
	return &data;
} 

// prehodi 2 hodnoty
template< class T >
inline static void swap ( T& x1 , T& x2)
{
	T temp = x1;
	x1 = x2;
	x2 = temp;
} 

// najde prazdne misto a zasle jeho index
// jestli ho nenajde => assert
inline static int get_whitespace_pos ( const char * const usporadani )
{
	int void_position = 0;

	for (; void_position < CONST::policek; ++void_position ) // prochazi vsechno
		if ( usporadani[void_position] == 0 ) // prazdne misto ?
			return void_position;

	assert ( !"Nenasel jsem bily znak\n" );
	exit( 1 );
}


OBJECT::OBJECT ( )
{
	for ( int x = 0 ; x < CONST::policek ; ++x )
		usporadani[x] = 0;
	children = 0;
	value = 0; // v pripada chyby se obj.
}


OBJECT::~OBJECT () 
{
}


// zkontroluje, jestli lze vytvorit okolni posunuti - vlevo,vpravo...
void OBJECT::inspect_near_elements ()
{
	// oznaci "1" ty co nelze uz vytvorit 
	// zaroven jim da flag "jiz vygenerovany"
	int void_position = get_whitespace_pos ( usporadani );
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
	int temp = 0;

	for ( int x = 0; x < CONST::policek; ++x )
	{
		if ( usporadani[x] == CONST::spravne_usporadani[x] ) // je na dobrem miste ?
			continue;
		else
		{
			// vypocet souradnice x
			temp = x%CONST::sloupcu - usporadani[x]%CONST::sloupcu; 
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;

			// vypocet souradnice y
			temp = x/CONST::radku -  usporadani[x]/CONST::radku;
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;
		}
	}

	// nejhorsi se zapocitava
	value = evaluate_v1;
	return;
}

// object * dynamicky nealokuje -> nutne zkopirovat ukazatel, je nepouzitelny nasledne
OBJECT* OBJECT::generate_best_children ()
{
	static OBJECT temp [ 4 ]; // zasobnik 
	int best_value_index = 0; // nejlepsi z potomku
	auto temp_children = children;
	int maximum = generate_children_all ( temp ); // generuje vsechny deti
	children = temp_children;

	if ( maximum == 0 ) return NULL; // nevygeneroval ani jeden

	for ( int pos = 1 ; pos < maximum ; ++pos ) // pro vsechny posekty
	{ 
		if ( 	temp[best_value_index].value >
			temp[pos].value )
		best_value_index = pos;	
	} // vybere nejlepsiho kandidata a poznaci se jeho index
	// musime zjistit co to bylo za posun
	
	int white_space = get_whitespace_pos ( usporadani );
	static int moves [ 4 ] = 
	{ CONST::MOVES::VLEVO , CONST::MOVES::VPRAVO , CONST::MOVES::NAHORU , CONST::MOVES::DOLU }; 
	static int tests [ 4 ] = { CONST::VLEVO , CONST::VPRAVO , CONST::NAHORU , CONST::DOLU };

	for ( int pos = 0 ; pos < 4 ; ++pos )	
	{
		if ( children & tests[ pos ]) // je to vubec mozne ?
			continue;

	  	if ( temp[best_value_index].usporadani[white_space + moves[pos]] !=
			usporadani[white_space] ) // vlevo
			continue;

		children |= tests[ pos ];
		return &( temp[best_value_index] );
	}

	// sem by se to nemelo dostat
	assert ( 0 );  
	exit ( 1 );
}

int OBJECT::generate_children_all ( OBJECT data [4] )
{
	int offset = 0;
	static const char zarazka [ 4 ] = { 0x20 , 0x10 , (char)0x80 , 0x40 };	
	int white_space_pos = get_whitespace_pos ( usporadani ); // kde je mezera

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
				data[offset + obj].usporadani[white_space_pos + CONST::MOVES::VLEVO] );
			break;
		case 1: // 2 -- pravo
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos + CONST::MOVES::VPRAVO] );
			break;
		case 2: // 4 -- nahoru
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos + CONST::MOVES::NAHORU] );
			break;
		case 3: // 8 -- dolu 
			swap ( 	data[offset + obj].usporadani[white_space_pos] ,
				data[offset + obj].usporadani[white_space_pos + CONST::MOVES::DOLU] );
			break;
		}

		data[offset + obj].inspect_near_elements (); // kam se jde dal 
		data[offset + obj].children |= zarazka[obj]; // zpetna zarazka
		data[offset + obj].evaluate (); // ohodnoti heuristikou
	}

	// nastavi, ze vsechny jsou pouzity !!!
	children |= CONST::VLEVO | CONST::VPRAVO | CONST::NAHORU | CONST::DOLU; 	
	return offset + 4;	// vrati kolik jich vytvoril ( offset <= 0 )
}


// optimalizovany konstruktor 
// kopiruje usporadani a nastavuje jiz predtim vypocitanou hodnotu heuristiky
// podle smeru predchoziho pohybu cislo od 0-3 vc. leva,prava,nahoru,dolu ( mezera )
// nastavi children
OBJECT::OBJECT (const OBJECT& polokopie , const int next_value , 
		const short smer_predchoziho_pohybu, const int whitespace_index )
{
	for ( int x = 0 ; x < CONST::policek ; ++x )
		usporadani[x] = polokopie.usporadani[x];

	switch ( smer_predchoziho_pohybu )
	{
		case 0: // doleva
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index - 1 ] );
			break;
		case 1: // doprava
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index + 1 ] );
			break;
		case 2: // nahoru 
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index - 4 ] );
			break;
		case 3: // dolu
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index + 4 ] );
			break;
	}

	value = next_value;
	inspect_near_elements (); // vynuluje children a zaplni je 	
	children |= 0x10 << smer_predchoziho_pohybu;
}


// generuje ( jestli to jde ) potomka hned pred sebe
// v pripada uspechu vraci false  chyba  === true
// optimalizovana varianta
bool OBJECT::generate_best_children_next_to ( )
{
	const int whitespace_index = get_whitespace_pos ( usporadani );
	if ( !( ~children % 0xf0 ) )
		return true; // neni kam pohnout 
	short smer_pohybu;
	int next_obj_value = get_hint ( *this , whitespace_index , smer_pohybu  );
	
	new ( this + 1 ) OBJECT ( *this , next_obj_value , smer_pohybu, whitespace_index ); // kopie sebe sama pred sebe xD ano je to tak
	return false; 
}

// vypocita nejlepsi moznost na posun
// vrati jeji velikost
//
// prebira original a index mezery
// a cislo, do ktereho ulozi posun:
// 	0 - vlevo
// 	1 - vpravo
// 	2 - nahoru
// 	3 - dolu
// 	nekontroluje jestli lze vubec nekam pohnout !
int OBJECT::get_hint ( const OBJECT& objekt, const int& whitespace_index, short& smer_posunu )
{
	int best_solution_value = INT_MAX;
	int temp;
	const int& x = whitespace_index;

	if ( !(objekt.children & 0x10) ) // je povolen posun doleva
	{
		temp = objekt.value + 1;// ohodnoceni org. 
					//  +1 protoze posouvame 0 do leva a ona chce byt v pravo
					//
		// [ x - 1 ] prvek doleva od mezery
		// (usporadani[x-1]%4 = do ktereho sloupce chce cislovano ( 0 , 1 , 2 , 3 )
		// whitespace_index % 4 = do ktereho sloupce zapadne
		if ( ( usporadani[x - 1] - 1 )%4 >= whitespace_index%4 )
			--temp;
		else
			++temp;
		// neni potreba zkoumat radky protoze ty se nepohnou

		best_solution_value = temp; // neni potreba podminka protoze je to prvni
		smer_posunu = 0; // 0 = posun vlevo
	}

	if ( !(objekt.children & 0x20) ) // je povolen posun doprava
	{
		temp = objekt.value - 1;// ohodnoceni org. 
					//  -1 protoze posouvame 0 do doprava a ona chce byt v pravo
					//
		// [ x + 1 ] prvek napravo od mezery
		// (usporadani[x+1]%4 = do ktereho sloupce chce cislovano ( 0 , 1 , 2 , 3 )
		// whitespace_index % 4 = do ktereho sloupce zapadne
		if ( ( usporadani[x + 1] - 1 )%4 <= whitespace_index%4 )
			--temp;
		else
			++temp;
		// neni potreba zkoumat radky protoze ty se nepohnou

		if ( best_solution_value > temp )
		{
			best_solution_value = temp; 
			smer_posunu = 1; // 0 = posun vpravo 
		}
	}

	if ( !(objekt.children & 0x40) ) // je povolen posun nahoru 
	{
		temp = objekt.value + 1;// ohodnoceni org. 
					//  +1 protoze posouvame 0 do nahoru a ona chce byt v dole
					//
		// [ x - 4 ] prvek nahore od mezery
		// (usporadani[x-4]%4 = do ktereho radku chce cislovano ( 0 , 1 , 2 , 3 )
		// whitespace_index / 4 = do ktereho radku zapadne
		if ( ( usporadani[x - 4] - 1 )/4 >= whitespace_index/4 )
			--temp;
		else
			++temp;
		// neni potreba zkoumat radky protoze ty se nepohnou

		if ( best_solution_value > temp )
		{
			best_solution_value = temp; 
			smer_posunu = 2; // 0 = posun vpravo 
		}
	}
		
	if ( !(objekt.children & 0x80) ) // je povolen posun dolu
	{
		temp = objekt.value - 1;// ohodnoceni org. 
					//  -1 protoze posouvame 0 do dolu a ona chce byt v dole
					//
		// [ x + 4 ] prvek dole od mezery
		// (usporadani[x-4]%4 = do ktereho radku chce cislovano ( 0 , 1 , 2 , 3 )
		// whitespace_index / 4 = do ktereho radku zapadne
		if ( ( usporadani[x + 4] - 1 )/4 <= whitespace_index/4 )
			--temp;
		else
			++temp;
		// neni potreba zkoumat radky protoze ty se nepohnou

		if ( best_solution_value > temp )
		{
			best_solution_value = temp; 
			smer_posunu = 3; // 0 = posun vpravo 
		}
	}

	// vime co bude
	return best_solution_value; // odevzdavame kolik bude value
}


/*****************************************************************************/
/********************************** DEBUG ************************************/

// vytiskne informace o OBJEKTU posklada do citelneho poradi
void OBJECT::print_obj ( )
{
	std::cout << 	"----------------------------------------\n"
		  <<	"children:\t" << (int)children << "\t" 
		  <<	*to_binary ( children ) << "\n"
		  <<	"value:\t" << value << std::endl;
//<< std::endl;

	for ( int radek = 0 ; radek < CONST::radku ; ++radek )
	{
		for ( int sloupec = 0; sloupec < CONST::sloupcu ; ++sloupec )
		{
			int cislo = usporadani[radek*CONST::sloupcu + sloupec];
			if (  cislo <= 9 ) // odsazeni
				std::cout << ' ' ;

			std::cout << " " << cislo << " ";
		}
		std::cout << std::endl;
	}

	std::cout <<	"-----------------------------------------" <<
	std::endl;
}

void OBJECT::print_obj_parser ( )
{
	std::cout << "(";
	for ( int pos = 0 ; pos < CONST::policek ; ++pos )
	{
		std::cout << (int)usporadani[pos] << ',';
	}
	std::cout << ")" ;
		
}












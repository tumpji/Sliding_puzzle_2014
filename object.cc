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

	for ( int x = 0; x < sizeof(T)*8; ++x, cislo>>=1)
		if ( cislo & 1 )
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

// zjisti jestli jsou zastoupeny vsechny cisla
static bool check_array_policek ( const char predane [CONST::policek] )
{
	bool ok = true;
        for ( int hledane = 0 ; hledane < CONST::policek ; ++hledane )
        {
                int iter = 0 ;
                for ( ; iter < CONST::policek ; ++iter )
                        if ( predane[iter] == hledane )
                                break;
                if ( iter == CONST::policek )
                        ok = false;
        }
	if ( ok )
        	return true;
	for ( int x = 0 ; x < CONST::policek ; ++x )
		std::cout << (int)predane[x] << ", ";
	std::cout << std::endl;
}


// zjisti jestli neni nejaka divna vec v dolnich bitech chidren var.
static bool check_children ( char x )
{
	int sum = 0;
	for ( int t = 0 ; t < 4 ; ++t )
	{
		if ( x & 1 )
			++sum;
		x >>= 1;
	}

	if ( sum < 3 ) 
		return true;

	return false;
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
	assert ( check_array_policek( usporadani ) );
	
	// oznaci "1" ty co nelze uz vytvorit 
	// zaroven jim da flag "jiz vygenerovany"
	int void_position = get_whitespace_pos ( usporadani );
	
	children = 0;
	if ( void_position%4 == 0 ) children |= 0x11; // doleva nelze
	if ( void_position%4 == 3 ) children |= 0x22; // vpravo
	if ( void_position/4 == 0 ) children |= 0x44; // nahoru
	if ( void_position/4 == 3 ) children |= 0x88; // dolu
	assert ( void_position >= 0 && void_position < CONST::policek );
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
			if ( usporadani[x] == 0 )
				temp = x%CONST::sloupcu - 3;
			else
				temp = x%CONST::sloupcu - (usporadani[x] - 1)%CONST::sloupcu; 
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;

			std::cout << "x: " << x << "\tusporadani[x] :" << (int)usporadani[x] << "\tTemp : " << temp ;

			// vypocet souradnice y
			if ( usporadani[x] == 0 ) // je to prazdne 
				temp = x/CONST::radku - 3;
			else
				temp = x/CONST::radku -  ( usporadani[x] - 1)/CONST::radku;
			if ( temp >= 0 )
				evaluate_v1 += temp;
			else
				evaluate_v1 -= temp;

			std::cout << "\tTemp : " << temp ;
			std::cout << "\tEval : " << evaluate_v1 << std::endl;
		}
	}

	// nejhorsi se zapocitava
	value = evaluate_v1;
	return;
}

// optimalizovany konstruktor 
// kopiruje usporadani a nastavuje jiz predtim vypocitanou hodnotu heuristiky
// podle smeru predchoziho pohybu cislo od 0-3 vc. leva,prava,nahoru,dolu ( mezera )
// nastavi children
OBJECT::OBJECT (const OBJECT& polokopie , const int next_value , 
		const short smer_predchoziho_pohybu, const int whitespace_index )
{
	assert ( next_value >= 0 );
	assert ( smer_predchoziho_pohybu >= 0 );
	assert ( smer_predchoziho_pohybu <= 3 );
	assert ( whitespace_index >= 0 && whitespace_index < CONST::policek );

	for ( int x = 0 ; x < CONST::policek ; ++x )
		usporadani[x] = polokopie.usporadani[x];

	value = next_value;		// value

	switch ( smer_predchoziho_pohybu )
	{
		case 0: // doleva
			assert ( whitespace_index - 1 >= 0 );
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index - 1 ] );
			inspect_near_elements (); 	// vynuluje children a zaplni je children dolni bity
			children |= 0x20; // byl posunut z prava do leva -> nemuze do prava
			assert ( usporadani[whitespace_index] >= 0 && usporadani[whitespace_index] < CONST::policek );
			assert ( usporadani[whitespace_index - 1] >= 0 && usporadani[whitespace_index - 1] < CONST::policek );
			break;
		case 1: // doprava
			assert ( whitespace_index + 1 < CONST::policek );
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index + 1 ] );
			inspect_near_elements (); 	// vynuluje children a zaplni je children dolni bity
			children |= 0x10; // podobne ale z leva do prava
			assert ( usporadani[whitespace_index] >= 0 && usporadani[whitespace_index] < CONST::policek );
			assert ( usporadani[whitespace_index + 1] >= 0 && usporadani[whitespace_index + 1] < CONST::policek );
			break;
		case 2: // nahoru 
			assert ( whitespace_index - 4 >= 0 );
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index - 4 ] );
			inspect_near_elements (); 	// vynuluje children a zaplni je children dolni bity
			children |= 0x80;
			assert ( usporadani[whitespace_index] >= 0 && usporadani[whitespace_index] < CONST::policek );
			assert ( usporadani[whitespace_index - 4] >= 0 && usporadani[whitespace_index - 4] < CONST::policek );
			break;
		case 3: // dolu
			assert ( whitespace_index + 4 < CONST::policek );
			swap ( 	usporadani[ whitespace_index     ] , 
			 	usporadani[ whitespace_index + 4 ] );
			inspect_near_elements (); 	// vynuluje children a zaplni je children dolni bity
			children |= 0x40;
			assert ( usporadani[whitespace_index] >= 0 && usporadani[whitespace_index] < CONST::policek );
			assert ( usporadani[whitespace_index + 4] >= 0 && usporadani[whitespace_index + 4] < CONST::policek );
			break;
	}
	assert ( check_children ( children ) );
}


// generuje ( jestli to jde ) potomka hned pred sebe
// v pripada uspechu vraci false  chyba  === true
// optimalizovana varianta
bool OBJECT::generate_best_children_next_to ( )
{
	assert ( value >= 0 );
	assert ( check_array_policek ( usporadani ) );
	assert ( check_children ( children ) ) ;

	const int whitespace_index = get_whitespace_pos ( usporadani );
	if ( !( ~children & 0xf0 ) )
		return true; // neni kam pohnout 
	short smer_pohybu;
	int next_obj_value = get_hint ( *this , whitespace_index , smer_pohybu  );

	// kopie sebe sama pred sebe xD ano je to tak
	new ( this + 1 ) OBJECT ( *this , next_obj_value , smer_pohybu, whitespace_index ); 
	
	this->children |= 0x10 << smer_pohybu; // zamezeni opakovani tohoto tahu
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
	assert ( objekt.value >= 0 );
	assert ( check_array_policek(objekt.usporadani) );
	assert ( ~objekt.children & 0xf0 );
	assert ( check_children ( children ) );
	assert ( whitespace_index >= 0 && whitespace_index < CONST::policek );

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
		assert ( x - 1 >= 0 );
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
		assert ( x + 1 < CONST::policek );
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
		assert ( x - 4 >= 0 );
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
		assert ( x + 4 < CONST::policek );
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

	assert ( smer_posunu >= 0 && smer_posunu <= 3 );
	assert ( best_solution_value >= 0 );
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












#include <cassert>
#include <cstring>
#include <new>

#ifndef NDEBUG
#include <iostream>
#endif

#include "object.h"

#define BLOCK_LEFT 	0x01
#define BLOCK_RIGHT 	0x02
#define BLOCK_UP	0x04
#define BLOCK_DOWN	0x08

static int vyhledej_nulu ( const unsigned char * data );
static unsigned char check_boarders ( const unsigned char * data , int size , unsigned char );
static inline unsigned heuristika_na_miru ( const unsigned char * data , unsigned int size );

static unsigned int heuristic_5x5 ( const unsigned char * data );
static unsigned int heuristic_4x4 ( const unsigned char * data );
static unsigned int heuristic_3x3 ( const unsigned char * data );

template < class T >
inline static void swap ( T& a, T& b )
{
	T temp = a;
	a = b;
	b = temp;
}
	
// *************************** KONSTRUKTORY / DE *****************************
// ***************************************************************************
//

// vola se k inicializaci korene u IDA*
// kopiruje, nastavuje flags , nastavuje heuristiku
OBJECT::OBJECT ( const unsigned char * data , unsigned short size ) : size(size)
{
	assert ( size <= 5 && size >= 3  );

	memcpy ( (void*)usporadani , (void*)data , size*size );

	whitespace_pos = vyhledej_nulu ( usporadani );

	flags = check_boarders( usporadani , size , whitespace_pos );

	heuristic = heuristika_na_miru ( usporadani , size );
}


// ma za ukol vygenerovat na adrese (this+1) kopii sebe
// tuhle kopii postupne menit a zjistit nejvhodnejsi rozpolozeni
// tohle zapsat do flags a nasledne 

#define SWAP( x ) \
	assert( whitespace_pos < size*size );\
	assert( whitespace_pos + (x) < size*size );\
	swap ( 	(this+1)->usporadani[whitespace_pos] , \
		(this+1)->usporadani[whitespace_pos+ (x) ] )
#define UPRAV_SCORE( x ) 	if( best_heuristic_value > temp ) {\
					best_heuristic_value = temp;\
					best_heuristic = x; }
bool OBJECT::generate_best_children ()
{
	unsigned int best_heuristic_value = (unsigned)-1;
	unsigned int best_heuristic = 0;
	unsigned int temp;
	assert ( usporadani[whitespace_pos] == 0 );

	//new (this + 1) OBJECT( *this ); // kopie
	
	if ( ! ( flags&BLOCK_LEFT ) ) // je povoleno posunuti smerem x
	{
		temp = heuristic;
		if ( size == 3 )
		{
			// nula jde na levo -> nechceme +1
			// [x-1] - prvek doleva od mezery
			// (usporadani[ x - 1] - 1) % 3 -- do ktereho sloupce chce
			// whitespace_pos % 3 = do ktereho sloupce zapadne
			if ( (unsigned)(usporadani[ whitespace_pos - 1] -1)%3 < whitespace_pos%3 )
				temp += 2;
			// neni treba zkoumat if
		} // neni 3 je neco vice
		else if ( ((usporadani[ whitespace_pos - 1] - 1)%size == 0) ||  
			  ((usporadani[ whitespace_pos - 1] - 1)/size == 0)  ) 
		{ 
			if ( (usporadani[ whitespace_pos - 1] -1)%size >= whitespace_pos%size )
				--temp;
			else
				++temp;
		}

		best_heuristic_value = temp; // je prvni nemusi se ptat
		best_heuristic = BLOCK_LEFT; 
	}

	if ( ! ( flags&BLOCK_RIGHT ) )
	{
		temp = heuristic;
		if ( size == 3 )
		{
			// nula jde na pravo -> chceme
			if ( (unsigned)(usporadani[whitespace_pos+1] -1 )%3 <= whitespace_pos%3 )
				temp -= 2;

		}
		else if ( ((usporadani[ whitespace_pos + 1] - 1)%size == 0) ||  
			  ((usporadani[ whitespace_pos + 1] - 1)/size == 0)  ) 
		{
			if ( (usporadani[whitespace_pos+1] -1 )%size <= whitespace_pos%size )
				--temp;
			else
				++temp;
		}
			
		UPRAV_SCORE(BLOCK_RIGHT);
	}

	if ( ! ( flags&BLOCK_UP) )
	{
		temp = heuristic;
		if ( size == 3 )
		{
			// nula jde nahoru -> nechceme
			if ( (unsigned)(usporadani[ whitespace_pos - size ] -1)/3 < whitespace_pos/3 )
				temp += 2;
			// neni treba zkoumat if
		} // neni 3 je neco vice
		else if ( ((usporadani[ whitespace_pos - size] - 1)%size == 0) ||  
			  ((usporadani[ whitespace_pos - size] - 1)/size == 0)  ) 
		{ 
			if ( (usporadani[ whitespace_pos - size] -1)/size >= whitespace_pos/size )
				--temp;
			else
				++temp;
		}

		UPRAV_SCORE(BLOCK_UP);
	}

	if ( ! ( flags&BLOCK_DOWN) )
	{
		temp = heuristic;
		if ( size == 3 )
		{
			// nula jde nahoru -> nechceme
			if ( (unsigned)(usporadani[ whitespace_pos + size ] -1)/3 <= whitespace_pos/3 )
				temp -= 2;
		} // neni 3 je neco vice
		else if ( ((usporadani[ whitespace_pos + size] - 1)%size == 0) ||  
			  ((usporadani[ whitespace_pos + size] - 1)/size == 0)  ) 
		{ 
			if ( (usporadani[ whitespace_pos + size] -1)/size <= whitespace_pos/size )
				--temp;
			else
				++temp;
		}

		UPRAV_SCORE(BLOCK_DOWN);
	}

	if ( best_heuristic_value == (unsigned)-1 )
		return true; // nepovednlo se 

	this->flags |= best_heuristic; // aby se nevytvarelo znova

	(this + 1)->heuristic = best_heuristic_value; // heuristic
	(this + 1)->size = size; // size
	// 1. cast kopirovani
	memcpy ( (void*)(this + 1)->usporadani , (void*)usporadani , size*size );

	// whitespace_pos , dokonceni usporadani	
	if ( best_heuristic == BLOCK_LEFT ) {
		SWAP ( -1 ); 			// transformace na nastavajici utvar
		best_heuristic = BLOCK_RIGHT; 	// nasledne opatreni proti vygenerovani rodice
		(this + 1)->whitespace_pos = whitespace_pos - 1; // vyhodnoceni pozice nuly
	}else if (  best_heuristic == BLOCK_RIGHT) {
		SWAP ( 1 );
		best_heuristic = BLOCK_LEFT;
		(this + 1)->whitespace_pos = whitespace_pos + 1;
	}else if ( best_heuristic == BLOCK_UP ) { 
		SWAP ( -size );
		best_heuristic = BLOCK_DOWN;
		(this + 1)->whitespace_pos = whitespace_pos - size;
	}else{ // block down
		SWAP ( size );
		best_heuristic = BLOCK_UP;
		(this + 1)->whitespace_pos = whitespace_pos + size;
	}

	// setting up flags
	(this+1)->flags = check_boarders( (this + 1)->usporadani , size , (this+1)->whitespace_pos );
	(this+1)->flags |= best_heuristic ; // ! modifikovana pred chvili
	return false;
}





// ***********************************************************************
// ***********************************************************************
//

// vraci index nuly v poly 
inline static int vyhledej_nulu ( const unsigned char * data )
{
	assert ( data );
	return (unsigned char*)memchr( data , 0 , 25 ) - data;
}

// preda to co by melo byt ve flags
// viz object.h
inline static unsigned char check_boarders ( const unsigned char * data , int size , unsigned char  index_nuly )
{
	assert ( data );
	assert ( index_nuly < size*size );
	assert ( data[index_nuly] == 0 );

	unsigned char flags = 0;

	if ( index_nuly%size == 0 	 ) flags |= BLOCK_LEFT;
	if ( index_nuly%size == size - 1 ) flags |= BLOCK_RIGHT;
	if ( index_nuly/size == 0 	 ) flags |= BLOCK_UP;
	if ( index_nuly/size == size - 1 ) flags |= BLOCK_DOWN;

	return flags;
}

static inline unsigned int heuristika_na_miru ( const unsigned char * data , unsigned int size )
{
	assert ( size >= 3 && size <= 5 );

	if ( size == 4 )
		return heuristic_4x4 ( data );
	if ( size <  4 )
		return heuristic_3x3 ( data );
	if ( size >  4 )
		return heuristic_5x5 ( data );
}

// heuristika pro 3x3 ostatni budou mit jine	
static unsigned int heuristic_3x3 ( const unsigned char * data )
{
	unsigned int value = 0; // heuristic
	int temp;

	for ( int pozice = 0 ; pozice < 3 * 3 ; ++pozice )
	{
		// jestli to sedi
		if ( pozice + 1 == data[pozice] ) continue;
		else if ( data[pozice] == 0 )
		{
			value += -(pozice)%3 +2; // horizontalni
			value += -(pozice)/3 +2; // vertikalni
			continue;
		}
		
		// to kam to chce byt polozeno  - to kde to je
		temp = (data[pozice] - 1)%3 - pozice%3;

		if ( temp < 0 ) // abs
			value += -temp;
		else
			value += temp;
		
		temp = (data[pozice] - 1)/3 - pozice/3;

		if ( temp < 0 ) // abs
			value += -temp;
		else
			value += temp;
	}
	return value;
}		

// heuristika vel 4	
static unsigned int heuristic_4x4 ( const unsigned char * data )
{
	unsigned int value = 0;
	int temp;
	
	for ( int pozice = 0 ; pozice < 4*4 ; ++pozice )
	{
		if ( data[pozice] == 0 ) continue;
		else if ( (data[pozice] - 1)/4 == 0 || // je to v hornim levem rohu ?
			  (data[pozice] - 1)%4 == 0  )
		{ // 1 - 4

			// to kam to chce byt polozeno  - to kde to je
			temp = (data[pozice] - 1)%4 - pozice%4;

			if ( temp < 0 ) // abs
				value += -temp;
			else
				value += temp;
			
			temp = (data[pozice] - 1)/4 - pozice/4;

			if ( temp < 0 ) // abs
				value += -temp;
			else
				value += temp;
		}	
	}

	return value;
}	


// heuristika vel 5
static unsigned int heuristic_5x5 ( const unsigned char * data )
{
	unsigned int value = 0;
	int temp;
	
	for ( int pozice = 0 ; pozice < 5*5 ; ++pozice )
	{
		if ( data[pozice] == 0 ) continue;
		else if ( (data[pozice] - 1)/5 == 0 || // je to v hornim levem rohu ?
			  (data[pozice] - 1)%5 == 0  )
		{ // 1 - 4

			// to kam to chce byt polozeno  - to kde to je
			temp = (data[pozice] - 1)%5 - pozice%5;

			if ( temp < 0 ) // abs
				value += -temp;
			else
				value += temp;
			
			temp = (data[pozice] - 1)/5 - pozice/5;

			if ( temp < 0 ) // abs
				value += -temp;
			else
				value += temp;
		}	
	}

	return value;
}	


// preda na jaky index se ma kliknout =>
// jaky index se oproti dalsimu ( predanemu ) usporadani
// usporadani zmenil ( krome nuly )
int OBJECT::get_change ( const OBJECT & obj ) const
{
	for ( int pos = 0; pos < (int)(size*size) ; ++pos )
		if ( usporadani[pos] != 0 )
			if ( usporadani[pos] != obj.usporadani[pos] )
				return pos;
	assert ( !"nenasla se zmena" );
	return -1;
}

// prevede z matice 5x5 na 4x4 a z 4x4 na 3x3
// tak aby vzdy bylo v usporadani od nuly do (size*size - 1)
unsigned char * OBJECT::convert ( )
{
	unsigned int return_data_pos = 0;
	for ( unsigned int x = 0; x < size*size ; ++x )
	{
		if ( x % size == 0 ) continue;
		else if ( x/size == 0 ) continue;

		assert ( return_data_pos < x );
		if ( usporadani[x] != 0 )
			usporadani[return_data_pos] = usporadani[x] - size - (usporadani[x]-1)/size;
		else
			usporadani[return_data_pos] = usporadani[x]; // zadna konverze
		++return_data_pos;
	}
	--size;
	return usporadani;
}


// prevede na vetsi matici
void OBJECT::convert_up ( unsigned size_to_conv )
{
	if ( size == size_to_conv )
		return;

	size += 1;
	unsigned char * old_data = new unsigned char[ size * size ];
	for ( unsigned x = 0 ; x < (unsigned)(size-1)*(size-1) ; ++x )
		old_data[x] = usporadani[x];

	unsigned int pozice_old_data = 0;

	for ( unsigned x = 0 ; x < size*size ; ++x )
	{
		if ( x % size == 0 || x/size == 0 )
		{ usporadani[x] = x + 1; continue; }
		
		if ( old_data[pozice_old_data] != 0 )
		usporadani[x] = old_data[pozice_old_data] + size   +
				(old_data[pozice_old_data] + size - 2)/(size-1);
		else
		usporadani[x] = 0;
		++pozice_old_data;
	}

	if ( size != size_to_conv )
		convert_up( size_to_conv );
	delete old_data;
}

#ifndef NDEBUG
#define PRINT( X ) std::cout << #X "\t" << (int)X << std::endl;

void OBJECT::print ()
{
	using namespace std;

	PRINT( size );
	PRINT( flags );
	PRINT( heuristic );
	
	for ( unsigned int x = 0 ; x < size*size ; ++x )
	{
		cout << (int)usporadani[x] << " ";
		if ( usporadani[x] < 10 )
			cout << " ";

		if ( x % size == (unsigned)size - 1 )
			cout << endl;
	}

}

#endif









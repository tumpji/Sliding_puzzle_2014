#include <cassert>
/***************************************************************************
*      OBJECT 
*      OBJECT() - nepouziva se temer , objekt po tomto je petencielne nebezpecny
*      OBJECT( OBJECT& ) -  kopirovaci konstruktor
*      OBJECT( const char *, int size )
*              - prevezme aktualni rozpolozeni na hraci plose 
*              - 0 oznacuje prazdne misto 
*              - ostatni jsou policka 1 - (size*size - 1)
*              - size je rozmer hraciho pole
*      ~OBJECT() - destruktor
*      operator< - radi podle rozpolozeni na hraci plose 
*      generate_best_children ();
*              - zjisti ktere z deti, ktere muze vygenerovat jsou nejlepsi
*              - toto vytvori na (this+1)
*              - inicializuje toto dite
*      get_change();	- diva se na nasledujiciho v poradi a zjisti 
*      			- index zmeny ( na jake policko se kliklo )
*
* private:
*      size    - velikost 3x3 , 4x4 , 5x5 - { 3, 4, 5 }
*              - jine hodnoty nepripadaji v uvahu
*
*      usporadani      - rozpolozeni na hraci desce 
*                      - stejny format jako u konstruktoru 
*
*      flags   - ukazuje, ktere dite nema smysl vygenerovat
*              - a) uz je vygeneroval
*              - b) vygeneroval by rodice
*              - 1 oznacuje co nelze vygenerovat
*              - 0x01 - nelze vlevo
*              - 0x02 - nelze vpravo
*              - 0x04 - nelze nahoru
*              - 0x08 - nelze dolu
*              - nelze nahoru => volne pole nesmi nahoru
*
*      heuristic       - ukazuje jak moc je rozeskladana hraci plocha
*                      - 0 oznacuje ze je hotovo v ramci dane velikosti        
*                      
*/

#ifndef OBJECT_HEADER_DEFINED
#define OBJECT_HEADER_DEFINED
class OBJECT {
public:
        OBJECT ();
        OBJECT ( const OBJECT & ); // inline
        OBJECT ( const unsigned char * , int size );
        ~OBJECT ();

        bool operator< ( const OBJECT & ) const; // inline
        bool generate_best_children ();

        unsigned get_heuristic () const; // inline

	int get_change ( const OBJECT & ) const; 
	unsigned char * convert ();
	void convert_up ( unsigned );

// debug
#ifndef NDEBUG
	void print ();
#else
	inline void print () {};
#endif

private:
        unsigned size;
        unsigned char usporadani [ 25 ]; // nejvice bude 5*5 vic ne
        unsigned char flags;
        unsigned heuristic;
};


inline OBJECT::OBJECT ( const OBJECT & vzor ) : size(vzor.size), heuristic(vzor.heuristic)
{
        for ( int x = 0; x < 25; ++x )
                usporadani[x] = vzor.usporadani[x];
        flags = vzor.flags;
}

inline OBJECT::OBJECT () : size(0)
{}

inline OBJECT::~OBJECT ( )
{} 

// ********************************************************************

inline unsigned OBJECT::get_heuristic () const
{
	return heuristic;
}

inline bool OBJECT::operator< ( const OBJECT& vzor ) const 
{
	assert ( size >= 3 && size <= 5 );
	assert ( sizeof(int) == 4 );
	
	if (  *(int*)usporadani < *(int*)vzor.usporadani )
		return true;
	else if (  *(int*)usporadani > *(int*)vzor.usporadani )
		return false;

// else
//  == 
//
		
	if (  *((int*)usporadani + 1 ) < *((int*)vzor.usporadani + 1) )
		return true;
	else if (  *((int*)usporadani + 1 ) > *((int*)vzor.usporadani + 1) )
		return false;

	
	if ( size == 3 )
	{
		for ( int x = size*size - 1; x >= 8; --x )
		{
			if ( usporadani[x] < vzor.usporadani[x] )
				return true;
			else if ( usporadani[x] == vzor.usporadani[x] )
				continue;
			else // >
				return false;
		}
		return false;
	}
	
	if (  *((int*)usporadani + 2 ) < *((int*)vzor.usporadani + 2) )
		return true;
	else if (  *((int*)usporadani + 2 ) > *((int*)vzor.usporadani + 2) )
		return false;

	if (  *((int*)usporadani + 3 ) < *((int*)vzor.usporadani + 3) )
		return true;
	else if (  *((int*)usporadani + 3 ) > *((int*)vzor.usporadani + 3) )
		return false;

	if ( size == 4 ) return false;

	if (  *((int*)usporadani + 4 ) < *((int*)vzor.usporadani + 4) )
		return true;
	else if (  *((int*)usporadani + 4 ) > *((int*)vzor.usporadani + 4) )
		return false;

	if (  *((int*)usporadani + 5 ) < *((int*)vzor.usporadani + 5) )
		return true;
	else if (  *((int*)usporadani + 5 ) > *((int*)vzor.usporadani + 5) )
		return false;

	if ( usporadani[24] < vzor.usporadani[24] )
		return true;
	else 
		return false;

	
}


#endif



















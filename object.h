#include <cassert>
#include <cstring>

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
        OBJECT (); // inline
        OBJECT ( const OBJECT & ); // inline
        OBJECT ( const unsigned char * , unsigned short size ); 

        ~OBJECT ();

        bool operator< ( const OBJECT & ) const; // inline
        bool generate_best_children (); // nepouziva konstruktor

        unsigned get_heuristic () const; // inline

	int get_change ( const OBJECT & ) const; 
	unsigned char * convert ();
	void convert_up ( unsigned );
	inline unsigned char * get_usporadani () { return usporadani; }

// debug
#ifndef NDEBUG
	void print ();
#else
	inline void print () {};
#endif

private:
/*        unsigned char size; // velikost 3 - 5
	unsigned char whitespace_pos; // pozice mezery
        unsigned char usporadani [ 25 ]; // nejvice bude 5*5 vic ne
        unsigned char flags;
        unsigned heuristic; */
        unsigned size; // velikost 3 - 5
	unsigned whitespace_pos; // pozice mezery
        unsigned heuristic; 
        unsigned flags;
        unsigned char usporadani [ 25 ]; // nejvice bude 5*5 vic ne
};

// konstruktory / destruktory obj. 
//
//

// zakladni
inline OBJECT::OBJECT ()
{}

// kopirovaci
inline OBJECT::OBJECT ( const OBJECT & vzor )// : size(vzor.size)
{
	memcpy ( (void*)this , (void*)&vzor , sizeof(OBJECT) );
        //flags = vzor.flags; // asi nema
	//memcpy ( (void*)usporadani , (void*)vzor.usporadani , size*size );
}

// destruktor
inline OBJECT::~OBJECT ( )
{} 

// ********************************************************************

inline unsigned OBJECT::get_heuristic () const
{
	return heuristic;
}

inline bool OBJECT::operator< ( const OBJECT& vzor ) const 
{
	return ( 0 > memcmp( (void*)usporadani , (void*)vzor.usporadani , size*size ) );
}


#endif



















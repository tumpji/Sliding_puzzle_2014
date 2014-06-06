
namespace CONST 
{
	const int radku = 4;
	const int sloupcu = 4;
	const int policek = radku*sloupcu;
	const int white_space_value = CONST::policek;

	const char spravne_usporadani [CONST::policek] = // cilovy stav policek
	{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 };

	enum { VLEVO = 0x10 , VPRAVO = 0x20 , NAHORU = 0x40, DOLU = 0x80 };

	namespace MOVES
	{
		enum {  VLEVO = -1 , VPRAVO = 1 , NAHORU = -CONST::sloupcu , DOLU = CONST::sloupcu };
	}
}

class OBJECT 
{
public:
	OBJECT ();
	// 1-x kde x je CONST::policek , dale x predstavuje mezeru
	OBJECT ( const char [CONST::policek] ); 	// interface 
	OBJECT ( const OBJECT& ); 			// kopirovaci
	~OBJECT (); 					// destruktor

	// generator uzlu 
	// vsechny ohodnoti a seradi podle ohodnoceni takze [0].value <<
	//int generate_children_all (OBJECT [4]);	
	// generuje s nejmensim ohodnocenim jinak vraci NULL
	//OBJECT * generate_best_children (); 

	// na nasledujici policko v array vygeneruje nejvhodnejsiho potomka
	// jestli to nelze vrati TRUE
	bool generate_best_children_next_to ();

	// vrati true jestli je to serazeno podle CONST::spravne_usporadani
	bool is_sorted ();
	int get_heuristic ();
	
	void print_obj ();				// debug fce na print
	// format pro 3x3 "(1,2,3,4,5,6,7,8,9)" - 9 je volne misto
	void print_obj_parser ();			// tisk stavu pro testovani
private:
	// optimalizovana varianta pouziva fce generate_best_children_next_to()
	OBJECT ( const OBJECT&, int , short , int);
	// ohodnoti podle heuristicke funkce, ktera je v nem impl.
	void evaluate (); 
	// nastavi children dolni bity - zjisti jestli je okolo misto na posun
	void inspect_near_elements ();


	// aktualni stav vsechn elementu 	
	char usporadani [ CONST::policek ];	
	// hodnota heuristiky mene = lepe
	int value; 			

	// 0x01 nelze pohnout doleva 
	// 0x02 nelze pohnout vpravo
	// 0x04 nelze pohnout nahoru
	// 0x08 nelze pohnout dolu // dale 0x80 - generovany dolu
	char children; 				
	int get_hint ( const OBJECT&, const int& , short& );
};

/********************************************************************************
 * 			INLINE FCE 
********************************************************************************/


inline bool OBJECT::is_sorted ()
{
	for ( int x = 0 ; x < CONST::policek ; ++x )
		if ( usporadani[x] != CONST::spravne_usporadani[x] )
			return false;
	return true;
}

inline int OBJECT::get_heuristic ()
{
	return value;
}

// prevede pole typu char na objekt OBJECT
// dale inicializuje promennou value & children
inline OBJECT::OBJECT ( const char predana [ CONST::policek ] )
{
        for ( int x = 0; x < CONST::policek ; ++x )
                usporadani[x] = predana[x]; // kopirovani

        evaluate (); // doplni value

        inspect_near_elements (); // doplnuje prom. children 
}

// kopirovaci konstruktor
inline OBJECT::OBJECT ( const OBJECT & objekt )
{
	for ( int x = 0 ; x < CONST::policek ; ++x ) // kopiruje usporadani
		usporadani[x] = objekt.usporadani[x];

	value = objekt.value;           // kopiruje ohodnoceni
	children = objekt.children;     // kopiruje rozmnozovani
}


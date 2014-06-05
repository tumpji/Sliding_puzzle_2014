
namespace CONST 
{
	const int radku = 4;
	const int sloupcu = 4;
	const int policek = radku*sloupcu;
	const int white_space_value = CONST::policek;

	const char spravne_usporadani [CONST::policek] = // cilovy stav policek
	{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };

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
	OBJECT ( const OBJECT & ); 			// kopirovaci
	~OBJECT (); 					// destruktor

	// generator uzlu 
	// vsechny ohodnoti a seradi podle ohodnoceni takze [0].value <<
	int generate_children_all (OBJECT [4]);	
	// generuje s nejmensim ohodnocenim jinak vraci NULL
	OBJECT * generate_best_children (); 

	// vrati true jestli je to serazeno podle CONST::spravne_usporadani
	bool is_sorted ();
	


	void print_obj ();				// debug fce na print

	// format pro 3x3 "(1,2,3,4,5,6,7,8,9)" - 9 je volne misto
	void print_obj_parser ();			// tisk stavu pro testovani
private:
	// ohodnoti podle heuristicke funkce, ktera je v nem impl.
	void evaluate (); 

	// nastavi children dolni bity - zjisti jestli je okolo misto na posun
	void inspect_near_elements ();


	// aktualni stav vsechn elementu 	
	char usporadani [ CONST::policek ];	
	// hodnota heuristiky
	int value; 			

	// 0x01 nelze pohnout doleva 
	// 0x02 nelze pohnout vpravo
	// 0x04 nelze pohnout nahoru
	// 0x08 nelze pohnout dolu // dale 0x80 - generovany dolu
	char children; 				
};



namespace CONST 
{
	const int radku = 4;
	const int sloupcu = 4;
	const int policek = radku*sloupcu;

	const char spravne_usporadani [CONST::policek] = // cilovy stav policek
	{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
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
	// generuje s nejmensim ohodnocenim	
	OBJECT * generate_best_children (); 


	void print_obj ();				// debug fce na print
private:
	// ohodnoti podle heuristicke funkce, ktera je v nem impl.
	void evaluate (); 

	// nastavi children dolni bity - zjisti jestli je okolo misto na posun
	void inspect_near_elements ();

	// najde prazdne misto a vrati jeho index
	int get_whitespace_pos ();



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


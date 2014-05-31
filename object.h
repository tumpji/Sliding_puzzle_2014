
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
	// 1-x kde x je CONST::policek , dale x predstavuje mezeru
	OBJECT ( const char [CONST::policek] ); 	// interface 
	OBJECT ( const OBJECT & ); 			// kopirovaci
	~OBJECT (); 					// destruktor

	// generator uzlu 
	// vsechny ohodnoti a seradi podle ohodnoceni
	// pokud nelze vytvorit vsechny 4 generuje NULL uk.
	OBJECT *(*generate_children () )[4];		


	void print_obj ();				// debug fce na print
private:
	// vraci hodnotu ktera signalizuje kolik se toho ma generovat a ceho
	// 0x1 - v levo neni misto
	// 0x2 - v pravo neni misto
	// 0x4 - nahore neni misto
	// 0x8 - dole neni misto
	char evaluate (); // ohodnoti podle heuristicke funkce, ktera je v nem impl.

	char usporadani [ CONST::policek ]; 	// aktualni stav policek
	int value; 		// hodnota heuristiky
	bool children; 		// uz se to rozmnozovalo ? :D
};


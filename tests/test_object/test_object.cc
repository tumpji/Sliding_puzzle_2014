#include <string>
#include <cctype>
#include <iostream>
#include "../../object.h"

using namespace std;

void test_num1 ( char [CONST::policek] );

void parser ( void )
{
	string radek;
	char predane [ CONST::policek ];
	
	while ( true )
	{
		if ( std::cin.eof( ) )
			break;
		getline ( std::cin , radek  );
		
		if ( radek == "getbest" )
			continue;
		else if ( radek == "end" )
			break;
		else if ( radek.size() == 0 )
			continue;
	
		int pozice_predane = 0;	
		int pos = 0;
		predane[0] = 0;

		for ( ; pos < radek.length() ; ++pos ) // iteruje prez radek
		{
			if ( isdigit(radek[pos] ) )
			{
				predane[pozice_predane] *= 10;
				predane[pozice_predane] += radek[pos] - '0';
			}
			else if ( radek[pos] == ',' )
			{
				pozice_predane += 1;
				if ( pozice_predane != CONST::policek )
					predane[ pozice_predane ] = 0;
			}
		}

		if ( pozice_predane != 16 ) {
			cerr << "Nespravny format " << endl;
			cerr << "Predano: " << radek << endl;
			cout << endl;
			continue;
		}
		
		test_num1 ( predane );
	}

}

void test_num1 ( char hodnoty[CONST::policek] )
{
	OBJECT objekt ( hodnoty );
	OBJECT * vysledek = objekt.generate_best_children ();
	vysledek->print_obj_parser ();
}

int main ( void ) 
{
	parser ( );
	return 0;
}

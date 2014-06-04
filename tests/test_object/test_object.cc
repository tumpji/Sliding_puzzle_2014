#include <string>
#include <cctype>
#include <iostream>
#include "../../object.h"

using namespace std;
void test_num1 ( const char [CONST::policek] ); // get best test
void test_num2 ( const char [CONST::policek] ); // get all test 
void test_num3 ( const char [CONST::policek] ); // test iteracni best test

void (*test_array[3])( const char [CONST::policek] ) = { test_num1 , test_num2 , test_num3 };

void parser ( void )
{
	string radek;
	int test_number = 0;
	char predane [ CONST::policek ];
	
	while ( true )
	{
		if ( std::cin.eof( ) )
			break;
		getline ( std::cin , radek  );
		
		if ( radek == "getbest" )
		{
			test_number = 0;
			continue;
		}
		else if ( radek == "getall" )
		{
			test_number = 1;
			continue;
		}
		else if ( radek == "iterative_check" )
		{
			test_number = 2;
			continue;
		}
		else if ( radek.size() == 0 )
			continue;

// program se spousti zadanim vstupnich cisel	
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
		
		test_array[test_number] ( predane );
	}

}

void test_num1 ( const char hodnoty[CONST::policek] )
{
	OBJECT objekt ( hodnoty );
	OBJECT * vysledek = objekt.generate_best_children ();
	vysledek->print_obj_parser ();
	cout << endl;
}

// test pro vsechny varianty
void test_num2 ( const char hodnoty[CONST::policek] )
{
	OBJECT zasobnik [4];
	OBJECT objekt ( hodnoty );

	int maximum = objekt.generate_children_all ( zasobnik );
	for ( int x = 0; x < maximum ; ++x )
		zasobnik[x].print_obj_parser ();
	cout << endl;
}

// testovani by melo byt shodne s testovanim get all
void test_num3 ( const char hodnoty[CONST::policek] )
{
	OBJECT objekt ( hodnoty );

	OBJECT * vysledek = objekt.generate_best_children ();

	do {
		vysledek->print_obj_parser ();
		vysledek = objekt.generate_best_children ();
	}while ( vysledek != NULL );
	
	cout << endl;
}

int main ( void ) 
{
	parser ( );
	return 0;
}

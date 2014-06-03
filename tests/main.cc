#include <string>
#include <iostream>

using namespace std;

int main ( void ) 
{

	string x;
	cout << "Testovani programu" << endl;

	while ( 1 )
	{

		getline ( std::cin , x );
		cout << x << std::endl;
		if ( x == "ahoj" )
			break;
	}


	return 0;
}

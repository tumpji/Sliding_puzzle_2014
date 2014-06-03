#include <string>
#include <iostream>

using namespace std;

int main ( void ) 
{

	string x;

	getline ( std::cin , x );
	cout << x << std::endl;
	getline ( std::cin , x );
	cout << x << std::endl;

	return 0;
}

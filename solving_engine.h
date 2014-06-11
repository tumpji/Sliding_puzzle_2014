#include "object.h"

class Engine
{
public:
	Engine ();
	~Engine ();

	int set_up_and_run ( const char [CONST::policek] );

private:	
	int predej_vysledek (  OBJECT * ,  int& );

	// zde se ukladaji vsechny objekty
	OBJECT * array_obj;	
};

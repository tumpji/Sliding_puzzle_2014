#include <vector>
#include "object.h"

class Engine
{
public:
	Engine ( unsigned );
	~Engine ();

	//int set_up_and_run ( const char [CONST::policek] );

	std::vector<std::pair<int,OBJECT>> run ( const unsigned char * ); 
	std::vector<OBJECT>* run_ida ( const unsigned char * , unsigned , unsigned);
private:	


	// zde se ukladaji vsechny objekty
	OBJECT * array_obj;	
	unsigned int size;

	//OBJECT * aloc_mem;
};

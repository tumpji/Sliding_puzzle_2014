#include <set>
#include "object.h"

class ENGINE
{
public:
	ENGINE ();
	~ENGINE (); 
	
	void start ( const char [DIAMETER] );
private: 
	std::set<OBJECT_DISTANCE> mapa_heur;	
	//map<OBJECT_DISTANCE> mapa_heur;	
	unsigned long nodes;
};

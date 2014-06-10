#include <X11/Xlib.h>
#include <vector>

class COORDINATES
{
public:
	COORDINATES ();
 	~COORDINATES ();
	
	int x;
	int y;
};

class Comunication
{
public:
	// postupna komunikace s uzivatelem
	Comunication ();
	~Comunication ();

	// preda aktualni usporadani vyctene na obrazovce 
	const char * preber_usporadani ();
	
	// vypocita kam ma kliknout a klikne na policko
	void click_on_area ( int index );
private:
	// pohne s ukazatelem a klikne
	void click_move ( int x , int y );

	// od uzivatele prevezme pribliznou polohu
	void get_approximately_area (); 
	// od uzivatele prebere posladany obrazek
	void get_user_indexes ();
	// vypocet ohraniceni
	void get_accurately_area ();
	// vezme obrazek zkoumane oblasti a rozcleni ho na kousky
	std::vector<unsigned long*> get_slices_screen ( );
	// vytiskne zkoumanou oblast a ulozi ji do dynamicky all. prom.
	unsigned long * get_print_screen ();

	Display * display;
	int screen;
	Window root;
	Window child;

	// [0] left top , [1] down bottom , game_area
	COORDINATES active_area[2];
	std::vector<unsigned long *> obrazy_poli;
};


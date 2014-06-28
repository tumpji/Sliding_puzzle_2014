#include <X11/Xlib.h>
#include <vector>

class Game_area 
{
public:
	Game_area ();
	~Game_area ();
	void init ();

	// vrati velikost jednoho policka bez obraniceni
	unsigned get_policko_size () const; 

	// vrati velikost plochy 
	unsigned get_area_size () const;

	// vrati kde se ma kliknout na policko
	// vraci std::pair -> first = x ,, second = y
	std::pair<unsigned,unsigned> get_policko ( int index ) const;

	// vrati vyrez z obrazovky ( ctverec , bitmapa ) dle x, y
	unsigned long * get_print_screen () const;
private:	

	// prevezme od uzivatele pribliznou polohu hraci plochy
	void get_approximately_area ();

	// vypocita presne hraci plochu a vlozi ji za x,y
	void compute_area ();

public:
	unsigned x [2];
	unsigned y [2];
	unsigned poskladej[2]; // tlacitko poskladej
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

	// klikne na tlacitko poskladej puzzle
	void poskladej_puzzle_click ();

private:
	// od uzivatele prebere posladany obrazek
	void get_user_indexes ();

	// vezme obrazek zkoumane oblasti a rozcleni ho na kousky
	 void get_slices_screen ( std::vector<unsigned long*>& );

	// vytiskne zkoumanou oblast a ulozi ji do dynamicky all. prom.

	//Display * display;
	//int screen;
	//Window root;
	//Window child;
	//int x_poskladej, y_poskladej;
	// [0] left top , [1] down bottom , game_area
	//COORDINATES active_area[2];

	Game_area game_area;
	std::vector<unsigned long *> obrazy_poli;
};


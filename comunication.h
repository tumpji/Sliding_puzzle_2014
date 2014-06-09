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
	// konstruktor preda se udaje jako je pozice od/do
	Comunication ();

	void screen_capture ();
	void get_cursor ();
	

private:
	void get_approximately_area ();
	void get_accurately_area ();
	unsigned long * get_print_screen ();
	void get_user_indexes ();

	Display * display;
	int screen;
	Window root;
	Window child;

	// [0] left top , [1] down bottom , game_area
	COORDINATES active_area[2];

	std::vector<unsigned long *> obrazy_poli;
};


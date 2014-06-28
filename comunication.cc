#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cmath>
#include <unistd.h> // sleep
#include <X11/Xutil.h>
#include <cstring>

#include "comunication.h"

#define PRINT(x) std::cout << #x  " " << x << std::endl

// 4x4
#define ROZMER 4 

#define ZPRAVA1 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad levim hornim rohem hraciho pole |^^ \n"\
		"Budete mit 4s na splneni" 

#define ZPRAVA2 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad pravym dolnim rohem hraciho pole _|\n"\
		"Budete mit 4s na splneni"

#define ZPRAVA3 "Potrebuji poskladany obrazek...\n"\
		"Odjedte z hraci plochy mysi pryc !!\n"\
		"Potom stisknete enter a odstrante z hraci plochy kurzor"
// debug fce 
// uklada do formatu ppm3 obrazek unsigned long zacinajici ukazatelem data
// uklada pouze ctverce rozmer - row;
// uklada soubory pojmenovane < file_num -> string >.ppm do aktualniho adresare
static void save_to_ppm3 ( unsigned long * data , int row , unsigned file_num );

// ceka na stisknuti klavesy nasledne postupne vypisuje odpocitavani
static void counting ();
// preda souradnice (root) mysi 
static std::pair<unsigned, unsigned> get_pos_pointer ();
// klikne na pozici 
static void click_coordinates ( );
static void move_cursor_to_coordinates ( unsigned , unsigned  );

// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************

template< class T >
static inline T return_bigger ( T a , T b )
{
	return ( a > b )?( a ):( b );
}

Game_area::Game_area ( )
{
	x[0] = 0; x[1] = 0;
	y[0] = 0; y[1] = 0;
}

Game_area::~Game_area ()
{ }

void Game_area::init ()
{
	get_approximately_area ();
	compute_area ();
}

// vraci velikost policka na hraci plose bez boarderu ( -2 )
// je ctvercove
unsigned Game_area::get_policko_size () const
{
	return ( x[1] - x[0] ) / ROZMER - 2;
}

// jeden rozmer hraci plochy ( je ctvercova )
unsigned Game_area::get_area_size () const
{
	return x[1] - x[0];
}

// vrati presne souradnice do kterych se ma kliknout 
// tyto souradnice jsou nahodne asak odpovidaji policku na hraci plose
// vraci std::pair firs = souradnice x
std::pair<unsigned , unsigned > Game_area::get_policko ( int index ) const
{
	std::pair< unsigned , unsigned > ret_val;

	// pozice horniho rohu policka ( boarder == 1  + neco navic )
	ret_val.first = get_area_size()/ROZMER * ( index%ROZMER ) + x[0] + 5;
	ret_val.second= get_area_size()/ROZMER * ( index/ROZMER ) + y[0] + 5;

	unsigned nejvice_pridat = get_policko_size() - 5;

	
	ret_val.first 	+= nejvice_pridat/2;
	ret_val.second 	+= nejvice_pridat/2;
	// pridat nahodne
	//ret_val.first 	+= rand()%( nejvice_pridat );
	//ret_val.second	+= rand()%( nejvice_pridat );

	return ret_val;
}


// komunikuje s Xorg a s uzivatelem
// prebira pracovni plochu
void Game_area::get_approximately_area () 
{
	std::pair<unsigned , unsigned > temp;
	// doplni abs. adresu na zaklade child window
	std::cout << ZPRAVA1 << std::endl;    counting ();
	temp = get_pos_pointer ();	x[0] = temp.first;	y[0] = temp.second;

	// doplni druhou soradnici
	std::cout << ZPRAVA2 << std::endl;    counting ();
	temp = get_pos_pointer ();	x[1] = temp.first; 	y[1] = temp.second;

	// doplni polohu tlacitka "poskladej puzzle "
	std::cout << "Podrz nad poskladej puzzle !!! " << std::endl; counting ();
	temp = get_pos_pointer ();	poskladej[0] = temp.first;	poskladej[1] = temp.second;
	
	unsigned size = return_bigger(  (x[1] -x[0]) , (y[1] -y[0])  );
	size += 6; // extra space
	x[0] -= 3; 	x[1] = x[0] + size;
	y[0] -= 3; 	y[1] = y[0] + size; // square
	
	// bez kontroly

#ifndef NDEBUG	
	std::cout 	<< "get_approximately_area : " << std::endl;
	PRINT( x[0] );
	PRINT( x[1] );
	PRINT( y[0] );
	PRINT( y[1] );
	PRINT( x[1] - x[0] );
#endif
}

// ma za ukol zjistit presne velikosti policek 
// na zaklade boarderu ktery se vyskytuje na hornim okraji pole
void Game_area::compute_area ( )
{
	unsigned long * first_picture;

	std::cout << ZPRAVA3 << std::endl; counting();
	first_picture = get_print_screen ( ); // first screen shot

	unsigned rozmery_plochy = get_area_size ();

	unsigned long last_pix = 0;
	int in_row = 1;
	bool done = false;
	// zkoumani usecek horizontalnich - boarders
	for ( unsigned yp = 0 ; yp < rozmery_plochy && !done; ++yp ) {
	for ( unsigned xp = 0 ; xp < rozmery_plochy ; ++xp )
		if ( last_pix == first_picture[ xp + yp*rozmery_plochy ] )
			++in_row;
		else
		{
			if ( (float)in_row >= ((float)rozmery_plochy)*3.f/4.f ) // je to vetsi ==>boarder
			{ // boarder
				x[1] -= rozmery_plochy - xp + 1;
				x[0] += xp - in_row + 1; // vc. b. 
				y[0] += yp + 1; // vc. b.
				y[1] -= ( y[1] - y[0] )  - ( x[1] - x[0] ); 
				done = true;
				break;
			}

			in_row = 1;
			last_pix = first_picture[ xp + yp* rozmery_plochy ];
		}
	
		in_row = 0;
	} // first for 

	free( first_picture ); // uprava hranic hohova - uvolni se obrazek 

#ifndef NDEBUG	
	std::cout 	<< "compute_area : " << std::endl;
	PRINT( x[0] );
	PRINT( x[1] );
	PRINT( y[0] );
	PRINT( y[1] );
	PRINT( x[1] - x[0] );
#endif
}

// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************


Comunication::Comunication ()
{
	game_area.init();
	get_user_indexes (); // vyuzije nastaveni game_area k extrakci obrazku
}

Comunication::~Comunication ()
{
	for ( unsigned long * mem : obrazy_poli )
	{ assert ( mem ); free(mem ); }
	obrazy_poli.clear(); 
}

void Comunication::poskladej_puzzle_click ()
{
	move_cursor_to_coordinates ( game_area.poskladej[0] , game_area.poskladej[1] );
	click_coordinates ();
}

// funkce ma za ukol vzit obrazky jiz serazeneho hraciho pole a 
// zaradit je do vektoru obrazy_poli - tj vektor pro ukladani referencnich policek
void Comunication::get_user_indexes ()
{
	unsigned long * volne_policko;

	obrazy_poli.clear(); // cistka
	obrazy_poli.reserve( ROZMER * ROZMER ); // rezervovani mista

	get_slices_screen ( obrazy_poli ); // vkladani elementu

	volne_policko = obrazy_poli.back();
	obrazy_poli.pop_back(); // uvolneni posledniho elementu ( background )

	assert( volne_policko != nullptr );
	free( volne_policko );  
}

// prebere index pole na ktery ma kliknout 
void Comunication::click_on_area ( int index )
{
#ifndef NDEBUG
	std::cout << " click_on_area(" << index << ") " << std::flush;
#endif
	std::pair< unsigned, unsigned > misto = game_area.get_policko( index );
	move_cursor_to_coordinates ( misto.first , misto.second );
	click_coordinates ( );
}




// funkce ma za ukol sejmou obrazovku a rozclenit ji na dilky
// ROZMER ROZMER z techto dilku jeste odstrani 1px z kazde strany
// alokuje pamet malloc a vlozi vysledne rozcleneni do teto pameti
// vraci vektor s ukazately na tyto bitmapy
void Comunication::get_slices_screen ( std::vector<unsigned long*>& zasobnik )
{
	zasobnik.clear();
	zasobnik.reserve( ROZMER * ROZMER ); // rezervovani mista a cistka

	unsigned long * buffer = NULL;
	int size_policko = game_area.get_policko_size();
	int size = game_area.get_area_size();

	for ( int x = 0; x < ROZMER*ROZMER ; ++x ) // alokace pameti
	{
		buffer = (unsigned long *)malloc( sizeof(long)*size_policko*size_policko );
		if ( buffer == nullptr )
		{ 	
			std::cerr << "Nedostatek pameti !\n" ; 
			for ( unsigned long * t : zasobnik ) 
				{ free( t ); }  
			exit(1);
		}
		zasobnik.push_back( buffer ); // ukazatele na pamet
	}

	buffer = game_area.get_print_screen (); // vezme obrazek

	// zapisovani dat do struktury
	for ( int pozice_vstupy = 0 ; pozice_vstupy < ROZMER*ROZMER; ++pozice_vstupy )
	{
		int x_pos = pozice_vstupy%ROZMER; // rozhoduje o coll a row v 
		int y_pos = pozice_vstupy/ROZMER; // tabulce podle ROZMER
		unsigned long * output_buff = zasobnik[pozice_vstupy]; // zde se uklada
		
		for ( int y = 1; y < size/ROZMER - 1; ++y )
		for ( int x = 1; x < size/ROZMER - 1; ++x )
		{
			// od zacatku bez hranic 1px po obou stranach = -2
			assert((unsigned long) (x - 1 + ( y - 1)*( size / ROZMER - 2 )) < 
				(sizeof(long) *size_policko*size_policko) );

			assert((x_pos * size/ROZMER + y_pos * size * size / ROZMER +
					x + y * size ) < size*size );

			output_buff[ x - 1 + ( y - 1 )*size_policko ] =
				buffer[ x_pos * size/ROZMER + 
					y_pos * size * ( size / ROZMER ) +
					x + y * size ]; // pohyb ve ctverci
		}
	}
	free(buffer);
}


// tvori mezivrstvu mezi Xorg prebere obrazek vyhrazeny v active_area
// vytiskne podle toho co je ulozeno v active_area[x].{x}
unsigned long * Game_area::get_print_screen () const 
{
	// vypocet velikosti cele plochy
	int size = get_area_size()*get_area_size();
	// alokace pameti
	unsigned long* puvodni_mapa = (unsigned long*) malloc ( size * sizeof(unsigned long) ); 

	Display * display = XOpenDisplay ( 0 ); // masina

	// generovani bitmapy
	XImage * image = XGetImage ( // get print screen
				display , 
				RootWindow ( display , DefaultScreen(display) ), 
				x[0], y[0], // pocatecni bod
				get_area_size(), // 2x rozmer x,y
				get_area_size(),
				XAllPlanes(),ZPixmap);


	// kontrola platnosti
	if ( puvodni_mapa == nullptr || image == nullptr ) 
		{ std::cerr << "synchnronize_with_game error\n"; exit(1); }
	
	for ( int xp = 0 ; xp < size ; ++xp )  // copy pixels.
		puvodni_mapa[xp] = image->f.get_pixel( image, xp%get_area_size(),
							xp/get_area_size() );
	image->f.destroy_image(image); // uvolneni pameti
	XCloseDisplay( display );
	return puvodni_mapa; // vrati bitmapu dyn. al.
}


// prebere aktualni rozpolozeni policek z obrazovky
// vrati serazeny 
const char * Comunication::preber_usporadani ()
{
	assert ( obrazy_poli.size() );
	static char return_val [ ROZMER*ROZMER ];
	std::vector<unsigned long*> current; 
	unsigned int rozmer = game_area.get_policko_size ();
	unsigned long * actual = NULL; // 
	unsigned long * porovn = NULL;
	bool nula_detekovana = false;
	bool chyba_detekovana = false;	

	get_slices_screen ( current );
	
	for ( unsigned x = 0 ; x < current.size() ; ++x ) // iterate crops of screen ( now )
	{ 
		actual = current[x]; 
	
		for ( unsigned y = 0 ; y < obrazy_poli.size() ; ++y ) // iterate ( reference )
		{ 
			porovn = obrazy_poli[y];
			chyba_detekovana = true;
			for ( unsigned pixel_pos = 0; pixel_pos < rozmer * rozmer ; ++pixel_pos )
			{ // prochazi pixely
				if ( actual[pixel_pos] != porovn[pixel_pos] )
				{
					chyba_detekovana = false;
					break;
				}
			}
			if ( chyba_detekovana == true )
			{
				return_val[x] = y + 1;
				break;
			}
				
		} // prochazi org. obrazy == slozene
		if ( chyba_detekovana == false ) // nenasel se 
		{
			if ( nula_detekovana == false ) // poprve
			{
				nula_detekovana = true;
				return_val[x] = 0;
				continue;
			}
				
			for ( unsigned long * pam : current )
			{ assert( pam ); free( pam ); }
			return nullptr;
		}
	}

	for ( unsigned long * pam : current ) // uvolni pamet z prebraneho printscreanu
	{ assert( pam ); free( pam ); }
	return return_val;
}




 
// ceka na stisknuti klavesy nasledne postupne vypisuje odpocitavani
static void counting () 
{
	using namespace std;
	cin.clear();
	cin.get();
//	cin.ignore( 100 , '\n' );
	cin.clear();
}

// klikne mysi ( nenastavuje souradnice )
static void click_coordinates ( )
{
#ifndef NDEBUG
	std::pair< unsigned , unsigned > pozice = get_pos_pointer ();
	std::cout << " click("<< pozice.first << ", "<< pozice.second << ") " << std::flush;
#endif
	Display * display = XOpenDisplay ( 0 ); // masina
	Window screen = DefaultScreen ( display ); // obrazovka
	Window root = RootWindow ( display , screen ); // nahore

	XEvent event; // co se bude dit
	std::memset(&event, 0x00, sizeof(event));

	event.type = ButtonPress;
	event.xbutton.button = Button1;
	event.xbutton.same_screen = True;
	
	XQueryPointer(	display, 
			RootWindow(display, DefaultScreen(display)), 
			&event.xbutton.root, 
			&event.xbutton.window, 
			&event.xbutton.x_root, 
			&event.xbutton.y_root, 
			&event.xbutton.x, 
			&event.xbutton.y, 
			&event.xbutton.state);
	
	event.xbutton.subwindow = event.xbutton.window;
	
	while(event.xbutton.subwindow)
	{
		event.xbutton.window = event.xbutton.subwindow;
		
		XQueryPointer( 	display, event.xbutton.window, &event.xbutton.root, 
				&event.xbutton.subwindow, &event.xbutton.x_root, 
				&event.xbutton.y_root, &event.xbutton.x, 
				&event.xbutton.y, &event.xbutton.state);
	}
	
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) 
		std::cerr << "XSendEvent error\n";
	
	XFlush(display);
	
	usleep(7000);
	
	event.type = ButtonRelease;
	event.xbutton.state = 0x100;
	
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) 
		std::cerr << "XSendEvent error\n";
	
	XFlush(display);
	XCloseDisplay( display );
}

// presune mys na zadane misto
static void move_cursor_to_coordinates ( unsigned x , unsigned y )
{

#ifndef NDEBUG
	std::cout << " move to <" << x << ", " << y << "> " << std::flush;
#endif

	Display * display = XOpenDisplay ( 0 ); // masina
	Window screen = DefaultScreen ( display ); // obrazovka
	Window root = RootWindow ( display , screen ); // nahore
	
	XSelectInput ( display, root, KeyReleaseMask );
	XWarpPointer ( display, None, root , 0,0,0,0,x,y);
	XFlush( display );
	usleep(4000);
	XCloseDisplay( display );
}

// prebere adresu ukazatele 
static std::pair<unsigned, unsigned> get_pos_pointer ( )
{
	int x_root , y_root , relat_x , relat_y ;
	unsigned int mask_return;
	Display * display = XOpenDisplay ( 0 ); // masina
	Window screen = DefaultScreen ( display ); // obrazovka
	Window root = RootWindow ( display , screen ); // nahore

	if( !XQueryPointer ( display , root , &root , &root,
		&x_root , &y_root , &(relat_x), &(relat_y), &mask_return ) )
	{
		std::cerr << "XQueryPointer error\n";
		exit (1);
	}

	XCloseDisplay( display );
	return std::make_pair ( relat_x , relat_y );
}







//**********************************************************************

// debuf fce 
// prebira unsigned long * data - ukazatel na zacatek obrazku
// int row - velikost ( uklada pouze ctverce )
// file_num - identifikator - <file_num>.ppm
static void save_to_ppm3 ( unsigned long * data , int row , unsigned file_num )
{
	std::ofstream ofs;
	std::ostringstream convert; convert << file_num; // convertion to number
	convert << ".ppm";
	
	ofs.open ( convert.str() , std::ofstream::out | std::ofstream::trunc );
	int size = row*row ;
	ofs << "P3\n" << row << " " << row << "\n" << "255\n";

	for ( int x = 0 ; x < size ; ++x )
	{
		ofs 	<< (unsigned)(( data[x] >> 16 )&0xff) << " "
			<< (unsigned)((data[x] >> 8)&0xff) << " "
			<< (unsigned)(data[x]&0xff)	<< " ";
	}

	ofs.close();
}

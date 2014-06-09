#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cmath>
#include <unistd.h> // sleep
#include "comunication.h"

#define PRINT(x) std::cout << #x  " " << x << std::endl;

#define ROZMER 4 // 4x4

#define ZPRAVA1 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad levim hornim rohem hraciho pole |^^ \n"\
		"Budete mit 4s na splneni" 

#define ZPRAVA2 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad pravym dolnim rohem hraciho pole _|\n"\
		"Budete mit 4s na splneni"

#define ZPRAVA3 "Nejprve volnym polem prejedte dolni pravy roh hraci plochy\n"\
		"Potom stisknete enter a odstrante z hraci plochy kurzor"

#define ZPRAVA4 "Prejdete na hlavni obrazovku - kde jsou poskladany vsechny v poradi\n"\
		"jak spravne patri. Potom stisknete enter a odsunte mis"

// ceka na stisknuti klavesy nasledne postupne vypisuje odpocitavani
static void counting ();

COORDINATES::COORDINATES () : x(0), y(0) {}
COORDINATES::~COORDINATES () {}

Comunication::Comunication ()
{
	display = XOpenDisplay ( 0 ); // masina
	screen = DefaultScreen ( display ); // obrazovka
	root = RootWindow ( display , screen ); // nahore

	get_approximately_area ( ); // dostane od uzivatele oblast hry 
	get_accurately_area ( ); // tuhle oblast lepe analizuje
	get_user_indexes ();
}

Comunication::~Comunication ()
{
	for ( unsigned long * mem : obrazy_poli )
	{ assert ( mem ); free(mem ); }
	obrazy_poli.clear(); 
}

static void save_to_ppm3 ( unsigned long * data , int row )
{

	std::ofstream ofs;
	ofs.open ( "obr.ppm" , std::ofstream::out | std::ofstream::trunc );
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

// funkce ma za ukol vzit obrazky jiz serazeneho hraciho pole a 
// zaradit je do vektoru obrazy_poli
void Comunication::get_user_indexes ()
{
	std::cout << ZPRAVA4 << std::endl;
	unsigned long * buffer;
	int rozmer = (active_area[1].x - active_area[0].x ) / ROZMER - 2;
	
	obrazy_poli = get_slices_screen ();
	
	for ( unsigned int x = 0 ; x < obrazy_poli.size() ; ++x )
	{
		std::cin.get();
		PRINT( x );
		save_to_ppm3 ( obrazy_poli[x] , rozmer );
	}
}

// funkce ma za ukol sejmou obrazovku a rozclenit ji na dilky
// ROZMER ROZMER z techto dilku jeste odstrani 1px z kazde strany
// alokuje pamet malloc a vlozi vysledne rozcleneni do teto pameti
// vraci vektor s ukazately na tyto bitmapy
std::vector<unsigned long*> Comunication::get_slices_screen ( )
{
	std::vector< unsigned long * > zasobnik;
	unsigned long * buffer = NULL;
	int rozmer = (active_area[1].x - active_area[0].x ) / ROZMER - 2;
	int size = active_area[1].x - active_area[0].x; // radek ( bez hranic )

	for ( int x = 0; x < ROZMER*ROZMER ; ++x ) // alokace pameti
	{
		buffer = (unsigned long *)malloc( sizeof(long)*rozmer*rozmer );
		if ( buffer == nullptr )
		{ 	
			std::cerr << "Nedostatek pameti !\n" ; 
			for ( unsigned long * t : zasobnik ) 
				{ free( t ); }  
			exit(1);
		}
		zasobnik.push_back( buffer ); // ukazatele na pamet
	}

	buffer = get_print_screen ( ); // vezme obrazek

	// zapisovani dat do struktury
	
	for ( int pozice_vstupy = 0 ; pozice_vstupy < ROZMER*ROZMER; ++pozice_vstupy )
	{
		int x_pos = pozice_vstupy%ROZMER; // rozhoduje o coll a row v 
		int y_pos = pozice_vstupy/ROZMER; // tabulce podle ROZMER
		unsigned long * output_buff = zasobnik[pozice_vstupy];
		
		for ( int y = 1; y < size/ROZMER - 1; ++y )
		for ( int x = 1; x < size/ROZMER - 1; ++x )
		{
			// od zacatku bez hranic 1px po obou stranach = -2
			assert((unsigned long) (x - 1 + ( y - 1)*( size / ROZMER - 2 )) < 
				(sizeof(long) *rozmer*rozmer) );
			assert((x_pos * size/ROZMER + y_pos * size * size / ROZMER +
					x + y * size ) < size*size );

			output_buff[ x - 1 + ( y - 1)*rozmer ] =
				buffer[ x_pos * size/ROZMER + 
					y_pos * size * size / ROZMER +
					x + y * size ]; // pohyb ve ctverci
		}
	}

	free(buffer);
	return zasobnik;
}

// vytiskne podle toho co je ulozeno v active_area[x].{x}
unsigned long * Comunication::get_print_screen ()
{
	// je to ctvercova oblast ?
	assert ( (active_area[1].x - active_area[0].x) == (active_area[1].y - active_area[0].y) );
	// vypocet velikosti
	int size = ( active_area[1].x - active_area[0].x  ) * ( active_area[1].x - active_area[0].x  );
	// alokace pameti
	unsigned long* puvodni_mapa = (unsigned long*) malloc ( size * sizeof(unsigned long) ); 
	// generovani print screenu
	XImage * image = XGetImage ( display , root , // get print screen
				active_area[0].x,active_area[0].y,
				active_area[1].x - active_area[0].x,
				active_area[1].y - active_area[0].y,
				XAllPlanes(),ZPixmap);
	// kontrola platnosti
	if ( puvodni_mapa == nullptr || image == nullptr ) 
		{ std::cerr << "synchnronize_with_game error\n"; exit(1); }
	
	for ( int x = 0 ; x < size ; ++x )  // copy pixels.
		puvodni_mapa[x] = image->f.get_pixel( image, x%(active_area[1].x - active_area[0].x),
							x/(active_area[1].x - active_area[0].x) );
	image->f.destroy_image(image); // uvolneni jedne z pameti
	return puvodni_mapa;
}


const char * Comunication::preber_usporadani ()
{
	static char return_val [ ROZMER*ROZMER ];
	assert ( obrazy_poli.size() );
	std::vector<unsigned long*> current = get_slices_screen ();
	

	// referencni data jsou v obrazy_poli




	for ( unsigned long * pam : current )
	{ assert( pam ); free( pam ); }
	current.clear();

	return return_val;
}


// upravy rozmery souradnic obrazku
static bool check_and_modify ( COORDINATES* active_area )
{
	float x = active_area[1].x - active_area[0].x; // velikost obrazku hor.
	float y = active_area[1].y - active_area[0].y; // velikost obrazku vert.
	float prum = ( x + y )/2.f; // to jak velka plocha se bude zabirat
	// je to ctverec -> takze musi byt x a y stejne
	//
	std::cout << "bez bez regulace:\t";
	std::cout << "x: " << x << "\ty: " << y << "\tprum: " << prum << std::endl;
	std::cout << "souradnice: " << std::endl;
	std::cout << "<^x: " << active_area[0].x << "_|x: " << active_area[1].x << std::endl;
	std::cout << "<^y: " << active_area[0].y << "_|y: " << active_area[1].y << std::endl;
	
	if ( x <= 0.f || y <= 0.f ) // uzivatel neco pokazil
		return true;

	if ( x > y ) { // nejvetsi je v cele
		prum = ceil( ( x + 8.f )/4.f ) * 4.f; 
	} else { 
		prum = ceil( ( y + 8.f )/4.f ) * 4.f; 
	} 

	active_area[0].x -= ((int)prum - (active_area[1].x - active_area[0].x))/2;
	active_area[1].x += (int)prum - (active_area[1].x - active_area[0].x) ;
	active_area[0].y -= ((int)prum - (active_area[1].y - active_area[0].y))/2;
	active_area[1].y += (int)prum - (active_area[1].y - active_area[0].y) ;

	std::cout << std::endl << "s regulaci:\t";
	std::cout << "prum: " << prum << std::endl;
	std::cout << "<^x: " << active_area[0].x << "_|x: " << active_area[1].x << std::endl;
	std::cout << "<^y: " << active_area[0].y << "_|y: " << active_area[1].y << std::endl;
	
	return false;
}

void Comunication::get_approximately_area ( )
{
	int x_root , y_root , relat_x , relat_y ;
	unsigned int mask_return;

	std::cout << ZPRAVA1 << std::endl;    counting ();

	// doplni abs. adresu na zaklade child window
	if( !XQueryPointer ( display , root , &root , &child ,
		&x_root , &y_root , &(active_area[0].x), &(active_area[0].y), &mask_return ) )
	{
		std::cerr << "XQueryPointer error\n";
		exit (1);
	}

	std::cout << ZPRAVA2 << std::endl;    counting ();

	// doplni druhou soradnici
	if ( !XQueryPointer ( display , root , &root , &child ,
			&x_root , &y_root , &(active_area[1].x), &(active_area[1].y), &mask_return ) )
	{
		std::cerr << "XQueryPointer error\n";
		exit (1);
	}

	if ( check_and_modify ( active_area ) ) 
	{
		std::cerr << "Chyba v uzivatelskem vstupu\n";
		exit (1);
	}

}

// ma za ukol zjistit presne velikosti policek na zaklade uzivatelem zpusobenem posunuti
void Comunication::get_accurately_area(  )
{
	unsigned long * first_picture, * second_picture, * third_picture;

	std::cout << ZPRAVA3 << std::endl; counting();
	first_picture = get_print_screen ( ); // first screen shot

	int rozmery_plochy = active_area[1].x - active_area[0].x;

	unsigned long last_pix = 0;
	int in_row = 1;
	// zkoumani usecek horizontalnich - boarders
	for ( int y = 0 ; y < rozmery_plochy ; ++y )
	{
	for ( int x = 0 ; x < rozmery_plochy ; ++x )
	{
		if ( last_pix == first_picture[ x + y*rozmery_plochy ] )
			++in_row;
		else
		{
			if ( (float)in_row >= ((float)rozmery_plochy)*3.f/4.f ) // je to vetsi ==>boarder
			{ // boarder
				active_area[1].x -= rozmery_plochy - x + 1;
				active_area[0].x += x - in_row + 1; // vc. b. 
				active_area[0].y += y + 1; // vc. b.
				active_area[1].y -= 	active_area[1].y - active_area[0].y - 
							( active_area[1].x - active_area[0].x ); 
				goto unik;
			}

			in_row = 1;
			last_pix = first_picture[ x + y* rozmery_plochy ];
		}
	}
		in_row = 0;
	}
	unik:

	free( first_picture ); // uprava hranic hohova
}
 
// ceka na stisknuti klavesy nasledne postupne vypisuje odpocitavani
static void counting () 
{
	using namespace std;

	cin.get();
	cin.clear();
	for ( int time = 2; time > 0 ; --time )
		{cout << time << " " << flush; sleep(1);}
	cout << 0 << endl;
}

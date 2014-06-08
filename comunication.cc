#include <iostream>
#include <cassert>
#include <cmath>
#include <unistd.h> // sleep
#include "comunication.h"

#define ZPRAVA1 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad levim hornim rohem hraciho pole |^^ \n"\
		"Budete mit 4s na splneni" 

#define ZPRAVA2 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad pravym dolnim rohem hraciho pole _|\n"\
		"Budete mit 4s na splneni"

#define ZPRAVA3 "Nejprve volnym polem prejedte dolni pravy roh hraci plochy\n"\
		"Potom stisknete enter a odstrante z hraci plochy kurzor"

#define ZPRAVA4 "Volnym polickem prejedte primo do leveho dolniho rohu\n"\
		"Potom stisknete enter a odstrante z hraci plochy kurzor"

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
}

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

// ma za ukol zjistit presne velikosti policek na zaklade uzivatelem zpusobenem posunuti
void Comunication::get_accurately_area(  )
{
	unsigned long * first_picture, * second_picture;

	std::cout << ZPRAVA3 << std::endl; counting();
	first_picture = get_print_screen ( ); // first screen shot

	std::cout << ZPRAVA4 << std::endl; counting();
	second_picture = get_print_screen ( ); // second screen shot 

/* nejprve projdu velikosti zacnu na velikosti size/6 a postupne az na size/4 vice nemuze policko mit
 * dale budu porovnavat policko 0 z first_picture ( pozice 16 dole v pravo )
 * budu meni postupne offset x a y souradnic 
 * to same budu delat i s second_picture dokud nenajdu shodu v celem obrazci -> mam velikost policka */

	int rozmer_plochy = active_area[1].x - active_area[0].x; // je ctvercova
	int size = rozmer_plochy/4; // puvocni rozmer jednoho policka
	int offset_x1, offset_x2 , offset_y1 , offset_y2 ; // offsety od puvodniho ramu
	int ctverec_1_offset, ctverec_2_offset;
	bool nasel_se_ctverec = false;
	
 for (; size >= rozmer_plochy/6; --size )
    for ( offset_x1 = 0; offset_x1 < rozmer_plochy/6; ++offset_x1 )
       for ( offset_y1 = 0; offset_y1 < rozmer_plochy/6; ++offset_y1 )
          for ( offset_x2 = 0; offset_x2 < rozmer_plochy/6; ++offset_x2 )
             for ( offset_y2 = 0; offset_y2 < rozmer_plochy/6; ++offset_y2 )
	     { // nini se musi kontrolovat ctverec
		ctverec_1_offset = 	rozmer_plochy*(rozmer_plochy - offset_y1 - size) + 
					rozmer_plochy - offset_x1 -size;

		ctverec_2_offset = 	rozmer_plochy*(rozmer_plochy - offset_y2 - size) + 
					offset_x2;
		nasel_se_ctverec = true;

		for ( int y = 0 ; y < size && nasel_se_ctverec ; ++y)
		for ( int x = 0 ; x < size                     ; ++x)
		{
			if ( 	first_picture [ctverec_1_offset + x + y*rozmer_plochy] != 
				second_picture[ctverec_2_offset + x + y*rozmer_plochy]   )
				{ nasel_se_ctverec = false; break; }
				
		}
		if ( nasel_se_ctverec )
			goto nasel_jsem_to;
             }

nasel_jsem_to:
	if ( !nasel_se_ctverec )
	{
		std::cerr << "Ctverec se nepodarilo detekovat\n";
		exit ( 1 );
	}

	double sum = 0;
	for ( int y = 0; y < size ; ++y )
	for ( int x = 0; x < size ; ++x )
	{
		unsigned long t = second_picture[ ctverec_2_offset + x + y*rozmer_plochy];
		sum += t;
	}

	sum /= size*size;

	for ( int y = 0; y < size ; ++y ) {
	for ( int x = 0; x < size ; ++x )
	{
		unsigned long t = second_picture[ ctverec_2_offset + x + y*rozmer_plochy];
		if ( sum*4 < t )
			std::cout << " ";
		else if ( sum*2 < t )
			std::cout << ".";
		else if ( sum < t )
			std::cout << ":";
		else if ( sum/2 < t )
			std::cout << "*";
		else if ( sum/4 < t )
			std::cout << "#";
	} std::cout << std::endl; }
	

	free( first_picture );
	free( second_picture);
}

void Comunication::screen_capture ()
{
}

void Comunication::get_cursor ( )
{
	// vsechny mozne soradnice co varaci xquerypointer
	return ;
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

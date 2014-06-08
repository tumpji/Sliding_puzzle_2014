#include <iostream>
#include <unistd.h> // sleep
#include "comunication.h"

#define ZPRAVA1 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad levim hornim rohem hraciho pole |^^ \n"\
		"Budete mit 4s na splneni\n" 

#define ZPRAVA2 "Po stisknuti tlacitka enter prejedte na okno se hrou\n"\
		"Podrzte nad pravym dolnim rohem hraciho pole _|\n"\
		"Budete mit 4s na splneni\n"

COORDINATES::COORDINATES () : x(0), y(0) {}
COORDINATES::~COORDINATES () {}


static void counting () 
{
	using namespace std;

	cin.get();
	cin.clear();
	for ( int time = 4; time > 0 ; --time )
		{cout << time << " " << flush; sleep(1);}
	cout << 0 << endl;
}

Comunication::Comunication ()
{
	int x_root , y_root , relat_x , relat_y ;
	unsigned int mask_return;
	display = XOpenDisplay ( 0 ); // masina
	screen = DefaultScreen ( display ); // obrazovka
	root = RootWindow ( display , screen ); // nahore

	std::cout << ZPRAVA1 << std::endl;    counting ();

	// doplni abs. adresu na zaklade child window
	if( !XQueryPointer ( display , root , &root , &child ,
		&x_root , &y_root , &(active_area[0].x), &(active_area[0].y), &mask_return ) )
		exit( 10 );

	std::cout << ZPRAVA2 << std::endl;    counting ();

	// doplni druhou soradnici
	if ( !XQueryPointer ( display , root , &root , &child ,
			&x_root , &y_root , &(active_area[1].x), &(active_area[1].y), &mask_return ) )
		exit(10);

	// debug info
	std::cout << "absolute size:\n";
	std::cout 	<< "left: " << active_area[0].x << "\tright:  " << active_area[1].x
			<< std::endl
			<< "top:  " << active_area[0].y << "\tbottom: " << active_area[1].y
			<< std::endl
			<< "x: " << active_area[1].x - active_area[0].x << std::endl
			<< "y: " << active_area[1].y - active_area[0].y 
			<< std::endl;
}

void Comunication::screen_capture ()
{
	// vem obrazek
	XImage * image = XGetImage ( display , root ,
				 active_area[0].x,active_area[0].y,
				active_area[1].x - active_area[0].x,
				active_area[1].y - active_area[0].y,
				XAllPlanes(),ZPixmap);

	if ( image != nullptr ) // povedlo se ti ho vyfotit ?
	{
		for ( int y = 0 ; y < active_area[1].y - active_area[0].y; ++y )
		{
			for ( int x = 0 ; x < active_area[1].x - active_area[0].x ; ++x )
			{
				unsigned long t = image->f.get_pixel( image , x , y);

				if ( t >= (unsigned long)image->red_mask )
					std::cout << " ";
				else if ( t > (unsigned long)image->red_mask/2 )
					std::cout << ".";
				else if ( t > (unsigned long)image->red_mask/4 )
					std::cout << ":";
				else if ( t > (unsigned long)image->red_mask/8 )
					std::cout << "~";
				else if ( t > (unsigned long)image->red_mask/16 )
					std::cout << "o";
				else if ( t > (unsigned long)image->red_mask/32 )
					std::cout << "0";
				else
					std::cout << "@";
			}
			std::cout << std::endl;
		}
	}

}

void Comunication::get_cursor ( )
{
	// vsechny mozne soradnice co varaci xquerypointer
	return ;
	int x_root , y_root , relat_x , relat_y ;
	unsigned int mask_return;

	//bool val = XQueryPointer ( display , root , &root , &child ,
	//		&x_root , &y_root , &relat_x , &relat_y ,
			//&mask_return );

	//std::cout << val << std::endl;
	std::cout 	<< "x_root: " << x_root << "\t" 
			<< "y_root: " << y_root << std::endl;		

	std::cout 	<< "relat_x: " << relat_x << "\t" 
			<< "relat_y: " << relat_y << std::endl;		

	XQueryPointer ( display , child , &root , &child , &x_root , &y_root , &relat_x , &relat_y , &mask_return );
	std::cout 	<< "x_root: " << x_root << "\t" 
			<< "y_root: " << y_root << std::endl;		

	std::cout 	<< "relat_x: " << relat_x << "\t" 
			<< "relat_y: " << relat_y << std::endl;		
	std::cin >> x_root;
	XQueryPointer ( display , child , &root , &child , &x_root , &y_root , &relat_x , &relat_y , &mask_return );
	
	std::cout 	<< "x_root: " << x_root << "\t" 
			<< "y_root: " << y_root << std::endl;		

	std::cout 	<< "relat_x: " << relat_x << "\t" 
			<< "relat_y: " << relat_y << std::endl;		
}

 


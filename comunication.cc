#include <iostream>
#include "comunication.h"

#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>

int funkce();

Comunication::Comunication ()
{
	Display * display;
	int screen;
	Window root;
	display = XOpenDisplay ( 0 );
	screen = DefaultScreen ( display );
	root = RootWindow ( display , screen );
	
	XImage * image = XGetImage ( display , root, 0,0,400,400,XAllPlanes(),ZPixmap);
	if ( image != nullptr )
	{
		std::cout << "povedlo se " << std::endl;	
	}

	for ( int y = 0 ; x < 30 ; ++x )
	for ( int x = 0 ; x < 30 ; ++x )
	{
	}
	
}

 


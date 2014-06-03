#!/usr/bin/env python3

import IOmodule
import random

#x = IOmodule.IOmodule( "echo" )

# hleda chyby v modulu object.o

class Generator:
	def __init__ ( self ):
		self.policka = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]
		self.generate_random ()

	def __del__ ( self ):
		pass

	def generate_random ( self ):
		random.shuffle( self.policka )

	def predej_programu ( self ):
		temp_string = "("	
		for number in self.policka:
			temp_string += str(number) + ","
		temp_string += ")"
		print ( temp_string )
			
			
		
	

x = Generator ()
x.predej_programu ()

#!/usr/bin/env python3

import IOmodule
import random
import time

#x = IOmodule.IOmodule( "echo" )

# hleda chyby v modulu object.o
class Rozhrani:
	def __init__ ( self ):
		self.program = IOmodule.IOmodule ( "./prog.run" )

	def __del__ ( self ):
		pass

	def predej_programu ( self , policka ):
		temp_string = "("	
		for number in policka:
			temp_string += str(number) + ","
		temp_string += ")\n"
		self.program.Write ( temp_string )

	def check_output ( self , tuple_tuple ):
		if len(tuple_tuple) > 4: # nemuze se vratit vice
			return True

		for tup in tuple_tuple: # nemuze byt vetsi
			if ( len(tup) != 16 ): # zkontroluje velikost
				return True
			
			suma = 0
			for elem in tup:
				suma += elem

			if suma != 136:
				return True
				
		return False # bez chyby ( ne zcela )

	def print_error_message ( self, list_tuple ):
		string = "Spatny format vystupniho retezce\nByl zpracovan jako:\n"
		print ( string )
		print ( list_tuple )

	def preber_od_programu ( self ): #vlastne parser testu
		list_tuple = list()
		temp_list = list()
		temp_number = 0
		string = self.program.Read ()

		for i in string:
			if   i == '{' : 	# zacinam odpovidat
				pass
			elif i == '}' :	# koncim odpoved programu
				pass
			elif i == '(' :	# zacina prvni list
				temp_list = list() 	# init na novy
			elif i == ')' : 		# konec listu
				list_tuple += tuple(temp_list),# predpoklada se ok format
				temp_list = list()
			elif i == ',' :
				temp_list += temp_number,
				temp_number = 0
			elif i <= '9' and i >= '0': #number
				temp_number *= 10
				temp_number += int(i)

		if ( self.check_output( list_tuple ) ):
			self.print_error_message ( list_tuple )
			raise IOmodule.MyError ( " Spatny format " )

		return tuple(list_tuple)



			
class TestModule:
	def __init__ ( self ):
		self.policka = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]
		self.output = None
		print( "INICIALIZACE" )
		self.rozhrani = Rozhrani()

	def __del__ ( self ):
		print ( "DESTRUKTOR" )

	def get_random ( self ):
		random.shuffle( self.policka ) #prohazi to

	def get_dismatch_indexes ( self , predany_tuple ):
		ret_val = tuple()
		for index in range( 0 , 15 ):
			if self.policka[index] != predany_tuple[index]:
				ret_val += index,
		if len(ret_val) != 2:
			self.tisk_chyby ()
		return ret_va

	def check_output ( self ):
		print ( "moje :" + str( self.policka ) )
		print ( "tvoje:" + str( self.output  ) ) 
		for i in range( 0 , 15 ):
			if ( self.policka[i] != self.output[0][i] ):
				print ( "Chyba" )

	def Post_test ( self ):
		print ( "TEST" )
		self.get_random () # prohazeni
		self.rozhrani.predej_programu ( self.policka )
		self.output = self.rozhrani.preber_od_programu ()
		self.check_output ()

	
		
program = TestModule()
program.Post_test ()
		
		

exit (0 )
# inicializace
x = Rozhrani ()

# vola generate_random
x.predej_programu ()
x.generate_random ()
x.predej_programu ()

listy = x.preber_od_programu ()

a = 0
for li in listy:
	print ( li )
	a += 1

listy = x.preber_od_programu ()

for li in listy:
	print ( li )
	a += 1

print ( "pocet :" + str( a ) )













#!/usr/bin/env python3

import IOmodule
import random
import time

#x = IOmodule.IOmodule( "echo" )

# hleda chyby v modulu object.o
class Rozhrani:
	def __init__ ( self ):
		self.program = IOmodule.IOmodule ( "./test_object.run" )

	def __del__ ( self ):
		pass

	def process_var_set ( self , policka ):
		temp_string = "set: ("	
		for number in policka:
			temp_string += str(number) + ","
		temp_string += ")\n"
		self.program.Write ( temp_string )

	def process_send_command ( self , command ):
		command += "\n"
		self.program.Write ( command )

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
		self.rozhrani = Rozhrani()
		self.number_of_test = 0
		self.current_test = 0

	def __del__ ( self ):
		pass

	# nastavi "nahodnnou" hodnou v self.policka a nastavi jaky test se pojede
	def get_random ( self ):
		random.shuffle( self.policka ) #prohazi to
		self.current_test =  random.randint ( 0 , 1 )

	# tiskne vstup a vystup z testu v pripade chyby
	def print_error_msg ( self ): # tiskne puvodni a predany
		string = "Puvodni:__________\n"
		for i in range ( 0 , 16 ):
			if i % 4 == 0:
				string += "|"
			string += "{0:^4}".format(str(self.policka[i]))
			if i % 4 == 3:
				string += "|\n"
		string += "^" * (4*4 + 2 ) 
		print ( string )
		print ( "vysledne:" )
		zasobnik = [ str(), str(), str(), str(), str(), str(), ]
		for out in self.output:
			zasobnik[0] += "_" * ( 4*4 + 2 )
			zasobnik[5] += "^" * ( 4*4 + 2 )
			if len(zasobnik[0] ) > 19:
				zasobnik[0] += "_"
				zasobnik[5] += "^"
			for index in range ( 0 , len(out) ):
				if index % 4 == 0:
					zasobnik[index//4 + 1] += "|"
				zasobnik[index//4 + 1] += "{0:^4}".format(str(out[index]))
				if index % 4 == 3:
					zasobnik[index//4 + 1] += "| "
		for string in zasobnik:	
			print ( string )
		print ( "#"*40 )		

	# zjisti jake dva idnexy se oproti poslanemu zmenily
	# vraci MyError jestli se nezmenili 2
	def get_dismatch_indexes ( self , predany_tuple ):
		ret_val = tuple()
		for index in range( 0 , 15 ):
			if self.policka[index] != predany_tuple[index]:
				ret_val += index,
		if len(ret_val) != 2:
			raise IOmodule.MyError( "Nesplnuje pozadavky" )
		return ret_va

	# kontroluje jestli test dopadl uspesne nebo ne
	def check_output ( self ):
		try:
			dismatch_tuple = self.get_dismatch_indexes ( self.output[0])
			print ( "Test probehl uspesne" )
		except IOmodule.MyError: 
			self.print_error_msg ()
	
	# generuje ruzne testy	
	def Post_test ( self ):
		for i in range( 0 , 10 ):
			self.get_random () # prohazeni
			self.rozhrani.process_var_set ( self.policka )
			self.rozhrani.process_send_command ( "getbest" )
			self.output = self.rozhrani.preber_od_programu ()
			self.check_output ()
			

	
		
program = TestModule()
program.Post_test ()
		
		














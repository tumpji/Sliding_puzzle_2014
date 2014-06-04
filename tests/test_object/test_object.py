#!/usr/bin/env python3

import IOmodule
import random
import time


# zpracuje output z testovani a zkontroluje syntax nasledne prevede do pozadovaneho formatu
class Rozhrani:
	# otevre soubor a pripravy vsechny fifa
	def __init__ ( self ):
		try:
			self.program = IOmodule.IOmodule ( "./test_object.run" )
		except:
			print ( "Pri spusteni modulu IOmodule nastala chyba" )
			exit ( 0 )

	def __del__ ( self ):
		pass

	# posle programu zpravu aby nastavil promennou OBJECT do pozadovaneho stavu
	# vstup je tuple/list ( iterovatelny obj ) o velikosti == 16 elem.
	def process_var_set ( self , policka ):
		assert ( len( policka ) == 16 ) 
		temp_string = "("	
		for number in policka:
			temp_string += str(number) + ","
		temp_string += ")\n"
		try:
			self.program.Write ( temp_string )
		except: 
			print ( "Nepodarilo se odeslat zpravu nastav var programu" )
			exit ( 0 )

	# posle textovou zpravu programu
	# prebira argument command tj str() za nej automaticky doplnuje mezeru a odesle
	def process_send_command ( self , command ):
		command += "\n"
		self.program.Write ( command )

	# zjisti jestli to co obdrzel odpovida ocekavanemu formatu
	# prebira iterovatelny obj ktery ma v sobe nekolik iterovatelnych obj
	def check_output ( self , tuple_tuple ):
		if len(tuple_tuple) > 4: # nemuze se vratit vice
			raise IOmodule.MyError ( "Velikost vstupu neodpovida" )	

		for tup in tuple_tuple: # nemuze byt vetsi
			if ( len(tup) != 16 ): # zkontroluje velikost
				raise IOmodule.MyError ( "Velikost podprvku neodpovida" )	

	# parser z programu returnuje ( (,,,,,...) , (,,,,...) ... ) podle programu
	def preber_od_programu ( self ): #vlastne parser testu
		list_tuple = list()
		temp_list = list()
		temp_number = 0

		try:
			string = self.program.Read ()
		except:
			print ( "Chyba pri cteni IOmodule::Read " )
			exit ( 0 )

		for i in string:
			if   i == '{' : 	# zacinam odpovidat
				pass
			elif i == '}' :		# koncim odpoved programu
				pass
			elif i == '(' :				# zacina novy vysledek
				temp_list = list() 		# vycistit temp
			elif i == ')' : 			# konec listu
				list_tuple += tuple(temp_list),	# pridat k vysledku
				temp_list = list()		# pro jistotu mazem znova
			elif i == ',' : # indikuje konec cisla
				temp_list += temp_number, # pridani do momentalne zpr. vysl.
				temp_number = 0
			elif i <= '9' and i >= '0':  # cisla 
				temp_number *= 10
				temp_number += int(i)

		try: # kontroluje jestli je v poradku to co bylo zpracovano ( pocet poloz. ... )
			self.check_output( list_tuple ) 
		except IOmodule.MyError as msg:
			print ( "Neodpovida format vstupu" )
			print ( msg + "\nByl zpracovan jako:\n" )
			print ( list_tuple )
			exit (0)

		return tuple(list_tuple)


################################################################################################

# tahle trida se zabiva jestli odpovida to co bylo 	
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
		print ( "#"*40 )		
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
		for index in range( 0 , 16 ):
			if self.policka[index] != predany_tuple[index]:
				ret_val += index,
		if len(ret_val) != 2:
			raise IOmodule.MyError( "CHYBA:\nRozdil neni ve dvou prvcich" )
		return ret_val

	# kontroluje jestli test dopadl uspesne nebo ne
	# NEDOKONCENO
	def check_output ( self ):
		try:
			dismatch_tuple = self.get_dismatch_indexes ( self.output[0])
		except IOmodule.MyError as msg:
			print ( msg )
			self.print_error_msg ()
	
	# generuje ruzne testy	
	def Post_test ( self ):
		for i in range( 0 , 10 ):
			self.get_random () # prohazeni
			print ( self.policka )
			self.rozhrani.process_var_set ( self.policka )
			self.rozhrani.process_send_command ( "getbest" )
			self.output = self.rozhrani.preber_od_programu ()
			self.check_output ()
		print ( "Testovani zkoncilo" )
			
		
program = TestModule()
program.Post_test ()
		
		














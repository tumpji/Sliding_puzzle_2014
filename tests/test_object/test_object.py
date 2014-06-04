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
	def __init__ ( self , pocet_testu ):
		self.policka = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]
		self.output = None
		self.rozhrani = Rozhrani()

		self.number_of_tests = pocet_testu
		self.current_test = 0
		self.passed_tests = 0

	def __del__ ( self ):
		pass

	# nastavi "nahodnnou" hodnou v self.policka a nastavi jaky test se pojede
	def get_random ( self ):
		random.shuffle( self.policka ) #prohazi to
		self.current_type_of_test =  random.randint ( 0 , 0 )

	# tiskne vstup a vystup z testu v pripade chyby
	def print_comparation ( self ): # tiskne puvodni a predany
		print ( "#"*40 )		
		string = "Puvodni:__________\n"
		for i in range ( 0 , 16 ):
			if i % 4 == 0:
				string += "|"
			if self.policka[i] != 16:
				string += "{0:^4}".format(str(self.policka[i]))
			else:
				string +=  ">{0:^2}<".format(str(self.policka[i]))
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
				if out[index] != 16:
					zasobnik[index//4 + 1] += "{0:^4}".format(str(out[index]))
				else:
					zasobnik[index//4 + 1] += ">{0:^2}<".format(str(out[index]))
				if index % 4 == 3:
					zasobnik[index//4 + 1] += "| "

		for string in zasobnik:	
			print ( string )
		print ( "#"*40 )		

	# zjisti jake dva idnexy se oproti poslanemu zmenily
	# raise MyError v pripade:
		# nejsou vsechny cisla obsazena 1-16 vc.
		# soucet cisel neni takovy jaky ma byt
		# nezmenili se prave 2 prvky
		# zmenlo se poradi prvku s mezerou
	# pro original je mezera [0]
	def __get_dismatch_indexes ( self , predany_tuple ):
		try: # nejprve kontroluje jestli jsou obsazeny vsechny ocekavane hodnoty
			for num in range( 1 , 17 ):
				predany_tuple.index ( num )
		except ValueError:
			raise IOmodule.MyError ( "CHYBA:\nNejsou obsazeny vsechny pozadovane cisla" )

		if sum(predany_tuple) != 136: # soucet hodnot take musi odpovidat
			raise IOmodule.MyError( "CHYBA:\nSoucet prvku neni 136" )

		ret_val = tuple() # zkontroluje jestli jsou opravdu zmeneny prave 2 indexy
		for index in range( 0 , 16 ):
			if self.policka[index] != predany_tuple[index]:
				ret_val += index,
		if len(ret_val) != 2:
			raise IOmodule.MyError( "CHYBA:\nRozdil neni ve dvou prvcich" )

		# zkontroluje, jestli se zmenila mezera ( 16 ) , prevrati poradi aby
		if self.policka[ret_val[0]] != 16:
			if self.policka[ret_val[1]] != 16:
				raise IOmodule.MyError ( "CHYBA:\nNezmenil se element 16 - mezera" )
			return ( ret_val[1],ret_val[0], ) #obrati poradi
		else:
			return ret_val
	
	# preda tuple ktere rekne jestli je mozne na tuhle stranu posuvat z 
	# hodnoty bere z self.policka
	# ( leva , prava , nahoru , dolu ) ( 0 - ne , 1 - ano )
	def __is_sliding_suitable ( self ):
		ret_list = [ 0,0,0,0, ]
		index = self.policka.index( 16 )
		if index % 4 != 0:
			ret_list[0] = 1
		if index % 4 != 3:
			ret_list[1] = 1
		if index // 4 != 0:
			ret_list[2] = 1
		if index // 4 != 3:
			ret_list[3] = 1
		return tuple( ret_list )

	# zjisti jestli je ok posunuti a preda list s hod. 1 pokud posunuti je timto smerem 
	def check_sliding ( self , predany_tuple ):
		slide_enabled = self.__is_sliding_suitable ()
		ret_lis = [0,0,0,0] # levo pravo nahoru dolu
		dismatch_tuple = self.__get_dismatch_indexes ( predany_tuple )
		
		if slide_enabled[0] and (dismatch_tuple[1] == dismatch_tuple[0] - 1):
			ret_lis[0] = 1
			return ret_lis
		elif slide_enabled[1] and (dismatch_tuple[1] == dismatch_tuple[0] + 1):
			ret_lis[1] = 1
			return ret_lis
		elif slide_enabled[2] and (dismatch_tuple[1] == dismatch_tuple[0] - 4 ):
			ret_lis[2] = 1
			return ret_lis
		elif slide_enabled[3] and (dismatch_tuple[1] == dismatch_tuple[0] + 4 ):
			ret_lis[3] = 1
			return ret_lis
		else:
			raise IOmodule.MyError ( "CHYBA:\nPosunuti je divne az to boli" )

	# kontroluje jestli test dopadl uspesne nebo ne
	# NEDOKONCENO
	def check_output ( self ):
		try:
			moved = self.check_sliding ( self.output[0])
			self.passed_tests += 1
			if 1:
				print ( str(self.policka) + " ->\n" +
					str(self.output[0]) + " OK " + str(moved) + "\n" + "^"*58 )
		except IOmodule.MyError as msg:
			print ( msg )
			self.print_comparation ()
	
	# generuje ruzne testy	
	def Post_test ( self ):
		while self.current_test < self.number_of_tests:
			self.current_test += 1
			self.get_random () # prohazeni
			self.rozhrani.process_var_set ( self.policka )

			if self.current_type_of_test == 0: # vyber druhu testu
				self.rozhrani.process_send_command ( "getbest" )
			elif self.current_type_of_test == 1:
				self.rozhrani.process_send_command ( "getall" )
			#elif self.current_type_of_test == 2:
				#self.rozhrani.process_send_command ( "iterative_check" )

			self.output = self.rozhrani.preber_od_programu ()
			self.check_output () # vyhodnoceni jednoho testu
		print ( "Testovani skoncilo " ) # zaverecna statistika 
		print ( "vysledek: " + str(self.passed_tests) + "/" + str(self.number_of_tests) + 
		 "\t{0:.1%}".format(self.passed_tests/self.number_of_tests) )
		
		
program = TestModule( 10 )
program.Post_test ()
		
		














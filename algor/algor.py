#!/usr/bin/env python3

import random
import math
import sys

pocet_clenu 	= int(40) # kolik clenu se vygeneruje ke konvergenci
pocet_zkousek 	= int(500) # kolikrat se bude pocitat

minimum = 2.8
optimum = 3.3
maximum = 4.0

conv_value = 100.
minimum_conv = int( minimum * 100 )
optimum_conv = int( optimum * 100 )
maximum_conv = int( maximum * 100 )

class RandEngine:
	def __init__(self) :
		self.cleny_t = list()

	def vypocet_smerodatne_odchylky ( self ):
		prumer = self.vypocet_prumeru ()
		temp = 0.
		for prvek in self.cleny_t:
			temp += (prvek - prumer)**2
		temp *= 1./float(len(self.cleny_t))
		temp = math.sqrt( temp )
		return temp

	def vypocet_prumeru ( self ):
		return float( sum( self.cleny_t ) ) / float( len( self.cleny_t  ) )

	def spustit_test ( self ):
		list_smer_odch = list()
		list_prum = list()
		smer_odch_ze_smer_odch = float( 0 )

		for i in range( 0 , pocet_zkousek ):
			self.generovani_clenu() # vygeneruje se nahodne udaje
			act_prum = self.vypocet_prumeru ()
			act_smer = self.vypocet_smerodatne_odchylky ()

			list_smer_odch.append( act_smer )
			list_prum.append( act_prum )

		# vyuzijem vytvorene fce pro vypocet zhodnoceni
		print ( " PRUMER --- PRUMER ---- PRUMER ---- " )
		self.cleny_t = list_prum
		print ( "Min : " + "{0:5.5}\t".format( min(list_prum))  +
		 	"Max : " + "{0:5.5}\t".format( max(list_prum) )  +
		 	"Prum : "+ "{0:5.5}\t".format( self.vypocet_prumeru() ) +
			"Smerod. : " + "{0:5.5}".format(self.vypocet_smerodatne_odchylky()) )

		print ( " Smerodatna odchylka ..... " )
		self.cleny_t = list_smer_odch
		print ( "Min : " + "{0:5.5}\t".format( min( list_smer_odch ) )  +
		 	"Max : " + "{0:5.5}\t".format( max( list_smer_odch ) )  +
		 	"Prum : " + "{0:5.5}\t".format( self.vypocet_prumeru() ) +
		 	"Smerod. : " + "{0:5.5}".format(self.vypocet_smerodatne_odchylky()))
		print ( )
		
			
	
	def tisk_clenu ( self ):
		if 1:
			na_radku = 0
			for prvek in self.cleny_t:
				na_radku += 1
				sys.stdout.write( "{0:<4}   ".format( prvek ) )
				if na_radku >= 14:
					sys.stdout.write( "\n" )
					na_radku = 0
			if na_radku:
				sys.stdout.write( "\n" )

		print ( "Prumer : " + "{0:.6}".format( self.vypocet_prumeru()  ) )
		print ( "Odchyl.: " + "{0:.6}".format( self.vypocet_smerodatne_odchylky()  ) )

	def generovani_clenu ( self ):
		self.cleny_t.clear()	# -||-
		for n_t in range ( 0 , pocet_clenu ):
			now = self.get_rand_min_max ()
			self.cleny_t.append ( now )

# ----------------------------------------------------------------------------------
class RandEngine_comrand( RandEngine ):
	def get_rand_min_max( self ):
		return float( 
			random.randint ( minimum_conv , maximum_conv ) 
			) / conv_value

class RandEngine_hcsigma( RandEngine ):
	def __init__ ( self , mocnina ):
		RandEngine.__init__( self )
		self.mocnina = mocnina
	def get_rand_min_max( self ):
		predpokladany_prumer =  sum( self.cleny_t )* 100.
		#sys.stdout.write ( "pred 1: " + "{0:6.6}".format(str( int(predpokladany_prumer) )) )
		predpokladany_prumer += ( - len( self.cleny_t ) + pocet_clenu ) * optimum_conv
		#sys.stdout.write ( "\tpred 2: " + "{0:6.6}".format(str( int(predpokladany_prumer) )) )
		predpokladany_prumer /= pocet_clenu * optimum_conv
		#sys.stdout.write ( "\tpred 3: " + str( predpokladany_prumer ) + "\n" )
		# predpokladany prumer je hodnota +- 1, ukazuje na odlisnost od plne optimum
		
		loc_max = maximum_conv
		loc_min = minimum_conv	
		if ( predpokladany_prumer > 1 ): # jsme moc 
			loc_max /= ( predpokladany_prumer )**self.mocnina
			if ( loc_max < loc_min ):
				loc_max = loc_min
				print ( "puck max" )
		else:
			loc_min /= (predpokladany_prumer )**self.mocnina
			if ( loc_min > loc_max ):
				loc_min = loc_max
		
		return float (
			random.randint ( int(loc_min) , int(loc_max) ) ) / conv_value
			
#engine = RandEngine_hcsigma()
#engine.generovani_clenu()

print ( "comrand:" )	
engine = RandEngine_comrand()
engine.spustit_test()

for mocnina in range ( 1 , 31 , 1 ):
	print ( "hcsigma:   mocnina = " + str(mocnina)  )	
	engine = RandEngine_hcsigma( mocnina )
	engine.spustit_test()













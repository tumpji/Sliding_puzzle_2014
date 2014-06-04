#!/usr/bin/env python3 import os
import sys
import subprocess

# objekt se stara o nejzakladnejsi komunikaci mezi testovanym programem
class IOmodule:
	def __init__ ( self , name ):
		try:
			self.program = subprocess.Popen (
			[name],stdin=subprocess.PIPE , stdout = subprocess.PIPE)
		except:
			print ( "Nepodarilo se spustit program" )
			exit ( 0 ) 
		# doplnit 

	# ukonci program a uvolni zdroje
	def __del__ ( self ):
		print ( "Cekam na ukonceni programu" )
		try:
			self.program.communicate( timeout=5 )
		except subprocess.TimeoutExpired as pr:
			print ( "Program se nepodarilo ukoncit vcas" )
			self.program.kill ()
			abort ( 1 )
		print ( "Program se ukoncil" )
	
	# preda testovanemu programu radek 
	def Write ( self , string ):
		try:
			self.program.stdin.write ( string.encode("UTF-8") )	
			self.program.stdin.flush ()
		except:
			print ( "Nepodarilo se predat programu informace" )
			exit ( 0 )

	# cte radek od testovaneho programu
	def Read  ( self ):
		try:
			line = ""
			while line == "":
				line = self.program.stdout.readline().decode( "UTF-8" )
		except:
			print ( "Nepodarilo se prebrat informace z programu" )
			exit ( 0 )
		return line


# class na predavani vyjimek
class MyError ( Exception ):
	def __init__ ( self, message ):
		self.message = message
	def __str__ ( self ):
		return self.message



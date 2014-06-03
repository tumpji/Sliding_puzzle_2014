#!/usr/bin/env python3
import os
import sys
import subprocess

class IOmodule:
	def __init__ ( self , name ):
		self.program = subprocess.Popen (
			[name],stdin=subprocess.PIPE , stdout = subprocess.PIPE )
	def __del__ ( self ):
		print ( "Cekam na ukonceni programu" )
		try:
			self.program.communicate( timeout=5 )
		except subprocess.TimeoutExpired:
			print ( "Program se nepodarilo ukoncit" )
			self.program.kill ()
			abort ( 1 )
		print ( "Program se ukoncil" )
	
	def Write ( self , string ):
		self.program.stdin.write ( string.encode("UTF-8") )	
		self.program.stdin.flush ()

	def Read  ( self ):
		if self.program.poll (): #ukoncil se program ?
			raise "Program skoncil neocekavane"
		return self.program.stdout.readline().decode( "UTF-8" )






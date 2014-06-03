#!/usr/bin/env python3

import os
import time	

class IOmodule:
	def __init__ ( self ):
		self.pipe_in_name = "Testin_pipe"
		self.pipe_out_name = "Testout_pipe"
		os.mkfifo ( self.pipe_in_name )
		os.mkfifo ( self.pipe_out_name  )
		self.WritePipe = open ( self.pipe_out_name , os.O_WRONLY )
		self.ReadPipe= open ( self.pipe_in_name, os.O_RDONLY )

	def __del__ ( self ):
		os.unlink ( self.pipe_in_name )
		os.unlink ( self.pipe_out_name )

	def Post ( self , text ):
		self.WritePipe.write ( text.decode( "UTF-8" ) ) 

IOmodule ()

#Post(IOmodule, "Hello Word !" )

exit ( 0 )
	


PREKLADAC= 	g++ -pipe -Wextra -Werror -pedantic -std=c++11
#PARAMS= 	-Og -g3   #debug
#PARAMS= 	-O3 -mtune=native -m64 -DNDEBUG  #release
PARAMS= 	-pg -O3 -mtune=native -m64 -DNDEBUG  #release



.PHONY: clear
.PHONY: all

all: object.o solving_engine.o
	$(PREKLADAC) $(PARAMS) main.cc object.o solving_engine.o -o program.run

clear:
	-rm program.run object.o solving_engine.o


####################################################################

object.o: object.cc object.h
	$(PREKLADAC) $(PARAMS) -c object.cc -o $@

solving_engine.o: solving_engine.cc solving_engine.h
	$(PREKLADAC) $(PARAMS) -c solving_engine.cc -o $@

PREKLADAC=g++
PARAMS= -Og -g3 -pedantic -Wall -Wextra -Werror -std=c++11 



all:  object.o
	$(PREKLADAC) $(PARAMS) object.o solve_engine.cc -o program

clean:
	-rm program object.o


object.o: object.cc object.h
	$(PREKLADAC) $(PARAMS) -c object.cc -o $@

IDAs.o: IDAs.cc IDAs.h
	$(PREKLADAC) $(PARAMS) -c IDAs.cc -o $@


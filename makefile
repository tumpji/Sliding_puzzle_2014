PREKLADAC=g++
PARAMS= -Og -g3 -pedantic -Wall -Wextra -Werror -std=c++11 


object.o: object.cc object.h
	$(PREKLADAC) $(PARAMS) -c object.cc -o $@


all:  object.o
	$(PREKLADAC) $(PARAMS) object.o solve_engine.cc -o program

.PHONY: clear
clear:
	-rm program object.o


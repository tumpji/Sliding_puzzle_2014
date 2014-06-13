PREKLADAC= 	g++ -pipe -Wextra -Werror -pedantic -std=c++11 
#PARAMS= 	-g3  -ggdb #-Og#debug
PARAMS= 	-g3  -ggdb -pg #-Og#debug
#PARAMS= 	-g3 -ggdb -fprofile-arcs -ftest-coverage   #debug
#PARAMS= 	-pg -O3 -mtune=native -m64 -DNDEBUG  #release
#PARAMS= 	-O3 -mtune=native -m64 -DNDEBUG  #release

#KNIH= $(shell pkg-config  gtkmm-3.0 --cflags --libs) #$(shell pkg-config glibmm-3.0 --cflags --libs )
KNIH=-L/usr/X11R6/lib -lX11 -I/usr/X11R6/include


.PHONY: clear
.PHONY: re
.PHONY: all
.PHONY: gprof

all: object.o solving_engine.o main.cc comunication.o
	$(PREKLADAC) $(PARAMS) main.cc object.o solving_engine.o comunication.o \
	-o program.run $(KNIH)

re: clear all

clear:
	-rm program.run object.o solving_engine.o comunication.o

gprof:
	-rm gmon.out
	./program.run
	gprof program.run gmon.out > test.txt
	-rm gmon.out
	


####################################################################

object.o: object.cc object.h
	$(PREKLADAC) $(PARAMS) -c object.cc -o $@

solving_engine.o: solving_engine.cc solving_engine.h object.h
	$(PREKLADAC) $(PARAMS) -c solving_engine.cc -o $@

comunication.o: comunication.h comunication.cc
	$(PREKLADAC) $(PARAMS) -c comunication.cc -o $@ $(KNIH)
	

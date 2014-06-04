
PREKLADAC= g++
PARAMS= -og -g3 

.PHONY: build
.PHONY: test



build: test_object/test_object.run

test:
	./test_object/test_object.py


test_object/test_object.run: ../object.o test_object/test_object.cc
	$(PREKLADAC) $(PARAMS) test_object/test_object.cc ../object.o -o test_object/test_object.run

CPPC=g++
CFLAGS= -std=c++11 -Wall
PFOLDER=../../src/
all: main.x

main.x : main.cpp $(PFOLDER)pattern_generation.cpp $(PFOLDER)dynamics.cpp
	$(CPPC) $(CFLAGS) -o $@ $^

run: main.x
	@./main.x &>/dev/null

clean:
	@rm -rf *.x *.dat

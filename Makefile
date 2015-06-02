CPPC=g++
CFLAGS= -std=c++11 -Wall

all: main.x

main.x : main.cpp pattern_generation.cpp dynamic.cpp
	$(CPPC) $(CFLAGS) -o $@ $^

run: main.x
	@./main.x

clean:
	rm -rf *.x *.dat

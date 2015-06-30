CPPC=g++
CFLAGS= -std=c++11 -Wall
PFOLDER=../../src
CFLAGS+= -I$(PFOLDER)

SRC_FILES = $(filter-out $(PFOLDER)/main.cpp, $(wildcard $(PFOLDER)/*.cpp))

.PHONY: all link run clean

all: main.x

main.x : main.cpp $(SRC_FILES)
	$(CPPC) $(CFLAGS) -o $@ $^

run: main.x
	@./main.x &>/dev/null

clean:
	@rm -rf *.x *.dat

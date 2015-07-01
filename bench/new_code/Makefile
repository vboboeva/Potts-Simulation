CPPC=icc
CFLAGS= -std=c++11 -Wall
PFOLDER=../../src
OPTFLAGS= -O3 -openmp
CFLAGS+= -I$(PFOLDER)
CMACROS+= -D_BENCH -D_FLOAT_PRECISION

SRC_FILES = $(filter-out $(PFOLDER)/main.cpp, $(wildcard $(PFOLDER)/*.cpp))

.PHONY: all link run clean

all: main.x

main.x : main.cpp $(SRC_FILES)
	$(CPPC) $(CFLAGS) $(OPTFLAGS) $(CMACROS) -o $@ $^

run: main.x
	@./main.x &>/dev/null

clean:
	@rm -rf *.x *.dat

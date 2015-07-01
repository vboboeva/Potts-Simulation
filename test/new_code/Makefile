CPPC=icc
CFLAGS= -std=c++11 -Wall -openmp
PFOLDER=../../src
CFLAGS+= -I$(PFOLDER)
CMACROS+= -D_TEST -D_DOUBLE_PRECISION

SRC_FILES = $(filter-out $(PFOLDER)/main.cpp, $(wildcard $(PFOLDER)/*.cpp))

.PHONY: all link run clean

all: main.x

main.x : main.cpp $(SRC_FILES)
	$(CPPC) $(CFLAGS) $(CMACROS) -o $@ $^

run: main.x
	@./main.x &>/dev/null

clean:
	@rm -rf *.x *.dat

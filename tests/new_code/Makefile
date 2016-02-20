CPPC=mpic++
CFLAGS= -std=c++11 -Wall
PRJ_ROOT=../../
OPTFLAGS=-O2 -fp-model precise
#OPTFLAGS=-O2

MACROS_LIST=_NO_SHUFFLE _FLOAT_PRECISION _TEST
CMACROS=$(foreach d, $(MACROS_LIST), -D$d)

INCLUDE_LIST=$(PRJ_ROOT)/include $(PRJ_ROOT)/lib
CINCLUDE=$(foreach d, $(INCLUDE_LIST), -I$d)

LIB_FILES = $(wildcard $(PRJ_ROOT)/lib/*.cpp)

.PHONY: all run clean


all: main.x

main.x : main.cpp $(LIB_FILES)
	$(CPPC) $(CFLAGS) $(CINCLUDE) $(OPTFLAGS) $(CMACROS) -o $@ $^

run: main.x
	./main.x

clean:
	@rm -rf *.x *.dat *.optrpt

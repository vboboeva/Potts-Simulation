################################################################################
# CONFIGURATION
################################################################################
CPPC=g++
CFLAGS= -std=c++11 -Wall
OPTFLAGS= -O3
CFLAGS+= -ftree-vectorize -fopt-info-vec-missed -march=native -ftree-vectorizer-verbose=5 -mavx -ffast-math or -fassociative-math
CMACRO=-D_FLOAT_PRECISION
SRC=src
ODIR=build
EXE=psim.x

CPP_FILES := $(wildcard $(SRC)/*.cpp)
OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

.PHONY: all clean run test bench compile

################################################################################
# COMPILE
################################################################################

all: $(ODIR)/$(EXE)

build:$(OBJ_FILES)

$(ODIR)/%.o : $(SRC)/%.cpp
	$(CPPC) $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

$(ODIR)/$(EXE): $(OBJ_FILES)
	$(CPPC) $(CFLAGS) $^ -o $@

compile: $(OBJ_FILES)

run: $(ODIR)/$(EXE)
	(cd $(ODIR) && ./$(EXE))

clean:
	@rm -rf $(ODIR)/*.x $(ODIR)/*.dat $(ODIR)/*.txt $(ODIR)/*.o

debug: CFLAGS+=-g
debug: $(ODIR)/$(EXE)

################################################################################
# TESTING
################################################################################

test:
	@$(MAKE) -s -C test
	@$(MAKE) -s -C test clean

################################################################################
# BENCHMARKING
################################################################################

bench:
	@$(MAKE) -s -C bench
	@$(MAKE) -s -C bench clean

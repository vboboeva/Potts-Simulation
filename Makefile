################################################################################
# CONFIGURATION
################################################################################
CPPC=g++
CFLAGS= -std=c++11 -Wall
OPTFLAGS= -O3
#CFLAGS+= -march=native -ftree-vectorize -fopt-info-vec-missed -march=native -ftree-vectorizer-verbose=5
SRC=src
ODIR=build
EXE=psim.x

CPP_FILES := $(wildcard $(SRC)/*.cpp)
OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

.PHONY: all clean run test ciao

################################################################################
# COMPILE
################################################################################

all: $(ODIR)/$(EXE)

$(ODIR)/%.o : $(SRC)/%.cpp
	$(CC) $(CFLAGS) $(OPTFLAGS) -c -o $@ $<

$(ODIR)/$(EXE): $(OBJ_FILES)
	$(CPPC) $(CFLAGS) $^ -o $@

run: $(ODIR)/$(EXE)
	(cd $(ODIR) && ./$(EXE))

ciao:
	@echo $(OBJS)

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

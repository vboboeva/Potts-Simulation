################################################################################
# CONFIGURATION
################################################################################
CPPC=icc
CFLAGS= -std=c++11 -Wall
OPTFLAGS= -O3
CFLAGS+= -qopt-report
CMACRO=-D_FLOAT_PRECISION

SRC=src
ODIR=build
LIB=lib
TESTS=tests
INCLUDE=include $(LIB)

EXE=psim.x

INC_PARAMS=$(foreach d, $(INCLUDE), -I$d)

SRC_FILES := $(wildcard $(SRC)/*.cpp)
LIB_FILES := $(wildcard $(LIB)/*.cpp)
SRC_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(SRC_FILES:.cpp=.o)))
LIB_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(LIB_FILES:.cpp=.o)))

.PHONY: all clean run test bench compile clean_obj

################################################################################
# COMPILE AND LINK
################################################################################

all: $(ODIR)/$(EXE)

exe: $(ODIR)/$(EXE) clean_obj

build:$(OBJ_FILES)


#COMPILE SOURCE FILES
$(ODIR)/%.o : $(SRC)/%.cpp
	$(CPPC) $(INC_PARAMS) $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#COMPILE LIBRARY
$(ODIR)/%.o : $(LIB)/%.cpp
	$(CPPC) $(INC_PARAMS)  $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#LINKING
$(ODIR)/$(EXE): $(SRC_OBJ_FILES) $(LIB_OBJ_FILES)
	$(CPPC) $^ -o $@

lib: $(LIB_OBJ_FILES)

src: $(SRC_OBJ_FILES)

run: $(ODIR)/$(EXE)
	(cd $(ODIR) && ./$(EXE))

clean:
	@rm -rf $(ODIR)/*.x $(ODIR)/*.dat $(ODIR)/*.txt $(ODIR)/*.o $(ODIR)/*.optrpt

clean_obj:
	@rm -rf $(ODIR)/*.o $(ODIR)/*.optrpt

debug: CFLAGS+=-g
debug: $(ODIR)/$(EXE)

################################################################################
# TESTING
################################################################################

test:
	@$(MAKE) -s -C $(TESTS)
	@$(MAKE) -s -C $(TESTS) clean

################################################################################
# BENCHMARKING
################################################################################

bench:
	@$(MAKE) -s -C bench
	@$(MAKE) -s -C bench clean

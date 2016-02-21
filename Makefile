################################################################################
# CONFIGURATION
################################################################################
#Compiler
CPPC=icpc
#Parallel compiler
PCPPC=mpic++

#Optimization flags
CFLAGS= -std=c++11 -Wall -g
OPTFLAGS=-O2 -xAVX
CFLAGS+= -qopt-report
CMACRO=-D_FLOAT_PRECISION

#Source files dir
SRC=src
PSRC=src/parallel

#Ouput dir
ODIR=build
PODIR=build/parallel

#Library source files
LIB=lib

#Parallel library source files
PLIB=lib/parallel

#list of include folders
INCLUDE=include include/parallel $(LIB) $(PLIB)

#Name of the linked final executable
EXE=psim.x
PEXE=pscheduler_psim.x

#Create the compiler include flags (add -I)
INC_PARAMS=$(foreach d, $(INCLUDE), -I$d)

#Create arrays of cpp files as dependencies
SRC_FILES := $(wildcard $(SRC)/*.cpp)
LIB_FILES := $(wildcard $(LIB)/*.cpp)
PARALLEL_SRC_FILES := $(wildcard $(PSRC)/*.cpp)
PARALLEL_LIB_FILES := $(wildcard $(PLIB)/*.cpp)

#Create array containing the names for each different .cpp into .o
SRC_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(SRC_FILES:.cpp=.o)))
LIB_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(LIB_FILES:.cpp=.o)))
PARALLEL_SRC_OBJ_FILES := $(addprefix $(PODIR)/,$(notdir $(PARALLEL_LIB_FILES:.cpp=.o)))
PARALLEL_LIB_OBJ_FILES := $(addprefix $(PODIR)/,$(notdir $(PARALLEL_SRC_FILES:.cpp=.o)))

.PHONY: all exe pexe build clean lib plib src clean_obj debug

################################################################################
# COMPILE AND LINK
################################################################################

all: $(ODIR)/$(EXE) $(PODIR)/$(PEXE)

exe: $(ODIR)/$(EXE) clean_obj

pexe: $(PODIR)/$(PEXE) clean_obj

build:$(LIB_OBJ_FILES) $(SRC_OBJ_FILES) $(PARALLEL_LIB_OBJ_FILES)

#COMPILE PARALLEL SOURCE FILES
$(PODIR)/%.o : $(PSRC)/%.cpp
	$(PCPPC) $(INC_PARAMS) $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#COMPILER PARALLEL LIBRARY
$(PODIR)/%.o : $(PLIB)/%.cpp
	$(PCPPC) $(INC_PARAMS)  $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#COMPILE SOURCE FILES
$(ODIR)/%.o : $(SRC)/%.cpp
	$(CPPC) $(INC_PARAMS) $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#COMPILE LIBRARY
$(ODIR)/%.o : $(LIB)/%.cpp
	$(CPPC) $(INC_PARAMS)  $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#LINKING
$(ODIR)/$(EXE): $(SRC_OBJ_FILES) $(LIB_OBJ_FILES)
	$(CPPC) $^ -o $@ -lpthread

$(PODIR)/$(PEXE): $(PARALLEL_SRC_OBJ_FILES) $(PARALLEL_LIB_OBJ_FILES) $(LIB_OBJ_FILES)
	$(PCPPC) $^ -o $@ -lpthread

lib: $(LIB_OBJ_FILES)

src: $(SRC_OBJ_FILES)

plib: $(PARALLEL_LIB_OBJ_FILES)


################################################################################
# UTILITIES
################################################################################
clean:
	@rm -rf $(ODIR)/*.x $(ODIR)/*.dat $(ODIR)/*.txt $(ODIR)/*.o $(ODIR)/*.optrpt
	@rm -rf $(PODIR)/*.x $(PODIR)/*.dat $(PODIR)/*.txt $(PODIR)/*.o $(PODIR)/*.optrpt
	@rm -rf $(ODIR)/serial/* $(ODIR)/thread/* $(ODIR)/output/* $(ODIR)/output_bak/*

clean_obj:
	@rm -rf $(ODIR)/*.o $(ODIR)/*.optrpt
	@rm -rf $(PODIR)/*.o $(PODIR)/*.optrpt

debug: CFLAGS+=-g
debug: all

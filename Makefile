################################################################################
# CONFIGURATION
################################################################################
#Compiler
CPPC=icpc
#Parallel compiler
PCPPC=mpic++

#Optimization flags
CFLAGS= -std=c++11 -Wall
OPTFLAGS= -O2 -xAVX
CFLAGS+= -qopt-report
CMACRO=-D_FLOAT_PRECISION

#Source files dir
SRC=src

#Ouput dir
ODIR=build

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
PARALLEL_LIB_FILES := $(wildcard $(PLIB)/*.cpp)

#Create array containing the names for each different .cpp into .o
SRC_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(SRC_FILES:.cpp=.o)))
LIB_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(LIB_FILES:.cpp=.o)))
PARALLEL_LIB_OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(PARALLEL_LIB_FILES:.cpp=.o)))

.PHONY: all clean run test bench compile clean_obj wow

################################################################################
# COMPILE AND LINK
################################################################################

all: $(ODIR)/$(EXE) $(ODIR)/$(PEXE)

exe: $(ODIR)/$(EXE) clean_obj

pexe: $(ODIR)/$(PEXE) clean_obj

build:$(LIB_OBJ_FILES) $(SRC_OBJ_FILES) $(PARALLEL_LIB_OBJ_FILES)


#COMPILE SOURCE FILES
$(ODIR)/%.o : $(SRC)/%.cpp
	$(CPPC) $(INC_PARAMS) $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#COMPILE LIBRARY
$(ODIR)/%.o : $(LIB)/%.cpp
	$(CPPC) $(INC_PARAMS)  $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#COMPILER PARALLEL LIBRARY
$(ODIR)/%.o : $(PLIB)/%.cpp
	$(PCPPC) $(INC_PARAMS)  $(CFLAGS) $(CMACRO) $(OPTFLAGS) -c -o $@ $<

#LINKING
$(ODIR)/$(EXE): $(SRC_OBJ_FILES) $(LIB_OBJ_FILES)
	$(CPPC) $^ -o $@

$(ODIR)/$(PEXE): $(SRC_OBJ_FILES) $(LIB_OBJ_FILES) $(PARALLEL_LIB_OBJ_FILES)
	$(PCPPC) $^ -o $@

lib: $(LIB_OBJ_FILES)

src: $(SRC_OBJ_FILES)

plib: $(PARALLEL_LIB_OBJ_FILES)

run: $(ODIR)/$(EXE)
	(cd $(ODIR) && ./$(EXE))


################################################################################
# UTILITIES
################################################################################
clean:
	@rm -rf $(ODIR)/*.x $(ODIR)/*.dat $(ODIR)/*.txt $(ODIR)/*.o $(ODIR)/*.optrpt

clean_obj:
	@rm -rf $(ODIR)/*.o $(ODIR)/*.optrpt

debug: CFLAGS+=-g
debug: $(ODIR)/$(EXE)

################################################################################
# PUSH ON CLUSTER
################################################################################

PASSWORDLESS_CLUSTER_DOMAIN=ulisse

push:
	@rsync --delete -avzhe ssh --progress . $(PASSWORDLESS_CLUSTER_DOMAIN):Potts_code/

pull:
	scp $(PASSWORDLESS_CLUSTER_DOMAIN):Potts_code/bench/parameters_sweeping/*.dat bench/parameters_sweeping/

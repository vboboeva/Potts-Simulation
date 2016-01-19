################################################################################
# CONFIGURATION
################################################################################
CPPC=mpic++
CFLAGS= -std=c++11 -Wall
OPTFLAGS= -O2 -xAVX
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

.PHONY: all clean run test bench compile clean_obj docs

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


################################################################################
# UTILITIES
################################################################################
clean:
	@rm -rf $(ODIR)/*.x $(ODIR)/*.dat $(ODIR)/*.txt $(ODIR)/*.o $(ODIR)/*.optrpt

clean_obj:
	@rm -rf $(ODIR)/*.o $(ODIR)/*.optrpt

debug: CFLAGS+=-g
debug: $(ODIR)/$(EXE)

docs:
	@echo "Not implemented yet, just go inside the docs folder and run --doxygen doxy.conf--"
################################################################################
# PUSH ON CLUSTER
################################################################################

PASSWORDLESS_CLUSTER_DOMAIN=ulisse

push:
	@rsync --delete -avzhe ssh --progress . $(PASSWORDLESS_CLUSTER_DOMAIN):Potts_code/

pull:
	scp $(PASSWORDLESS_CLUSTER_DOMAIN):Potts_code/bench/parameters_sweeping/*.dat bench/parameters_sweeping/

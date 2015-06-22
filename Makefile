CPPC=g++
CFLAGS= -std=c++11 -Wall
SRC=src
BUILD=build
EXE=potts.x
.PHONY: all clean run test

all: $(BUILD)/$(EXE)

$(BUILD)/$(EXE): $(wildcard $(SRC)/*.cpp)
	$(CPPC) $(CFLAGS) -o $@ $^

run: $(BUILD)/$(EXE)
	(cd $(BUILD) && ./$(EXE))


clean:
	@rm -rf $(BUILD)/*.x $(BUILD)/*.dat

test:
	@$(MAKE) -s -C test
	@$(MAKE) -s -C test clean

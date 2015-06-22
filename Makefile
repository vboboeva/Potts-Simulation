CPPC=g++
CFLAGS= -std=c++11 -Wall
SRC=src
BUILD=build

.PHONY: all clean run test

all: $(BUILD)/potts.x

$(BUILD)/potts.x: $(wildcard $(SRC)/*.cpp)
	$(CPPC) $(CFLAGS) -o $@ $^

run: $(BUILD)/potts.x
	@./$^

clean:
	@rm -rf $(BUILD)/*.x $(BUILD)/*.dat

test:
	@$(MAKE) -s -C test
	@$(MAKE) -s -C test clean

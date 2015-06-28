CPPC=g++
CFLAGS= -std=c++11 -Wall -O3 -ftree-vectorizer-verbose=9
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
	@rm -rf $(BUILD)/*.x $(BUILD)/*.dat $(BUILD)/*.txt

debug: CFLAGS+=-g
debug: $(BUILD)/$(EXE)


test:
	@$(MAKE) -s -C test
	@$(MAKE) -s -C test clean

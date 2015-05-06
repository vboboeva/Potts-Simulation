CPPC=g++
CFLAGS= -Wall


all: generator.x main.x


generator.x : genero_pattern.cpp
	$(CPPC) $(CFLAGS) -o $@ $^

main.x : main_restyle.cpp
	$(CPPC) $(CFLAGS) -o $@ $^

generate: generator.x
	@./generator.x

profile: CFLAGS += -g -pg
profile: all

run: generate main.x
	@./main.x 1

clean:
	rm -rf *.x *.txt *.dat

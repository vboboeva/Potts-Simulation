
master.x: master.cpp
	icc -o master.x master.cpp

clean:
	@rm -rf *.out *.x *.dat *.txt

run: master.x
	@./master.x

patterns:
	@icc -O0 genero_pattern.cpp -o gen.out
	@./gen.out

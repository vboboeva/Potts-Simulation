
OLD_CODE=old_code
NEW_CODE=new_code

.PHONY: test run clean

run:
	@echo "Compiling and running new code..."
	$(MAKE) -s -C $(NEW_CODE) run
	@cp $(NEW_CODE)/patterns.dat $(OLD_CODE)/pattern.dat
	@cp $(NEW_CODE)/init_connections.dat $(OLD_CODE)/init_connections.dat 
	@echo "Compiling and running old code..."

	$(MAKE) -s -C $(OLD_CODE) run
	@./run_test.sh

test: run clean

clean:
	$(MAKE) -s -C $(OLD_CODE) clean
	$(MAKE) -s -C $(NEW_CODE) clean

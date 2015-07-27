OLD_CODE=old_code
NEW_CODE=new_code

.PHONY: compile bench clean run

.DEFAULT_GOAL := compile


compile:
	@$(MAKE) -s -C $(OLD_CODE) master.x
	@$(MAKE) -s -C $(NEW_CODE) main.x

run:
	@echo "Compiling old code..."
	@$(MAKE) -s -C $(OLD_CODE) master.x
	@echo "Compiling new code..."
	@$(MAKE) -s -C $(NEW_CODE) main.x
	@./run_bench.sh

bench: run clean

clean:
	@$(MAKE) -s -C $(OLD_CODE) clean
	@$(MAKE) -s -C $(NEW_CODE) clean

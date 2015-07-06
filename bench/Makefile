OLD_CODE=old_code
NEW_CODE=new_code

.PHONY: test clean compile

.DEFAULT_GOAL := compile


compile:
	@$(MAKE) -s -C $(OLD_CODE) master.x
	@$(MAKE) -s -C $(NEW_CODE) main.x
	
bench:
	@echo "Running old code..."
	@$(MAKE) -s -C $(OLD_CODE) master.x
	@echo "Running new code..."
	@$(MAKE) -s -C $(NEW_CODE) main.x
	@./run_bench.sh

clean:
	@$(MAKE) -s -C $(OLD_CODE) clean
	@$(MAKE) -s -C $(NEW_CODE) clean

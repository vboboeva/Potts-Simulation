
OLD_CODE=old_code
NEW_CODE=new_code

.PHONY: test

test:
	@echo "Running old code..."
	@$(MAKE) -s -C $(OLD_CODE) run
	@echo "Running new code..."
	@$(MAKE) -s -C $(NEW_CODE) run
	@./run_test.sh

clean:
	@$(MAKE) -s -C $(OLD_CODE) clean
	@$(MAKE) -s -C $(NEW_CODE) clean

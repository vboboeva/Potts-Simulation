
bench=

.PHONY: bench clean


bench:
ifdef bench
	@$(MAKE) -s -C $(bench) bench
else
	@echo "Error, define bench type es. 'make bench=standard'"
endif

clean:
	@$(MAKE) -s -C parameters_sweeping clean
	@$(MAKE) -s -C standard clean

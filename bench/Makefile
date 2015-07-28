bench=

.PHONY: bench


bench:
ifdef bench
	@$(MAKE) -s -C $(bench) bench
else
	@echo "Error, define bench type es. 'make bench=standard'"
endif

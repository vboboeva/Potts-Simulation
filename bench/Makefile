bench=

.PHONY: bench clean


bench:
ifdef bench
	@$(MAKE) -s -C $(bench) bench
else
	@echo "Error, define bench type es. 'make bench=standard'"
endif

clean:
ifdef bench
	@$(MAKE) -s -C $(bench) clean
else
	@echo "Error, define bench type es. 'make clean bench=standard'"
endif

LC=lc
HC=hc
VLC=vlc
OLD_CODE=old_code
OLD_CODE_BUFFER=old_code_buffer
.PHONY: compile bench clean

.DEFAULT_GOAL := compile


compile:
	@$(MAKE) -s -C $(OLD_CODE) master.x
	@$(MAKE) -s -C $(LC) main.x
	@$(MAKE) -s -C $(VLC) main.x
	@$(MAKE) -s -C $(HC) main.x

bench: compile
	@echo "Starting benchmark..."
	@./run_bench.sh

clean:
	@$(MAKE) -s -C $(OLD_CODE) clean
	@$(MAKE) -s -C $(LC) clean
	@$(MAKE) -s -C $(VLC) clean
	@$(MAKE) -s -C $(HC) clean
	@$(MAKE) -s -C $(OLD_CODE_BUFFER) clean
	@rm -f *.dat *.png

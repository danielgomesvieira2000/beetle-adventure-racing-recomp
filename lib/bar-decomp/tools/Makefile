#*************************************************************
#
#	A very simple Makefile for Copymask and other tools
#
#	TODO
#
#*************************************************************




default:# torch
	@$(CC) n64crc.c -o n64crc
	@make  -C ido-static-recomp/ setup
	@make  -C ido-static-recomp/ VERSION=5.3
	@$(CC) moduleTool.c -o moduleTool
	@make -C daisybox dependencies
	@make -C daisybox
	@make -C daisybox genForm0
	@cp daisybox/genForm0 .

	@printf "done\n"

torch:
	@echo "Building torch..."
	$(MAKE) -C Torch type=release -j$(N_THREADS)

clean:
	rm n64crc

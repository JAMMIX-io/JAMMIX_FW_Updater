SDCC=sdcc
CPU=z80
CODE=os
OBJ=os.rel
DATALOC=0xC000

.SECONDARY:

all: $(CODE).bin

%.rel: %.c
	$(SDCC) -m$(CPU) -c --data-loc $(DATALOC) $<
	
%.ihx: $(OBJ)
	$(SDCC) -m$(CPU) --data-loc $(DATALOC) $(OBJ)
 
%.hex: %.ihx
	mv $< $@

%.bin: %.hex
	srec_cat $< -intel -o $@ -binary
	rm *.hex
	od -An -t x1 -v ../src/os.bin > ../verilator/rom.hex
	od -An -t x1 -v ../src/os.bin > ../rtl/rom.hex
	od -An -t x1 -v ../MiSTer.pf > ../verilator/font.hex
	od -An -t x1 -v ../MiSTer.pf > ../rtl/font.hex
	sshpass -p '1' scp os.bin root@192.168.0.76:/media/fat/JAMMIX_FW

disasm: $(CODE).bin
	z80dasm -a -t -g 0 $<

clean:
	rm -rf *~ *.asm *.ihx *.lk *.lst *.map *.noi *.rel *.sym *.hex *.bin

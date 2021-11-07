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
	#srec_cat os.bin -binary -output os.mif −Memory_Initialization_File
	#cp os.mif ../rtl/
	#./bin2hex os.bin os.hex > os.hex
	#srec_cat os.bin -binary -output os.hex -Ascii-Hex
	#unix2dos os.hex
	xxd -ps os.bin os.hex
	cp os.hex ../rtl/
	sshpass -p '1' scp os.bin root@192.168.0.76:/media/fat/JAMMIX_FW

disasm: $(CODE).bin
	z80dasm -a -t -g 0 $<

clean:
	rm -rf *~ *.asm *.ihx *.lk *.lst *.map *.noi *.rel *.sym *.hex *.bin

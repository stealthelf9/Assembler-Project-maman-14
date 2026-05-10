CFLAGS = -Wall -ansi -pedantic
OBJS = assembler.o pre_assembler.o first_pass.o second_pass.o macro_helpers.o symbol_helpers.o utils.o structs.o

assembler: $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o assembler


# Compile the object files
assembler.o: assembler.c assembler.h macro_helpers.h symbol_helpers.h structs.h first_pass.h second_pass.h utils.h
	gcc $(CFLAGS) -c assembler.c -o assembler.o

pre_assembler.o: pre_assembler.c pre_assembler.h macro_helpers.h symbol_helpers.h structs.h
	gcc $(CFLAGS) -c pre_assembler.c -o pre_assembler.o

first_pass.o: first_pass.c first_pass.h symbol_helpers.h structs.h utils.h
	gcc $(CFLAGS) -c first_pass.c -o first_pass.o

second_pass.o: second_pass.c second_pass.h symbol_helpers.h structs.h utils.h
	gcc $(CFLAGS) -c second_pass.c -o second_pass.o

structs.o: structs.c structs.h
	gcc $(CFLAGS) -c structs.c -o structs.o

macro_helpers.o: macro_helpers.c macro_helpers.h symbol_helpers.h structs.h
	gcc $(CFLAGS) -c macro_helpers.c -o macro_helpers.o

symbol_helpers.o: symbol_helpers.c symbol_helpers.h structs.h
	gcc $(CFLAGS) -c symbol_helpers.c -o symbol_helpers.o

utils.o: utils.c utils.h structs.h
	gcc $(CFLAGS) -c utils.c -o utils.o

clean:
	rm *.o assembler
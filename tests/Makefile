CC = clang
CFLAGS = -g -fwasmsbx -DWASM_SBX -fPIE
LDFLAGS = -L/home/arun/Desktop/WASM_SBX -lSBX_CON_LIB -lisc_lib_final -ldl -lstdc++
TARGETS = callback function_ptr assign_value string_concat array_access pointer_arithmetic file_operations dynamic_memory

# The source files
SRC = assign_value.c string_concat.c array_access.c pointer_arithmetic.c file_operations.c dynamic_memory.c fp.c callback.c

# The object files
OBJ = $(SRC:.c=.o)

all: $(TARGETS)

assign_value: assign_value.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

string_concat: string_concat.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

array_access: array_access.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

pointer_arithmetic: pointer_arithmetic.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

file_operations: file_operations.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

dynamic_memory: dynamic_memory.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

function_ptr: fp.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

callback: callback.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGETS)

.PHONY: all clean


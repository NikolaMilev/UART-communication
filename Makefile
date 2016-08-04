PROGRAM = uart_echo
CPP_FILES = $(wildcard *.c)
OBJ_FILES = $(notdir $(CPP_FILES:.c=.o))
CC_FLAGS = -Wall

$(PROGRAM): $(OBJ_FILES)
	gcc $(LD_FLAGS) -o $@ $^

%.o: %.c
	gcc $(CC_FLAGS) -c -o $@ $<

PHONY: .clean

clean:
	rm -f *.o $(PROGRAM)
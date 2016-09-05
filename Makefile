#Assumes that every .c file in the directory is to be compiled into one program
#left it as general as possible

PROGRAM = uart_echo_1
CC_FILES = $(wildcard *.c)
CC_FLAGS = -Wall

all: $(PROGRAM)

$(PROGRAM): $(CC_FILES)
	$(CC) $(CC_FLAGS) -o $@ $^

PHONY: .clean

clean:
	rm -f *.o *.h.gch $(PROGRAM)

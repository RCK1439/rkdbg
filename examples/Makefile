CC = cc

RAYLIB_DIR = raylib
RAYLIB_INCL = $(RAYLIB_DIR)/include
RAYLIB_LIB = $(RAYLIB_DIR)/lib/linux

.PHONY: all clean

all:
	$(CC) example.c -o example -I$(RAYLIB_INCL) -L$(RAYLIB_LIB) -lraylib -lm

clean:
	rm -f example
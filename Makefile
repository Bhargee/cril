CC=clang
CFLAGS= -Wall -Wextra -pedantic -g
RELEASE= -Ofast
DEBUG= -O1 -g -fsanitize=address -fno-omit-frame-pointer
OBJECTS=parson.o table.o interp.o main.o
TARGET=interp

all: $(TARGET)

debug: $(OBJECTS)
	$(CC) $(CFLAGS) $(DEBUG) -o interp $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(RELEASE) -o interp $(OBJECTS)
	rm $(OBJECTS)

parson.o: deps/parson.c deps/parson.h
	$(CC) $(CFLAGS) -c deps/parson.c

table.o: src/table.h src/table.c
	$(CC) $(CFLAGS) -c src/table.c

interp.o: src/interp.c
	$(CC) $(CFLAGS) -c src/interp.c

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c


.PHONY: clean
clean:
	-@rm interp 2>/dev/null || true
	-@rm $(OBJECTS) 2>/dev/null || true

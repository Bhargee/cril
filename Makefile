CC=clang
CFLAGS= -Wall -Wextra -pedantic
RELEASE= -Ofast
DEBUG= -O1 -g -fsanitize=address -fno-omit-frame-pointer
OBJECTS=parson.o table.o interp.o main.o
TARGET=cril

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(RELEASE) -o $(TARGET) $(OBJECTS)
	rm $(OBJECTS)

parson.o: deps/parson.c deps/parson.h
	$(CC) $(CFLAGS) $(RELEASE) -c deps/parson.c

table.o: src/table.h src/table.c
	$(CC) $(CFLAGS) $(RELEASE) -c src/table.c

interp.o: src/interp.c
	$(CC) $(CFLAGS) $(RELEASE) -c src/interp.c

main.o: src/main.c
	$(CC) $(CFLAGS) $(RELEASE) -c src/main.c -lm


.PHONY: clean
clean:
	-@rm interp 2>/dev/null || true
	-@rm $(OBJECTS) 2>/dev/null || true

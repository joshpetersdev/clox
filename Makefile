CC = clang
CFLAGS = -Wall -Wextra -std=c11

TARGET = main
SOURCES = $(wildcard *.c) 
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGETS)

.PHONY: clean
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = imcsh
SOURCES = imcsh.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

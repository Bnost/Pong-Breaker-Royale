CC = gcc
TARGET = pong_breaker_royale
SRCS = main.c character.c
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS = -O2 -Wall

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(LIBS) $(CFLAGS)

oyna: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all oyna clean

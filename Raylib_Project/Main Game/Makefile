CC = gcc
TARGET = pong_breaker_royale
SRCS = main.c character.c
CFLAGS = -O2 -Wall

ifeq ($(OS),Windows_NT)
    LIBS = -lraylib -lgdi32 -lwinmm -lpthread -lm
    EXEC = $(TARGET).exe
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = $(TARGET)
endif

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(SRCS) -o $(EXEC) $(LIBS) $(CFLAGS)

oyna: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(EXEC)

.PHONY: all oyna clean

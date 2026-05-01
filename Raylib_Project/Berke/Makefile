CC = gcc
CFLAGS = -Wall -std=c99 -O2 -I../İbrahim
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lGL -lX11 -lXrandr -lXi -lXxf86vm -lXinerama -lXcursor -lasound


SRC = main.c ../İbrahim/character.c
OBJ = $(SRC:.c=.o)

TARGET = pong_breaker_royale

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

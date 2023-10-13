CC := g++
CFLAGS := -Wall -Werror -std=c11
SRC_DIR := src

run : app.exe

app.exe : $(SRC_DIR)/main.c
	$(CC) $(SRC_DIR)/main.c -o app.exe

clean:
	@rm app.exe

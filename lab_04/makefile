CC := gcc
CFLAGS := -Wall -Werror -std=c17
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := out

app.exe : $(OBJ_DIR) $(OBJ_DIR)/main.o $(OBJ_DIR)/sha1.o $(OBJ_DIR)/rsa.o
	$(CC) $(OBJ_DIR)/*.o -o app.exe

$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/main.o : $(SRC_DIR)/main.c $(INC_DIR)/sha1.h $(INC_DIR)/rsa.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/main.c -c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/sha1.o : $(SRC_DIR)/sha1.c $(INC_DIR)/sha1.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/sha1.c -c -o $(OBJ_DIR)/sha1.o

$(OBJ_DIR)/rsa.o : $(SRC_DIR)/rsa.c $(INC_DIR)/rsa.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/rsa.c -c -o $(OBJ_DIR)/rsa.o

.PHONY : clean

clean :
	rm -rf $(OBJ_DIR) app.exe

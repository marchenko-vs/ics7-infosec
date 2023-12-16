CC := g++
CFLAGS := -Wall -Werror -std=c++20
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := out

run : app.exe make_func.sh
	@bash make_func.sh

app.exe : $(OBJ_DIR) $(OBJ_DIR)/main.o $(OBJ_DIR)/enigma.o $(OBJ_DIR)/reflector.o $(OBJ_DIR)/rotor.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/steckerbrett.o
	$(CC) $(OBJ_DIR)/*.o -o app.exe

$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/main.o : $(SRC_DIR)/main.cpp $(INC_DIR)/enigma.h $(INC_DIR)/reflector.h $(INC_DIR)/steckerbrett.h $(INC_DIR)/utils.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/main.cpp -c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/enigma.o : $(SRC_DIR)/enigma.cpp $(INC_DIR)/rotor.h $(INC_DIR)/reflector.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/enigma.cpp -c -o $(OBJ_DIR)/enigma.o

$(OBJ_DIR)/reflector.o : $(SRC_DIR)/reflector.cpp $(INC_DIR)/rotor.h $(INC_DIR)/reflector.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/reflector.cpp -c -o $(OBJ_DIR)/reflector.o

$(OBJ_DIR)/rotor.o : $(SRC_DIR)/rotor.cpp $(INC_DIR)/rotor.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/rotor.cpp -c -o $(OBJ_DIR)/rotor.o

$(OBJ_DIR)/utils.o : $(SRC_DIR)/utils.cpp $(INC_DIR)/utils.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/utils.cpp -c -o $(OBJ_DIR)/utils.o

$(OBJ_DIR)/steckerbrett.o : $(SRC_DIR)/steckerbrett.cpp $(INC_DIR)/steckerbrett.h
	$(CC) $(CFLAGS) -I $(INC_DIR) $(SRC_DIR)/steckerbrett.cpp -c -o $(OBJ_DIR)/steckerbrett.o

.PHONY : clean

clean :
	rm -rf $(OBJ_DIR) *.txt *.gcov app.exe

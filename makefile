
CC = gcc
CFLAGS = -Wall -Werror -Wconversion -std=c99
DEPS = utiles.o animos.c defendiendo_torres.c 
GAME_FILES = juego.c
TEST= test.c
GAME_NAME = juego

clear:
	clear

game: clear $(DEPS) $(GAME_FILES)
	$(CC) $(DEPS) $(GAME_FILES) $(CFLAGS) -o $(GAME_NAME)

test: clear $(DEPS) $(TEST)
	$(CC) $(DEPS) $(TEST) -o $(GAME_NAME)

run: $(GAME_NAME)
	./$(GAME_NAME)


zip: $(DEPS) $(GAME_FILES) $(TEST)
	zip TP2 $(DEPS) $(GAME_FILES) $(TEST) makefile

do: clear game run zip

rec: clear
	asciinema rec




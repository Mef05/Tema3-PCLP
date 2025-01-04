CC = gcc
FLAGS = -Wall -O0 -Werror
SRC = main.c
EXE = main

all: run clean

val: debug clean

run: build 
	@echo "Running"
	@./$(EXE) echo "input.txt"

build:
	@echo "Comp"
	@$(CC) -o $(EXE) $(SRC) $(FLAGS) -lm


.PHONY : clean
clean :
	@rm -f $(EXE) *~

view: build 
	@echo "Running"
	@./$(EXE) > output.txt
	@echo "Building .txt"
	# @rm -f $(EXE) *~

debug: build
	@echo "Running Valgrind"
	@valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./$(EXE)
all: compile run clean

compile:
	g++ create_fs.c create_fs 
	g++ code.cpp -o code.
run:
	./create_fs disk0
	./code input.txt

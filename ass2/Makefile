run:
	gcc -c copytree.c -o copytree.o
	ar rcs libcopytree.a copytree.o
	gcc part4.c -L. -lcopytree -o main_program

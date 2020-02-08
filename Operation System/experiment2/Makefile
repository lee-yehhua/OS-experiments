all:
	nasm -f elf64 my_print.asm
	g++ -m64 -o run main.cpp my_print.o
	./run

clean:
	rm -f my_print.o
	rm -f run

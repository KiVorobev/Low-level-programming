main: main.o dict.o lib.o
		ld -o main main.o dict.o lib.o

main.o: main.asm colon.inc words.inc
		nasm -f elf64 main.asm

lib.o: lib.asm 
		nasm -f elf64 lib.asm

dict.o: dict.asm 
		nasm -f elf64 dict.asm

clean:
		rm -f main.o lib.o dict.o main

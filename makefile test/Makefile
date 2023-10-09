heap.o:
	gcc -c heap.c

main.o:
	gcc -c main.c

link: main.o heap.o
	gcc heap.o main.o -o final.exe

clear:
	rm heap.o main.o final.exe
a.out: main.o operation.o
	gcc main.o operation.o -o a.out
main.o: main.c
	gcc -c main.c
operation.o: operation.c
	gcc -c operation.c
clean:
	rm *.o a.out

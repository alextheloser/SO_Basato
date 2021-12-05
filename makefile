SpaceDefender: navicella.o main.o
	gcc -o SpaceDefender navicella.o main.o

navicella.o: navicella.c navicella.h
	gcc -c -o navicella.o navicella.c

main.o: main.c navicella.h
	gcc -c -o main.o main.c

clean:
	rm *.o
	rm SpaceDefender

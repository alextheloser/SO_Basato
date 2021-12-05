SpaceDefender: main.o navicella.o
	gcc -o SpaceDefender main.o navicella.o

navicella.o: navicella.c navicella.h
	gcc -c -o navicella.o navicella.c

main.o: main.c navicella.h
	gcc -c -o main.o main.c

clean:
	rm *.o
	rm SpaceDefender

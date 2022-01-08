SpaceDefender: main.o controllo.o nemici.o navicella.o
	gcc -o SpaceDefender main.o controllo.o nemici.o navicella.o -lncurses

main.o: main.c
	gcc -c -o main.o main.c

navicella.o: navicella.c navicella.h
	gcc -c -o navicella.o navicella.c

nemici.o: nemici.c nemici.h
	gcc -c -o nemici.o nemici.c

controllo.o: controllo.c controllo.h
	gcc -c -o controllo.o controllo.c

clean:
	rm *.o
	rm SpaceDefender

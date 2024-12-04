progetto: main.o collisioni.o stampa.o gestione.o
	gcc main.o collisioni.o stampa.o gestione.o -o progetto -lncurses -lpthread

main.o: stampa.h gestione.h main.c
	gcc -c main.c

gestione.o: globale.h gestione.h gestione.c 
	gcc -c gestione.c

stampa.o: globale.h collisioni.h stampa.h stampa.c
	gcc -c stampa.c

collisioni.o: globale.h gestione.h stampa.h collisioni.h collisioni.c
	gcc -c collisioni.c

clean:
	rm -f *.o progetto

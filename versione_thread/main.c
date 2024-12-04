#include "gestione.h"
#include "stampa.h"

int main(){
    pthread_t thread_astronave, thread_crea_navicelle;
	
    /* inizializzo semafori e mutex */
    sem_init(&semph_underflow, 0, 0);
    sem_init(&semph_overflow, 0, DIM_BUFFER);
    pthread_mutex_init(&mutex, NULL);
        
    /* inizializzazione delle variabili globali dei contatori per la lettura e la scrittura */
    in = 0;
    out = 0;
	
	/* inizializzo schermo */
    initscr();
    noecho();
    curs_set(false);
    keypad(stdscr, true);
    start_color();
	
	/* definisco le coppie di colori da utilizzare */
    init_pair(COLORE_BOMBE, COLOR_RED, COLOR_BLACK);
    init_pair(COLORE_NAVICELLE, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLORE_NAVICELLE_1UP, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLORE_ASTRONAVE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLORE_MISSILI, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLORE_PUNTEGGIO, COLOR_WHITE, COLOR_BLACK);


    pthread_mutex_lock(&mutex);
    getmaxyx(stdscr, maxy, maxx); //dimensione schermo
    pthread_mutex_unlock(&mutex);
    

    timeout(1);
    flushinp(); //pulisce il buffer input 

    /* crea thread per la gestione di astronave e navicelle */
    pthread_create(&thread_astronave, NULL, creaAstronave, NULL);
    pthread_create(&thread_crea_navicelle, NULL, navicelleLiv1, NULL);

    /* assegno stampaGioco al main thread */
    stampaGioco();

    pthread_join(thread_crea_navicelle, NULL);
    pthread_join(thread_astronave, NULL);

	/* cancello semafori e mutex */
    sem_destroy(&semph_underflow);
    sem_destroy(&semph_overflow);

    pthread_mutex_destroy(&mutex);
	
    endwin();
    return 0;
}

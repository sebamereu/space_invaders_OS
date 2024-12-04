#ifndef GESTIONE_H
#define GESTIONE_H

#include "globale.h"

/* definizione della struttura per il passaggio dei dati alla funzione per la generazione del missile */
typedef struct thread_missile_param{
    int id_missile;
    int posizione_y;
    Posizione missile;
}Thread_missile_param;

/* richiamo alle variabili globali */
extern Posizione buffer[DIM_BUFFER]; 
extern int in, out;

extern sem_t semph_overflow, semph_underflow;
extern pthread_mutex_t mutex;

extern _Bool collisioni_missile_dw[DIM_STAMPA_MISSILI], collisioni_missile_up[DIM_STAMPA_MISSILI], collisioni_navicelle[N_NAVICELLE], 
collisioni_navicelle_liv2[N_NAVICELLE], collisioni_bombe[DIM_STAMPA_BOMBE], collisioni_astronave;


/* dichiarazioni delle funzioni per i produttori */
void writeMessage(Posizione messaggio);

void *creaAstronave(void *arg);
void *movimentoMissile(void *arg);
void *movimentoBomba(void *arg);
void *movimentoNavicella(void *arg);

void *navicelleLiv1(void *arg);
void navicelleLiv2(Posizione *navicella);




#endif

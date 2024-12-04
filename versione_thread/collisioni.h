#ifndef COLLISIONI_H
#define COLLISIONI_H

#include "globale.h"
#include "gestione.h"
#include "stampa.h"

/* richiamo alle variabili globali */
extern Posizione buffer[DIM_BUFFER]; 
extern int in, out;

extern sem_t semph_overflow, semph_underflow;
extern pthread_mutex_t mutex;

extern _Bool collisioni_missile_dw[DIM_STAMPA_MISSILI], collisioni_missile_up[DIM_STAMPA_MISSILI], collisioni_navicelle[N_NAVICELLE], 
collisioni_navicelle_liv2[N_NAVICELLE], collisioni_bombe[DIM_STAMPA_BOMBE], collisioni_astronave;


/* dichiarazioni delle funzioni per la gestione delle collisioni */
void inizializzaVettoriCollisioni();

void collisioneMissiliNavicelle(Posizione missili[], Posizione navicelle[], int n_vite[], Posizione navicelle_liv2[]);
void collisioneNavicelle(Posizione navicelle[], int n_vite[]);
void collisioneBombaMissile(Posizione bomba[], Posizione missile[], int *n_vite);
void collisioniBombaAstronave(Posizione bombe[], Posizione astronave, int *n_vite);

_Bool collisioneNavicellaAstronave(Posizione astronave, Posizione navicelle[]);
_Bool collisioneNavicellaBordo(Posizione navicelle[]);
_Bool quadroTerminato(int n_vite_navicelle[]);
_Bool astronaveMorta(int n_vite_astronave);
_Bool fineGioco(Posizione astronave, Posizione navicelle[], int n_vite_navicelle[], int n_vite_astronave);



#endif

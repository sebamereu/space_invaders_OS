#ifndef CONSUMATORE_GRAFICA_H
#define CONSUMATORE_GRAFICA_H

#include "globale.h"
#include "collisioni.h"

_Bool flag_navicelle[N_NAVICELLE];  //flag movimento navicelle

/* richiamo alle variabili globali */
extern Posizione buffer[DIM_BUFFER]; 
extern int in, out;

extern sem_t semph_overflow, semph_underflow;
extern pthread_mutex_t mutex;


/* dichiarazioni delle funzioni per la gestione della grafica */
Posizione leggiMessaggio();

void stampaGioco();

void stampaAstronave(Posizione valore_letto, Posizione *astronave);
void stampaMissile(Posizione valore_letto, Posizione missile[]);
void stampaNavicelle(Posizione valore_letto, Posizione navicelle[], int n_vite[]);
void stampaMissile(Posizione valore_letto, Posizione missile[]);
void stampaBomba(Posizione valore_letto, Posizione bombe[]);

void dimensione_schermo();
int get_center_x(int x);
int get_center_y(int y);

void stampaVite(int n_vite_astronave);


#endif

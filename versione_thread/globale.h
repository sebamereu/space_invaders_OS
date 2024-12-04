#ifndef GLOBALE_H
#define GLOBALE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>


#define COLORE_BOMBE 1
#define COLORE_NAVICELLE 2
#define COLORE_NAVICELLE_1UP 3
#define COLORE_ASTRONAVE 4
#define COLORE_MISSILI 5
#define COLORE_PUNTEGGIO 6

#define BORDO 75

#define PUNTEGGIO_NAV_L1 30                 //punteggio navicella livello 1 
#define PUNTEGGIO_NAV_L2 20                 //punteggio navicella livello 2 con due vite
#define PUNTEGGIO_NAV_L2_L 10               //punteggio navicella livello 2 con una vita

#define MINY 1                              //minimo di y
#define MINX 30                              //minimo di x
#define MIN_GEN_MOV 0                       //minimo per generazione casuale 
#define MAX_GEN_MOV 100                     //massimo per generazione casuale
#define MIN_GEN_BOMBA 45                    //minimo per la generazione della bomba  
#define MAX_GEN_BOMBA 80                    //massimo per la generazione della bomba
#define MIN_GEN_DISTANZA 0                  //minimo per la generazione della distanza

#define BARRA_SPAZIO 32                     //ascii della barra spaziatrice

#define DIM_SPRITEY_ASTRONAVE 2  			//dimensione dello sprite astronave nell'asse y
#define DIM_SPRITE_ASTRONAVE 4              //dimensione dello sprite astronave
#define DIM_SPRITE_NAVICELLA_LIV1 3         //dimensione dello sprite navicella lvl 1
#define DIM_SPRITE_NAVICELLA_LIV2 7        //dimensione dello sprite navicella lvl 2

#define CICLI_DISCESA 60                    //cicli d'attesa per la discesa delle navicelle
#define CICLI_MISSILI 900                   //cicli d'attesa per lo sparo dei missili
#define DIMENSIONE_COLONNA 3                   //dimensione di una riga di navicelle
#define POS_COLONNA_1 25                        //coordinata x della colonna 1
#define POS_COLONNA_2 15                        //coordinata x della colonna 2
#define POS_COLONNA_3 5                        //coordinata x della colonna 3

#define N_NAV_PER_COLONNA 2                    //numero di navicelle per colonna    
#define N_NAVICELLE 4                      //numero delle navicelle di livello 1   
#define DIM_STAMPA_MISSILI 20               //numero di celle per stampare i missili
#define DIM_STAMPA_BOMBE 20                 //numero di celle per stampare le bombe
#define DIM_RECORD_PUNTEGGI 6               //dimensione del vettore per mantenere i punteggi

#define N_VITE_NAV_LIV2 2                   //numero di vite delle navicelle di livello 2
#define N_VITE_ASTRONAVE 3                  //numero di vite dell'astronave

#define ID_ASTRONAVE 0                      //id assegnato all'entita' astronave         
#define ID_MISSILE_DW 1                     //id missile destro
#define ID_MISSILE_UP 2                     //id missile sinistro
#define ID_NAVICELLA_LIV1 3                 //id navicella lvl 1
#define ID_BOMBA 4                          //id bomba navicella
#define ID_NAVICELLA_LIV2 5                 //id navicella lvl 2

#define DIM_BUFFER 100                      //dimensione del buffer


#define MOV_STELLE 60

typedef struct posizione{
    int cont;
    int id;
    int x;
    int y;
} Posizione;


/* dichiarazione delle variabili globali */
int maxx, maxy;                             //variabili per la gestione delle dimensioni dello schermo 

Posizione buffer[DIM_BUFFER];               //buffer di tipo posizione
int in, out;

sem_t semph_overflow, semph_underflow;      //semafori per il controllo di overflow e underflow
pthread_mutex_t mutex;                      //mutex per la gestione delle sezioni critiche

/* vettori per il controllo delle collisioni */
_Bool collisioni_missile_dw[DIM_STAMPA_MISSILI], collisioni_missile_up[DIM_STAMPA_MISSILI], collisioni_navicelle[N_NAVICELLE], 
collisioni_navicelle_liv2[N_NAVICELLE], collisioni_bombe[DIM_STAMPA_BOMBE], collisioni_astronave;


#endif

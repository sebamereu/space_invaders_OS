#ifndef GENERALE_H
#define GENERALE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DELAY_NAVICELLA 170000              //delay della navicella 
#define BORDO 68

#define COLORE_BOMBE 1
#define COLORE_NAVICELLE 2
#define COLORE_NAVICELLE_1UP 3
#define COLORE_ASTRONAVE 4
#define COLORE_MISSILI 5
#define COLORE_PUNTEGGIO 6

#define PUNTEGGIO_NAV_L1 30                 //punteggio navicella livello 1 
#define PUNTEGGIO_NAV_L2 20                 //punteggio navicella livello 2 con due vite
#define PUNTEGGIO_NAV_L2_L 10               //punteggio navicella livello 2 con una vita

#define	MAXX 71				    //massimo di x
#define MAXY 30				    //massimo di y
#define MINY 0                              //minimo di y
#define MINX 30                             //minimo di x
#define MIN_GEN_MOV 0                       //minimo per generazione casuale 
#define MAX_GEN_MOV 100                     //massimo per generazione casuale
#define MIN_GEN_BOMBA 45                    //minimo per la generazione della bomba  
#define MAX_GEN_BOMBA 80                    //massimo per la generazione della bomba
#define MIN_GEN_DISTANZA 0                  //minimo per la generazione della distanza

#define BARRA_SPAZIO 32                     //ascii della barra spaziatrice

#define DIM_SPRITEY_ASTRONAVE 2   	    //dimensione verticale dello sprite astronave
#define DIM_SPRITE_ASTRONAVE 4              //dimensione dello sprite astronave
#define DIM_SPRITE_NAVICELLA_LIV1 3         //dimensione dello sprite navicella livello 1
#define DIM_SPRITE_NAVICELLA_LIV2 7         //dimensione dello sprite navicella livello 2

#define CICLI_DISCESA 80                    //cicli d'attesa per lo scorrimento verso sinistra delle navicelle
#define CICLI_MISSILI 450                   //cicli d'attesa per lo sparo dei missili
#define DIMENSIONE_COLONNA 3                //dimensione di una colonna di navicelle
#define POS_COLONNA_1 20                    //coordinata x della colonna 1
#define POS_COLONNA_2 10                    //coordinata x della colonna 2
#define POS_COLONNA_3 0                     //coordinata x della colonna 3

#define INIZIO_ASTRONAVE 25                 //posizione iniziale astronave

#define N_NAV_PER_COLONNA 2                 //numero di navicelle per colonna    
#define N_NAVICELLE 4                       //numero delle navicelle di livello 1   
#define DIM_STAMPA_MISSILI 20               //numero di celle per stampare i missili
#define DIM_STAMPA_BOMBE 20                 //numero di celle per stampare le bombe

#define N_VITE_NAV_LIV2 2                   //numero di vite delle navicelle di livello 2
#define N_VITE_ASTRONAVE 3                  //numero di vite dell'astronave

#define ID_ASTRONAVE 0                      //id assegnato all'entita' astronave         
#define ID_MISSILE_DW 1                     //id missile down
#define ID_MISSILE_UP 2                     //id missile up
#define ID_NAVICELLA_LIV1 3                 //id navicella livello 1
#define ID_BOMBA 4                          //id bomba navicella
#define ID_NAVICELLA_LIV2 5                 //id navicella livello 2

#define MOV_STELLE 60

int maxx, maxy;


typedef struct posizione{
    int pid;              //prende il pid del processo
    int cont;             //conta quanti elementi sono stati creati
    int id;
    int x;		  //coordinata x dell'elemento
    int y;		  //coordinata y dell'elemento
} Posizione;

#endif

#ifndef COLLISIONI_H
#define COLLISIONI_H

#include "globale.h"
#include "gestione.h"
#include "stampa.h"

/* dichiarazioni delle funzioni per la gestione delle collisioni */
void collisioneMissiliNavicelle(int pipeout, Posizione missili[], Posizione navicelle[], int n_vite[], int *punteggio);
void collisioniBombaAstronave(Posizione bombe[], Posizione astronave, int *n_vite);
void collisioneBombaMissile(Posizione bomba[], Posizione missile[], int *n_vite);
void collisioneNavicelle(Posizione navicelle[], int n_vite[]);

_Bool collisioneNavicellaAstronave(Posizione astronave, Posizione navicelle[]);
_Bool collisioneNavicellaBordo(Posizione navicelle[]);
_Bool quadroTerminato(int n_vite_navicelle[]);
_Bool astronaveMorta(int n_vite_astronave);
_Bool fineGioco(Posizione astronave, Posizione navicelle[], int n_vite_navicelle[], int n_vite_astronave);

#endif

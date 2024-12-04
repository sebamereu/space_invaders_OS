#ifndef PRODUTTORI_H
#define PRODUTTORI_H

#include "globale.h"

/* dichiarazioni delle funzioni per i produttori */
void creaAstronave(int pipepout);
void movimentoMissile(int missile_id, int pipeout, Posizione astronave, Posizione missile);
void movimentoNavicella(int pipepout, Posizione navicella);
void movimentoBomba(int pipeout, Posizione navicella, Posizione bomba);
void navicelleLiv1(int pipeout);
void navicellaLiv2(int pipeout, Posizione navicella);

#endif

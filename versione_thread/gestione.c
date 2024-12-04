#include "gestione.h"

/*************************************************************************************************
    scrive messaggio sul buffer

    parametri: 
                messaggio -> messaggio da scrivere sul buffer
*************************************************************************************************/
void writeMessage(Posizione messaggio){
    
    /* controllo su overflow */
    sem_wait(&semph_overflow);
    
    pthread_mutex_lock(&mutex); //inizio sezione critica
    /* scrivo messaggio sul buffer */
    buffer[in] = messaggio;

    /* incremento contatore */
    in = (in + 1) % DIM_BUFFER;
    pthread_mutex_unlock(&mutex); //fine sezione critica

    /* segnala un'aggiunta */
    sem_post(&semph_underflow);
}

/*************************************************************************************************
    controlla il movimento dell'astronave e scrive le coordinate sul buffer. Inoltre, genera il 
	thread missili destro e sinistro in seguito alla pressione della barra spaziatrice

    parametri: 
                *arg -> argomento della funzione thread
*************************************************************************************************/
void *creaAstronave(void *arg){
    int c, timer_missile = 0;
    Posizione astronave, missile;
    Thread_missile_param param_missile_dw[DIM_STAMPA_MISSILI], param_missile_up[DIM_STAMPA_MISSILI];
    pthread_t thread_missile_dw, thread_missile_up;

    //inizializzo la variabile astronave
    astronave.id = 0;
	astronave.x = maxx/2 - 25;
	astronave.y = maxy/2;
    missile.cont = 0;

    writeMessage(astronave);

    while(true) {

        /* se il risultato e' diverso da 0 non incrementa per dare la possibilita' di sparare un missile */
        if(timer_missile % CICLI_MISSILI != 0)
            timer_missile++;

        /* controllo sulla variabile bool per la terminazione del thread */
        if(collisioni_astronave == true)
            pthread_exit(NULL);
 
        /* imposto il limite dello schermo secondo la dimensione dello sprite */
        if(astronave.y >= maxy - DIM_SPRITE_ASTRONAVE)
            astronave.y = maxy - DIM_SPRITE_ASTRONAVE;
        
        pthread_mutex_lock(&mutex);
        c = getch();
        pthread_mutex_unlock(&mutex);

        switch(c) {
            /* nel caso si prema la freccia sinistra, l'astronave fa un passo in meno sull'asse delle x */
            case KEY_UP:
                if(astronave.y > MINY+DIM_SPRITE_ASTRONAVE-1)
                    astronave.y -= 1;
                break;

            /* nel caso si prema la freccia sinistra, l'astronave fa un passo in piu' sull'asse delle x */
            case KEY_DOWN:
                if(astronave.y < maxy)
                    astronave.y += 1;
                break;
            
            /* nel caso si prema la barra spaziatrice, vengono creati due processi per la gestione dei missili */
            case BARRA_SPAZIO:
                /* se il risultato e' 0 allora spara */
                if(timer_missile % CICLI_MISSILI == 0){
                    param_missile_dw[missile.cont % DIM_STAMPA_MISSILI].id_missile = ID_MISSILE_DW;
                    param_missile_dw[missile.cont % DIM_STAMPA_MISSILI].posizione_y = astronave.y;
                    param_missile_dw[missile.cont % DIM_STAMPA_MISSILI].missile = missile;

                    /* crea il thread per il missile destro */
                    pthread_create(&thread_missile_dw, NULL, movimentoMissile, &param_missile_dw[missile.cont % DIM_STAMPA_MISSILI]);


                    param_missile_up[missile.cont % DIM_STAMPA_MISSILI].id_missile = ID_MISSILE_UP;
                    param_missile_up[missile.cont % DIM_STAMPA_MISSILI].posizione_y = astronave.y;
                    param_missile_up[missile.cont % DIM_STAMPA_MISSILI].missile = missile;

                    /* crea il thread per il missile sinistro */
                    pthread_create(&thread_missile_up, NULL, movimentoMissile, &param_missile_up[missile.cont % DIM_STAMPA_MISSILI]);


                    missile.cont++;
                    timer_missile++;
                }

                break;
        }

        writeMessage(astronave);
    }

    return NULL;
}

/*************************************************************************************************
    crea il missile e aggiorna la sua posizione fino a quando non tocca il bordo superiore

    parametri: 
                *arg -> argomento della funzione thread
*************************************************************************************************/
void *movimentoMissile(void *arg){
    Thread_missile_param *params;
    params = (Thread_missile_param*) arg;
    int i;

    /* inizializziamo variabile missile nel punto centrale dell'astronave */
    params->missile.y = params->posizione_y + DIM_SPRITE_ASTRONAVE/2-2;
    params->missile.x = maxx/2 - 23;

    /* caso missile dw */
    if (params->id_missile == ID_MISSILE_DW){
        params->missile.id = ID_MISSILE_DW;

        if(params->missile.cont % DIM_STAMPA_MISSILI == 0){
            for(i = 0; i <DIM_STAMPA_MISSILI; i++){
                pthread_mutex_lock(&mutex);
                collisioni_missile_dw[i] = false; //rinizializzazione variabile bool per il riutilizzo
                pthread_mutex_unlock(&mutex);
            }
        }
        
        do {

			/* controllo sulla variabile bool per la terminazione del thread */
            if(collisioni_missile_dw[params->missile.cont % DIM_STAMPA_MISSILI] == true){
                pthread_exit(NULL);
            }



            params->missile.x+=3;
            params->missile.y++;

            writeMessage(params->missile);

            usleep(50000);
        }while(params->missile.y >= 0);
    }

    /* caso missile up */
    if (params->id_missile == ID_MISSILE_UP){
        params->missile.id = ID_MISSILE_UP;

        if(params->missile.cont % DIM_STAMPA_MISSILI == 0){
            for(i = 0; i <DIM_STAMPA_MISSILI; i++){
                pthread_mutex_lock(&mutex);
                collisioni_missile_up[i] = false; //rinizializzazione variabile bool per il riutilizzo
                pthread_mutex_unlock(&mutex);
            }
        }

        do {
			
			/* controllo sulla variabile bool per la terminazione del thread */
            if(collisioni_missile_up[params->missile.cont % DIM_STAMPA_MISSILI] == true){
                pthread_exit(NULL);
            }




            params->missile.x+=3;
            params->missile.y--;

            writeMessage(params->missile);

            usleep(50000);
        }while(params->missile.y >= 0);
    }

    pthread_exit(NULL); //termina il thread
}


/*************************************************************************************************
    gestisce il movimento della bomba e lo scrive sul buffer sino a quando non tocca il bordo
	inferiore

    parametri: 
                *arg -> argomento della funzione thread
*************************************************************************************************/
void *movimentoBomba(void *arg){
    Posizione *navicella = (Posizione *)arg;
    Posizione bomba;

    bomba.cont = navicella->cont;
    bomba.id = ID_BOMBA;
    bomba.x = navicella->x - 1;
    bomba.y = navicella->y + DIM_SPRITE_NAVICELLA_LIV1/2;

    do{
		
		/* controllo sulla variabile bool per la terminazione del thread */
        if(collisioni_bombe[bomba.cont] == true){
            
            pthread_exit(NULL);

            pthread_mutex_lock(&mutex);
            collisioni_bombe[bomba.cont] = false; //rinizializzazione variabile bool per il riutilizzo
            pthread_mutex_unlock(&mutex);

        }
		
        bomba.x--;
		
        writeMessage(bomba);

        usleep(170000);
    }while(bomba.x > maxx/2-26 );

    pthread_exit(NULL);
}

/*************************************************************************************************
    crea il movimento della navicella nemica di livello 1 e 2 nella posizione x, y 
    assegnate tramite gli argomenti della funzione

    parametri: 
                *arg -> argomento della funzione thread
*************************************************************************************************/
void *movimentoNavicella(void *arg){
    int diry, timer_discesa = 1, timer_bomba = 1, r_movimento, r_bomba, limite[N_NAV_PER_COLONNA], i;
    pthread_t thread_bomba;
    Posizione *navicella = (Posizione *)arg;

	/* imposto i limiti dello schermo per ogni navicella */
    for(i = 0; i < N_NAV_PER_COLONNA; i++)
        limite[i] = i * maxy / N_NAV_PER_COLONNA;
	
	limite[N_NAV_PER_COLONNA]= maxy;

	/* divido le navicelle su tre colonne */
    if(navicella->id == ID_NAVICELLA_LIV1){
        if(navicella->cont < N_NAV_PER_COLONNA)
            navicella->x=maxx/2+POS_COLONNA_1;
        else if(navicella->cont >= N_NAV_PER_COLONNA && navicella->cont < N_NAV_PER_COLONNA * 2)
            navicella->x =maxx/2+POS_COLONNA_2;
        else 
            navicella->x =maxx/2+POS_COLONNA_3;
    }

    writeMessage(*navicella);

    r_movimento = MIN_GEN_MOV + rand() % (MAX_GEN_MOV - MIN_GEN_MOV + 1);           //genera un numero random per il movimento della navicella
    r_bomba = MIN_GEN_BOMBA + rand() % (MAX_GEN_BOMBA - MIN_GEN_BOMBA + 1);         //genera un numero random per la generazione del timer bomba

    /* decide il movimento della navicella */
    if(r_movimento % 2 == 0)
        diry = 1;
    else
        diry = -1;

    while(true){
		
		/* controllo sulla variabile bool e sull'id della navicella per la terminazione del thread */
        if(collisioni_navicelle[navicella->cont] == true && navicella->id == ID_NAVICELLA_LIV1){
            pthread_exit(NULL);
        }
        
		/* controllo sulla variabile bool e sull'id della navicella per la terminazione del thread */
        if(collisioni_navicelle_liv2[navicella->cont] == true && navicella->id == ID_NAVICELLA_LIV2){
            pthread_exit(NULL);
        }

        /* fa scendere di una colonna la posizione delle navicelle */
        if (timer_discesa % CICLI_DISCESA == 0){
            navicella->x -= DIMENSIONE_COLONNA;
        }
        
        /* limita il movimento delle navicelle ad una sezione specifica di schermo, garantendo un effetto
           rimbalzo nel caso in cui due navicelle arrivassero allo stesso limite */
        switch (navicella->cont % N_NAV_PER_COLONNA){
            case 0:
                if(navicella->id == ID_NAVICELLA_LIV1){
                    if (navicella->y + diry >= limite[1] - 
					DIM_SPRITE_NAVICELLA_LIV1 +1|| navicella->y + diry < MINY)
                        diry *= -1;
                } else{
                    if (navicella->y + diry >= limite[1] - 
					DIM_SPRITE_NAVICELLA_LIV2 +1|| navicella->y + diry < MINY)
                        diry *= -1;
                }
                break;

            case 1:
                if(navicella->id == ID_NAVICELLA_LIV1){
                    if (navicella->y + diry >= limite[2] - 
					DIM_SPRITE_NAVICELLA_LIV1 || navicella->y + diry < limite[1])
                        diry *= -1;
                } else{
                    if (navicella->y + diry >= limite[2] - 
					DIM_SPRITE_NAVICELLA_LIV2 || navicella->y + diry < limite[1])
                        diry *= -1;
                }
                break;

            case 2:
                if(navicella->id == ID_NAVICELLA_LIV1){
                    if (navicella->y + diry >= maxy - 
					DIM_SPRITE_NAVICELLA_LIV1 || navicella->y + diry < limite[2])
                        diry *= -1;
                } else{
                    if (navicella->y + diry >= maxy - 
					DIM_SPRITE_NAVICELLA_LIV2 || navicella->y + diry < limite[2])
                        diry *= -1;
                }
                break;

            case 3:
                if(navicella->id == ID_NAVICELLA_LIV1){
                    if (navicella->y + diry >= limite[4] - 
					DIM_SPRITE_NAVICELLA_LIV1 || navicella->y + diry < limite[3])
                        diry *= -1;
                } else{
                    if (navicella->y + diry >= limite[4] - 
					DIM_SPRITE_NAVICELLA_LIV2 || navicella->y + diry < limite[3])
                        diry *= -1;
                }
                break;

            case 4:
                if(navicella->id == ID_NAVICELLA_LIV1){
                    if (navicella->y + diry >= maxy - 
					DIM_SPRITE_NAVICELLA_LIV1 || navicella->y + diry < limite[4])
                        diry *= -1;
                } else{
                    if (navicella->y + diry >= maxy - 
					DIM_SPRITE_NAVICELLA_LIV2 || navicella->y + diry < limite[4])
                        diry *= -1;
                }
                break;

            default:
                break;
        }
        
        navicella->y += diry;        //incrementa la posizione sulle x

        writeMessage(*navicella);

        /* fa cadere la bomba a distanza di r_bomba cicli */
        if(timer_bomba % r_bomba == 0){
            pthread_create(&thread_bomba, NULL, movimentoBomba, navicella);
        }

        timer_discesa++;
        timer_bomba++;
        usleep(160000);
    }

    return NULL;
}


/*************************************************************************************************
    crea le navicelle di livello 1

    parametri: 
                *arg -> argomento della funzione thread
*************************************************************************************************/
void *navicelleLiv1(void *arg){
    Posizione navicella[N_NAVICELLE];
    pthread_t thread_navicella;
    int i, distanza, limite[N_NAV_PER_COLONNA], max_gen_distanza = maxx/5, flag;

	/* imposto i limiti dello schermo per ogni navicella */
    for(i = 0; i < N_NAV_PER_COLONNA; i++)
        limite[i] = i * maxy / N_NAV_PER_COLONNA;

	limite[N_NAV_PER_COLONNA]= maxy;

    for(i = 0; i < N_NAVICELLE; i++){

        /* inizializzo variabile navicella di livello 1 */
        navicella[i].id = ID_NAVICELLA_LIV1;
        navicella[i].cont = i;
        
        srand(time(NULL) + pthread_self());       //inizializza il seed per la generazione casuale
    
        /* assegna ad ogni navicella la propria posizione nello schermo */
        switch(navicella[i].cont % N_NAV_PER_COLONNA){
            case 0:
                do{
                    distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
                    navicella[i].y = MINY + distanza - DIM_SPRITE_NAVICELLA_LIV1;
                }while(navicella[i].y <= MINY || navicella[i].y >= limite[1]- DIM_SPRITE_NAVICELLA_LIV1 - 1);
                break;

            case 1:
                do{
                    distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
                    navicella[i].y = limite[1] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
                }while(navicella[i].y <= limite[1] || navicella[i].y >= maxy - DIM_SPRITE_NAVICELLA_LIV1 - 1);
                break;
                
            case 2:
                do{
                    distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
                    navicella[i].y = limite[2] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
                }while(navicella[i].y <= limite[2] || navicella[i].y >= maxy - DIM_SPRITE_NAVICELLA_LIV1 - 1);
                break;
            
            case 3:
                do{
                    distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
                    navicella[i].y = limite[3] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
                }while(navicella[i].y <= limite[3] || navicella[i].y >= limite[4]);
                break;
            
            case 4:
                do{
                    distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
                    navicella[i].y = limite[4] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
                }while(navicella[i].y <= limite[4] || navicella[i].y >= maxy);
                break;
        }

        pthread_create(&thread_navicella, NULL, movimentoNavicella, &navicella[i]);

    }

    /* aspetta che tutte le navicelle siano state uccise per uscire dal thread */
    while(1){
        flag = 0;
        for(i = 0; i < N_NAVICELLE; i++)
            if(collisioni_navicelle[i] == false || collisioni_navicelle_liv2[i] == false)
                flag = 1;

        if(flag == 0)
            break;

        usleep(10000);
    }

    sleep(1);
    pthread_exit(NULL);
}


/*************************************************************************************************
    crea le navicelle di livello 2

    parametri: 
                *navicella -> navicella da creare
*************************************************************************************************/
void navicelleLiv2(Posizione *navicella){
    int limite[N_NAV_PER_COLONNA], i;
    pthread_t thread_crea_navicella_liv2;
    

    navicella->id = ID_NAVICELLA_LIV2;       //assegna il giusto id alla navicella di livello 2

	/* imposto i limiti dello schermo per ogni navicella */
    for(i = 0; i < N_NAV_PER_COLONNA; i++)
        limite[i] = i * maxy / N_NAV_PER_COLONNA;

	limite[N_NAV_PER_COLONNA]= maxy;

    /* imposta i nuovi limiti per la navicella di livello 2 */
    switch (navicella->cont % N_NAV_PER_COLONNA){
        case 0:
            if(navicella->y + DIM_SPRITE_NAVICELLA_LIV2 > limite[1])
                navicella->y = limite[1] - DIM_SPRITE_NAVICELLA_LIV2;
            break;
        
        case 1:
            if(navicella->y+ DIM_SPRITE_NAVICELLA_LIV2 > limite[2])
                navicella->y = limite[2] - DIM_SPRITE_NAVICELLA_LIV2;
            break;
        
        case 2:
            if(navicella->y + DIM_SPRITE_NAVICELLA_LIV2 > limite[3])
                navicella->y = limite[3] - DIM_SPRITE_NAVICELLA_LIV2;
            break;
        
        case 3:
            if(navicella->y + DIM_SPRITE_NAVICELLA_LIV2 > limite[4])
                navicella->y = limite[4] - DIM_SPRITE_NAVICELLA_LIV2;
            break;
        
        case 4:
            if(navicella->y + DIM_SPRITE_NAVICELLA_LIV2 > maxy)
                navicella->y = limite[5] - DIM_SPRITE_NAVICELLA_LIV2;
            break;
        
        default:
            break;
    }
    
    pthread_create(&thread_crea_navicella_liv2, NULL, movimentoNavicella, navicella);
}

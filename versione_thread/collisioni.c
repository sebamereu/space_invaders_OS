#include "collisioni.h"

/*************************************************************************************************
    inizializza le variabili globali per il controllo delle collisioni

*************************************************************************************************/
void inizializzaVettoriCollisioni() {
	int i;

	pthread_mutex_lock(&mutex);
	collisioni_astronave = false;
	pthread_mutex_unlock(&mutex);

	for (i = 0; i < DIM_STAMPA_MISSILI; i++) {

		pthread_mutex_lock(&mutex);
		collisioni_missile_dw[i] = false;
		pthread_mutex_unlock(&mutex);
	}

	for (i = 0; i < DIM_STAMPA_MISSILI; i++) {

		pthread_mutex_lock(&mutex);
		collisioni_missile_up[i] = false;
		pthread_mutex_unlock(&mutex);
	}

	for (i = 0; i < N_NAVICELLE; i++) {
		pthread_mutex_lock(&mutex);
		collisioni_navicelle[i] = false;
		pthread_mutex_unlock(&mutex);
	}

	for (i = 0; i < N_NAVICELLE; i++) {
		pthread_mutex_lock(&mutex);
		collisioni_navicelle_liv2[i] = false;
		pthread_mutex_unlock(&mutex);
	}

	for (i = 0; i < DIM_STAMPA_BOMBE; i++) {
		pthread_mutex_lock(&mutex);
		collisioni_bombe[i] = false;
		pthread_mutex_unlock(&mutex);
	}

}


/*************************************************************************************************
    gestisce le collisioni che avvengono nell'asse delle y tra navicelle sulla stessa 
    sezione di schermo 

    parametri:
                navicelle[] -> vettore delle navicelle da controllare
                n_vite -> vettore delle vite delle navicelle
*************************************************************************************************/
void collisioneNavicelle(Posizione navicelle[], int n_vite[]){
    int i, j, k;

    /* controlla ogni navicella con la sua naicella sottostante */
    for(i = 0; i < N_NAVICELLE; i++){
        if(navicelle[i].x != -1 && navicelle[i + N_NAV_PER_COLONNA].x != -1){

            /* se le navicelle hanno la stessa y (ovvero si trovano sulla stessa riga), quella di sotto 
               (sicuramente di livello 2) verra' fatta scendere ulteriormente */
            if(navicelle[i].x == navicelle[i + N_NAV_PER_COLONNA].x){
                for(j = 0; j < 2; j++)
                    for(k = 0; k < 11; k++){
                        pthread_mutex_lock(&mutex);
                        mvaddch(navicelle[i + N_NAV_PER_COLONNA].y + j, navicelle[i + N_NAV_PER_COLONNA].x + k, ' ');
                        pthread_mutex_unlock(&mutex);
                    }
                /* la navicella di livello 2 scende di una riga */
                navicelle[i + N_NAV_PER_COLONNA].y += DIMENSIONE_COLONNA;

                /* stampa le navicelle */
                stampaNavicelle(navicelle[i + N_NAV_PER_COLONNA], navicelle, n_vite);
                stampaNavicelle(navicelle[i], navicelle, n_vite);
		    }
        }
    }
}

/*************************************************************************************************
    gestisce le collisioni tra bombe e astronave. Quando le vite dell'astronave arrivano
    a 0, viene terminato il programma

    parametri:
                bombe[] -> vettore delle bombe da controllare
                astronave -> posizione dell'astronave
                *n_vite -> puntatore al numero delle vite dell'astronave
*************************************************************************************************/
void collisioniBombaAstronave(Posizione bombe[], Posizione astronave, int *n_vite) {
	int i,j,k;

	/* controlla la collisione tra le bombe e l'astronave */

	for(i = 0; i < DIM_STAMPA_BOMBE; i++)
		for(j=0; j< 4; j++)
			for(k=0; k< 2; k++)
				if(bombe[i].y == astronave.y-k && bombe[i].x == astronave.x+j) {

					bombe[i].x = -1;                //pulisce la posizione dal vettore

					pthread_mutex_lock(&mutex);
					collisioni_bombe[i] = true;
					pthread_mutex_unlock(&mutex);

					(*n_vite)--;                    //diminuisce il numero di vite dell'astronave
				}
}


/*************************************************************************************************
    gestisce le collisioni tra bombe e missili. Quando una bomba ed un missile si trovano 
    nella stessa posizione, i processi vengono terminati 

    parametri:
                bomba[] -> vettore delle bombe da controllare
                missile[] -> vettore dei missili da controllare
                n_vite -> vettore delle vite delle navicelle
*************************************************************************************************/
void collisioneBombaMissile(Posizione bomba[], Posizione missile[], int *n_vite){
	int i, j;
	for(i = 0; i < DIM_STAMPA_BOMBE; i++)
		for(j = 0; j < DIM_STAMPA_MISSILI; j++)
			if(bomba[i].x != -1 && missile[j].x != -1)
                /* se la posizione della bomba e la posizione del missile coincidono 
                   vengono terminati i due processi */
				if(bomba[i].x == missile[j].x && bomba[i].y == missile[j].y){
                    pthread_mutex_lock(&mutex);
					mvaddch(bomba[i].y, bomba[i].x, ' ');
					
					curs_set(0);
					refresh();
                    pthread_mutex_unlock(&mutex);
					

					bomba[i].x = -1;
					missile[j].x = -1;
					
                    if(missile[j].id == ID_MISSILE_DW){
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_dw[j] = true;
                        pthread_mutex_unlock(&mutex);
                    } else{
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_up[j] = true;
                        pthread_mutex_unlock(&mutex);
                    }

                    pthread_mutex_lock(&mutex);
                    collisioni_bombe[i] = true;
                    pthread_mutex_unlock(&mutex); 
                }
}


/*************************************************************************************************
    gestisce le collisioni tra navicelle e missili

    parametri:
                navicelle[] -> vettore delle navicelle da controllare
                missili[] -> vettore dei missili da controllare
                n_vite[] -> vettore delle vite delle navicelle 
                navicelle_liv2[] -> vettore delle navicelle di livello 2 da controllare
                
*************************************************************************************************/
void collisioneMissiliNavicelle(Posizione missili[], Posizione navicelle[], int n_vite[], Posizione navicelle_liv2[]){
    int i, j, k, z;

    for(i = 0; i < DIM_STAMPA_MISSILI; i++){
        for(j = 0; j < N_NAVICELLE; j++){
            /* se missili e navicelle esistono */
            if(missili[i].x != -1 && navicelle[j].x != -1){

                /* se le coordinate di qualche missile corrispondono con le coordinate di qualche navicella allora vengono 
                   cancellati missile e navicella che collidono e vengono terminati i processi */
                if(missili[i].y >= navicelle[j].y 
				&& missili[i].y <= (navicelle[j].y + DIM_SPRITE_NAVICELLA_LIV1 - 1) 
				&& missili[i].x >= navicelle[j].x 
				&& missili[i].x <= (navicelle[j].x + DIM_SPRITE_NAVICELLA_LIV1 - 1) 
				&& navicelle[j].id == ID_NAVICELLA_LIV1){
                    
                    
                    /* imposta a true la cella del vettore che controlla le collisioni */
                    pthread_mutex_lock(&mutex);
                    collisioni_navicelle[j] = true;
                    pthread_mutex_unlock(&mutex);

                    /* imposta a true la cella del vettore che controlla le collisioni */
                    if(missili[i].id == ID_MISSILE_DW){
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_dw[i] = true;
                        pthread_mutex_unlock(&mutex);
                    } else{
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_up[i] = true;
                        pthread_mutex_unlock(&mutex);
                    }

                    /* cancella il missile */
                    pthread_mutex_lock(&mutex);
                    mvaddch(missili[i].y, missili[i].x, ' ');
                    pthread_mutex_unlock(&mutex);

                    /* cancella navicella e stampa */
                    for(z = 0; z < DIM_SPRITE_NAVICELLA_LIV1; z++)
                        for(k = 0; k < DIM_SPRITE_NAVICELLA_LIV1; k++){
                            pthread_mutex_lock(&mutex);
                            mvaddch(navicelle[j].y + z, navicelle[j].x + k, ' ');
                            pthread_mutex_unlock(&mutex);
                        }
                    
                    /* aggiorna lo schermo */
                    pthread_mutex_lock(&mutex);
                    curs_set(0);
                    refresh();
                    pthread_mutex_unlock(&mutex);

                    /* copia la navicella di livello 1 */
                    navicelle_liv2[j] = navicelle[j];

                    /* crea una navicella di livello 2 */
                    navicelleLiv2(&navicelle_liv2[j]);
                    
                    /* toglie dai vettori i missili e le navicelle che collidono */
                    navicelle[j].x = -1;
                    missili[i].x = -1;

                /* viene eseguito lo stesso controllo sulle navicelle di livello 2 */
				}else if(missili[i].y >= navicelle[j].y && missili[i].y <= (navicelle[j].y + DIM_SPRITE_NAVICELLA_LIV2 - 1) &&
				   missili[i].x >= navicelle[j].x && missili[i].x <= (navicelle[j].x + 2) && navicelle[j].id == ID_NAVICELLA_LIV2){
				  

                    /* decrementa le vite della navicella */
                    n_vite[j]--;

                    /* imposta a -1 la x del missile */
                    missili[i].x = -1;
                    
                    /* imposta a true la cella del vettore che controlla le collisioni */
                    if(missili[i].id == ID_MISSILE_DW){
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_dw[i] = true;
                        pthread_mutex_unlock(&mutex);
                    } else{
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_up[i] = true;
                        pthread_mutex_unlock(&mutex);
                    }

                    /* cancella il missile */
                    pthread_mutex_lock(&mutex);
                    mvaddch(missili[i].y, missili[i].x, ' ');
                    pthread_mutex_unlock(&mutex);

                    /* se le vite sono uguali a 0 comunica al thread della navicella la sua terminazione */
                    if(n_vite[j] <= 0){

                        /* imposta a true la cella del vettore che controlla le collisioni */
                        pthread_mutex_lock(&mutex);
                        collisioni_navicelle_liv2[j] = true;
                        pthread_mutex_unlock(&mutex);

                        /* cancella la navicella */
                        for(z = 0; z < 7; z++)
                            for(k = 0; k < 3; k++){
                                pthread_mutex_lock(&mutex);    
                                mvaddch(navicelle[j].y + z, navicelle[j].x + k, ' ');
                                pthread_mutex_unlock(&mutex);
                            }
						
                    /* imposta a true la cella del vettore che controlla le collisioni */
                    if(missili[i].id == ID_MISSILE_DW){
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_dw[i] = true;
                        pthread_mutex_unlock(&mutex);
                    } else{
                        pthread_mutex_lock(&mutex);
                        collisioni_missile_up[i] = true;
                        pthread_mutex_unlock(&mutex);
                    }

                        /* aggiorna lo schermo */
                        pthread_mutex_lock(&mutex);
                        curs_set(0);
                        refresh();
                        pthread_mutex_unlock(&mutex);

                        /* toglie dai vettori i missili e le navicelle che collidono */
                        navicelle[j].x = -1;
                    }
                }
            }
        }
    }
}


/*************************************************************************************************
    controlla se la navicella sbatte con il bordo inferiore, restituendo true se la collisione
    avviene, e false nel caso in cui la collisione non avvenga

    parametri:
                navicelle[] -> vettore delle navicelle da controllare
*************************************************************************************************/
_Bool collisioneNavicellaBordo(Posizione navicelle[]){
	int i;
	for(i = 0; i < N_NAVICELLE; i++)
        if(navicelle[i].x != -1)
            if(navicelle[i].x + 1 <= maxx/2 - 21)
                return true;
			
	return false;
}

/*************************************************************************************************
    controlla se tutte le navicelle nemiche sono state colpite

    parametri:
                n_nvite_navicelle[] -> vettore che contiene le vite delle navicelle
*************************************************************************************************/
_Bool quadroTerminato(int n_vite_navicelle[]){
	int i;

    /* quando tutte le navicelle hanno vita pari a 0 la funzione restituisce true */
	for(i = 0; i < N_NAVICELLE; i++){
		if(n_vite_navicelle[i] > 0)
			return false;
	}
	
	return true;
}

/*************************************************************************************************
    gestisce le vite dell'astronave, restituendo true quando sono pari a 0

    parametri:
                n_vite_astronave -> numero di vite dell'astronave
*************************************************************************************************/
_Bool astronaveMorta(int n_vite_astronave){
    if(n_vite_astronave <= 0)
        return true;
    
    return false;
}

/*************************************************************************************************
    termina il gioco quando una delle funzioni di controllo restituisce true

    parametri:
                astronave -> posizione dell'astronave
                navicelle[] -> vettore delle navicelle da controllare
                n_vite_navicelle[] -> vettore delle vite delle navicelle
                n_vite_astronave -> numero delle vite dell'astronave
                record_punteggi[] -> vettore dei punteggi
                
*************************************************************************************************/
_Bool fineGioco(Posizione astronave, Posizione navicelle[], int n_vite_navicelle[], int n_vite_astronave){
    int i, j, k;
	
	char premereSpazio [50]="Per chiudere il programma premere Ctrl+c";
	
    char game_over[6][72] = {
        {" _____           __  __ ______    ______      ________ _____  "},
        {"/ ____|    /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\ "},
        {"| |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) |"},
        {"| | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  / "},
        {"| |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\ "},
        {"\\_____/_/     \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\"}
    };

    char you_win[6][51] = {
        {" __     ______  _    _  __          _______ _   _ "},
        {" \\ \\   / / __ \\| |  | | \\ \\        / /_   _| \\ | |"},
        {"  \\ \\_/ / |  | | |  | |  \\ \\  /\\  / /  | | |  \\| |"},
        {"   \\   /| |  | | |  | |   \\ \\/  \\/ /   | | | . ` |"},
        {"    | | | |__| | |__| |    \\  /\\  /   _| |_| |\\  |"},
        {"    |_|  \\____/ \\____/      \\/  \\/   |_____|_| \\_|"}
    };

    /* se una delle funzioni restituisce true il gioco termina */
    if (collisioneNavicellaBordo(navicelle) || astronaveMorta(n_vite_astronave) ||quadroTerminato(n_vite_navicelle)){
        
        /* imposta a true la variabile per il controllo della collisione su astronave */
        pthread_mutex_lock(&mutex);
        collisioni_astronave = true;
        pthread_mutex_unlock(&mutex);

        /* imposta a true la cella del vettore che controlla le collisioni sulle navicelle */
        for(i = 0; i < N_NAVICELLE; i++){
            pthread_mutex_lock(&mutex);
            collisioni_navicelle[i] = true;
            collisioni_navicelle_liv2[i] = true;
            pthread_mutex_unlock(&mutex);
        }
        /* imposta a true la cella del vettore che controlla le collisioni sui missili */
        for(i = 0; i < DIM_STAMPA_MISSILI; i++){
            pthread_mutex_lock(&mutex);
            collisioni_missile_dw[i] = true;
            collisioni_missile_up[i] = true;
            pthread_mutex_unlock(&mutex);
        }

        /* se il giocatore vince viene segnalata la vittoria, altrimenti la sconfitta */
        attron(COLOR_PAIR(COLORE_NAVICELLE));
        if(quadroTerminato(n_vite_navicelle)){
            sleep(1);
            clear();
            for(i = 0; i < 6; i++)
                for(j = 0; j < strlen(you_win[i]); j++){
                    pthread_mutex_lock(&mutex);
                    mvaddch(get_center_y(6) + i, get_center_x(strlen(you_win[i])) + j, you_win[i][j]);
                    pthread_mutex_unlock(&mutex);
                }
				
			for(k=0; k<strlen(premereSpazio);k++){
				pthread_mutex_lock(&mutex);
                    mvaddch(maxy-3, get_center_x(strlen(premereSpazio)) + k, premereSpazio[k]);
					pthread_mutex_unlock(&mutex);
					
			}

            pthread_mutex_lock(&mutex);
            refresh();
            pthread_mutex_unlock(&mutex);

            sleep(1000);

        } else{
            sleep(1);

            pthread_mutex_lock(&mutex);
            clear();
            pthread_mutex_unlock(&mutex);

            for(i = 0; i < 6; i++)
                for(j = 0; j < strlen(game_over[i]); j++){
                    pthread_mutex_lock(&mutex);
                    mvaddch(get_center_y(6) + i, get_center_x(strlen(game_over[i])) + j, game_over[i][j]);
                    pthread_mutex_unlock(&mutex);
                }
				
			for(k=0; k<strlen(premereSpazio);k++){
				pthread_mutex_lock(&mutex);
                    mvaddch(maxy-3, get_center_x(strlen(premereSpazio)) + k, premereSpazio[k]);
					pthread_mutex_unlock(&mutex);
					
			}
            
            pthread_mutex_lock(&mutex);
            refresh();
            pthread_mutex_unlock(&mutex);

            sleep(1000);

        }
        attroff(COLOR_PAIR(COLORE_NAVICELLE));

        pthread_mutex_lock(&mutex);
        clear();
        refresh();
        pthread_mutex_unlock(&mutex);

        sleep(5);
        return true;
    }
    return false;
}


/*************************************************************************************************
    restituisce il centro dello schermo in base alle dimensioni dell'oggetto

    parametri: 
                x -> coordinata della prima occorrenza della stampa
*************************************************************************************************/
int get_center_x(int x){
    return (maxx / 2) - (x / 2);
}

/*************************************************************************************************
    restituisce il centro dello schermo in base alle dimensioni dell'oggetto

    parametri: 
                y -> coordinata della prima occorrenza della stampa
*************************************************************************************************/
int get_center_y(int y){
    return (maxy / 2) - (y / 2);
}


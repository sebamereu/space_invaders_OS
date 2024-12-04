#include "collisioni.h"

/*************************************************************************************************
    gestisce le collisioni tra navicelle e missili

    parametri:
                pipeout -> pipe su cui scrivere
                missili[] -> vettore dei missili da controllare
                navicelle[] -> vettore delle navicelle da controllare
                n_vite[] -> vettore delle vite delle navicelle
*************************************************************************************************/
void collisioneMissiliNavicelle(int pipeout, Posizione missili[], Posizione navicelle[], int n_vite[], int *punteggio) {
	int i, j, k, z, w, q;
	Posizione navicella_liv2;

	for(i = 0; i < DIM_STAMPA_MISSILI; i++) {
		for(j = 0; j < N_NAVICELLE; j++) {
			/* se missili e navicelle esistono */
			if(missili[i].x != -1 && navicelle[j].x != -1){

				/* se le coordinate di qualche missile corrispondono con le coordinate di qualche navicella allora vengono
				   cancellati missile e navicella che collidono e vengono terminati i processi */
				   if(missili[i].y >= navicelle[j].y && missili[i].y <= (navicelle[j].y + DIM_SPRITE_NAVICELLA_LIV1 - 1) &&
				   missili[i].x >= navicelle[j].x && missili[i].x <= (navicelle[j].x + DIM_SPRITE_NAVICELLA_LIV1 - 1) && navicelle[j].id == ID_NAVICELLA_LIV1){
				       
			

							/* cancella navicella e stampa */
							for(z = 0; z < DIM_SPRITE_NAVICELLA_LIV1; z++)
								for(k = 0; k < DIM_SPRITE_NAVICELLA_LIV1; k++)
									mvaddch(navicelle[j].y + z, navicelle[j].x + k, ' ');

							curs_set(0);
							refresh();

							/* copia la navicella di livello 1 */
							navicella_liv2 = navicelle[j];

							/* crea una navicella di livello 2 */
							navicellaLiv2(pipeout, navicella_liv2);

							/* toglie dai vettori i missili e le navicelle che collidono */
							navicelle[j].x = -1;
							missili[i].x = -1;

							/* termina i processi */
							kill(navicelle[j].pid, SIGHUP);
							kill(missili[i].pid, SIGHUP);

				   }else if(missili[i].y >= navicelle[j].y && missili[i].y <= (navicelle[j].y + DIM_SPRITE_NAVICELLA_LIV2 - 1) &&
				   missili[i].x >= navicelle[j].x && missili[i].x <= (navicelle[j].x + 2) && navicelle[j].id == ID_NAVICELLA_LIV2){
				
							n_vite[j]--;
							if(n_vite[j] <= 0) {


								for(z = 0; z < 7; z++)
									for(k = 0; k < 3; k++)
										mvaddch(navicelle[j].y + z, navicelle[j].x + k, ' ');

								curs_set(0);
								refresh();

								/* toglie dai vettori i missili e le navicelle che collidono */
								navicelle[j].x = -1;

								/* termina i processi */
								kill(navicelle[j].pid, SIGHUP);
							}

							missili[i].x = -1;
							kill(missili[i].pid, SIGHUP);
						}
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
	int i, j, k;

	/* controlla la collisione tra le bombe e l'astronave */
	for(i = 0; i < DIM_STAMPA_BOMBE; i++)
		for(j=0; j< (DIM_SPRITE_ASTRONAVE-1); j++)
			for(k=0; k< (DIM_SPRITEY_ASTRONAVE); k++)
				if(bombe[i].y == astronave.y-k &&
				        bombe[i].x == astronave.x+j) {

					bombe[i].x = -1;                //pulisce la posizione dal vettore
					kill(bombe[i].pid, SIGHUP);     //uccide il processo della bomba che e' entrato in collisione con l'astronave

					(*n_vite)--;                    //diminuisce il numero di vite dell'astronave
				}
}

/*************************************************************************************************
    gestisce le collisioni tra bombe e missili. Quando una bomba ed un missile si trovano 
    nella stessa posizione, i processi vengono terminati 

    parametri:
                bomba[] -> vettore delle bombe da controllare
                missile[] -> vettore dei missili da controllare
                *n_vite -> numero vite astronave
*************************************************************************************************/
void collisioneBombaMissile(Posizione bomba[], Posizione missile[], int *n_vite){
	int i, j;
	for(i = 0; i < DIM_STAMPA_BOMBE; i++)
		for(j = 0; j < DIM_STAMPA_MISSILI; j++)
			if(bomba[i].x != -1 && missile[j].x != -1)
                /* se la posizione della bomba e la posizione del missile coincidono 
                   vengono terminati i due processi */
				if(bomba[i].x == missile[j].x && bomba[i].y == missile[j].y){
					mvaddch(bomba[i].y, bomba[i].x, ' ');

					
					curs_set(0);
					refresh();
					
					bomba[i].x = -1;
					missile[j].x = -1;
					
					kill(bomba[i].pid, SIGHUP);
					kill(missile[j].pid, SIGHUP);
                }
}


/*************************************************************************************************
    gestisce le collisioni che avvengono nell'asse delle y tra navicelle sulla stessa 
    sezione di schermo 

    parametri:
                navicelle[] -> vettore delle navicelle da controllare
                n_vite[] -> vettore delle vite delle navicelle
*************************************************************************************************/
void collisioneNavicelle(Posizione navicelle[], int n_vite[]){
    int i, j, k;

    /* controlla ogni navicella con la sua navicella sottostante */
    for(i = 0; i < N_NAVICELLE; i++){
        if(navicelle[i].x != -1 && navicelle[i + N_NAV_PER_COLONNA].x != -1){

            /* se le navicelle hanno la stessa x (ovvero si trovano sulla stessa colonna), quella di sotto 
               (sicuramente di livello 2) verra' fatta scendere ulteriormente */
            if(navicelle[i].x == navicelle[i + N_NAV_PER_COLONNA].x){
                for(j = 0; j < 7; j++)
                    for(k = 0; k < 3; k++)
                        mvaddch(navicelle[i + N_NAV_PER_COLONNA].y + j, navicelle[i + N_NAV_PER_COLONNA].x + k, ' ');
            
                /* la navicella di livello 2 scende di una colonna */
                navicelle[i + N_NAV_PER_COLONNA].x += DIMENSIONE_COLONNA;

                /* stampa le navicelle */
                stampaNavicelle(navicelle[i + N_NAV_PER_COLONNA], navicelle, n_vite);
                stampaNavicelle(navicelle[i], navicelle, n_vite);
		    }
        }
    }
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
	endwin();
	return true;
}


/*************************************************************************************************
    controlla se la navicella sbatte con l'astronave, restituendo true se la collisione
    avviene, e false nel caso in cui la collisione non avvenga

    parametri:
                astronave -> posizione dell'astronave
                navicelle[] -> vettore delle navicelle da controllare
*************************************************************************************************/
_Bool collisioneNavicellaAstronave(Posizione astronave, Posizione navicelle[]){
    int i, j;

    for(i = 0; i < N_NAVICELLE; i++){
        if(astronave.x != -1 && navicelle[i].x != -1){

            /* controlla che tutti i caratteri della navicella di livello 1 entrino in contatto con l'astronave */
            for(j = 0; j < DIM_SPRITE_NAVICELLA_LIV1; j++)
                if(navicelle[i].y + j >= astronave.x && navicelle[i].y + j <= astronave.y + DIM_SPRITE_ASTRONAVE && 
                navicelle[i].x + 1 == astronave.x - 1)
                    return true;

            /* controlla che tutti i caratteri della navicella di livello 2 entrino in contatto con l'astronave */
            for(j = 0; j < DIM_SPRITE_NAVICELLA_LIV2; j++)
                if(navicelle[i].y + j >= astronave.x && navicelle[i].y + j <= astronave.y + DIM_SPRITE_ASTRONAVE && 
                navicelle[i].x + 1 == astronave.x - 1)
                    return true;
        }
    }

    return false;           //restituisce false se la collisione non e' avvenuta
}


/*************************************************************************************************
    controlla se la navicella sbatte con il bordo di sinistra, restituendo true se la collisione
    avviene, e false nel caso in cui la collisione non avvenga

    parametri:
                navicelle[] -> vettore delle navicelle da controllare
*************************************************************************************************/
_Bool collisioneNavicellaBordo(Posizione navicelle[]){
	int i;
	for(i = 0; i < N_NAVICELLE; i++)
        if(navicelle[i].x != -1)
            if(navicelle[i].x + 1 <= maxx/2-21)
			    return true;
			
	return false;
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
                record_punteggi -> vettore dei punteggi
*************************************************************************************************/
_Bool fineGioco(Posizione astronave, Posizione navicelle[], int n_vite_navicelle[], int n_vite_astronave){
    int i, j, k, fine=0;
	char c;
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
    if (astronaveMorta(n_vite_astronave) || quadroTerminato(n_vite_navicelle)|| collisioneNavicellaBordo(navicelle) ){
        
		
        attron(COLOR_PAIR(COLORE_NAVICELLE));
        if(quadroTerminato(n_vite_navicelle)){
			
            clear();
            for(i = 0; i < 6; i++)
                for(j = 0; j < strlen(you_win[i]); j++)
                    mvaddch(get_center_y(6) + i, get_center_x(strlen(you_win[i])) + j, you_win[i][j]);
            
			for(k=0; k<strlen(premereSpazio);k++)
                    mvaddch(maxy-3, get_center_x(strlen(premereSpazio)) + k, premereSpazio[k]);
            
            refresh();
            sleep(1000);
				
			
			
        } else{
            clear();
            for(i = 0; i < 6; i++)
                for(j = 0; j < strlen(game_over[i]); j++)
                    mvaddch(get_center_y(6) + i, get_center_x(strlen(game_over[i])) + j, game_over[i][j]);


			
			for(k=0; k<strlen(premereSpazio);k++)
                    mvaddch(maxy-3, get_center_x(strlen(premereSpazio)) + k, premereSpazio[k]);
					
            refresh();
            sleep(1000);
			
        }	
        attroff(COLOR_PAIR(COLORE_NAVICELLE));


        clear();
        refresh();

        for(i = 0; i < N_NAVICELLE; i++)
            kill(navicelle[i].pid, SIGHUP);
		
        return true;
    }
	
		
    return false;
}

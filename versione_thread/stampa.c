#include "stampa.h"

/*************************************************************************************************
    legge messaggio dal buffer

*************************************************************************************************/
Posizione leggiMessaggio() {
	Posizione valore_letto;

	/* previene l'underflow */
	sem_wait(&semph_underflow);

	/* acquisisce il valore dal buffer ed incrementa il contatore */
	pthread_mutex_lock(&mutex);         //inizio sezione critica
	valore_letto = buffer[out];
	out = (out + 1) % DIM_BUFFER;
	pthread_mutex_unlock(&mutex);       //fine sezione critica

	/* segnala che un elemento e' stato aggiunto all'interno del buffer */
	sem_post(&semph_overflow);

	return valore_letto;    //restituisce il valore letto dal buffer
}



/*************************************************************************************************
    gestisce le stampe di tutti gli oggetti presenti a schermo, sfruttando le funzioni apposite

*************************************************************************************************/
void stampaGioco() {
	Posizione valore_letto, astronave, missile_dw[DIM_STAMPA_MISSILI], missile_up[DIM_STAMPA_MISSILI],
	          navicelle[N_NAVICELLE], navicelle_liv2[N_NAVICELLE], bombe[DIM_STAMPA_BOMBE];
	int i, n_vite_astronave = N_VITE_ASTRONAVE, n_vite_navicelle[N_NAVICELLE];
	int livello_dim, livello_count2=0, livello_count=0,x_sprite, y_sprite,x_sprite2, y_sprite2;
	
	
	/* inizializzo posizione bordo superiore */
	x_sprite2=maxx+5;
	y_sprite2=0;

	/* inizializzo posizione bordo  inferiore*/
	x_sprite=maxx+5;
	y_sprite=maxy-1;
	
	char sprite1[]= "*  ";

	/* creazione bordo superiore concatenando la spite1 */
	char sprite[BORDO];

	for(i=0; i< BORDO ; i++) {
		strcat(sprite, sprite1);
	}
	
	/* creazione bordo inferiore concatenando la sprite1*/
	char sprite2[BORDO];

	for(i=0; i< BORDO ; i++) {
		strcat(sprite2, sprite1);
	}

	livello_dim=MOV_STELLE;
	
	
	/* inizializzo a -1 tutte le x */
	astronave.x = -1;

	for (i = 0; i < DIM_STAMPA_MISSILI; i++) {
		missile_dw[i].x = -1;
		missile_up[i].x = -1;
	}

	for (i = 0; i < N_NAVICELLE; i++) {
		navicelle[i].x = -1;
	}

	for (i = 0; i < N_NAVICELLE; i++) {
		navicelle_liv2[i].x = -1;
	}

	for(i = 0; i < DIM_STAMPA_BOMBE; i++) {
		bombe[i].x = -1;
	}

	for (i = 0; i < N_NAVICELLE; i++) {
		n_vite_navicelle[i] = N_VITE_NAV_LIV2;
	}

	/* inizializza i vettori e le variabili per la gestione delle collisioni */
	inizializzaVettoriCollisioni();

	do {

		attroff(COLOR_PAIR(COLORE_NAVICELLE));

		valore_letto = leggiMessaggio();        //legge il valore dal buffer e lo assegna alla variabile

		if(maxx > 90 || maxy > 26) {
			stampaVite(n_vite_astronave);
		} else {

			/* stampa il valore delle vite dell'astronave */
			pthread_mutex_lock(&mutex);
			mvprintw(1, maxx - strlen("Vite: %d"), "Vite: %d\n", n_vite_astronave);
			pthread_mutex_unlock(&mutex);

		}

			/*
			    in base all'id stampa uno degli oggetti
			    stampa solo nel caso in cui la variabile del controllo di collisione ha valore false
			*/
			switch(valore_letto.id) {
			case ID_ASTRONAVE:
				if(collisioni_astronave == false)
					stampaAstronave(valore_letto, &astronave);
				break;

			case ID_MISSILE_DW:
				if(collisioni_missile_dw[valore_letto.cont % DIM_STAMPA_MISSILI] == false)
					stampaMissile(valore_letto, missile_dw);
				break;

			case ID_MISSILE_UP:
				if(collisioni_missile_up[valore_letto.cont % DIM_STAMPA_MISSILI] == false)
					stampaMissile(valore_letto, missile_up);
				break;

			case ID_NAVICELLA_LIV1:
				stampaNavicelle(valore_letto, navicelle, n_vite_navicelle);
				break;

			case ID_NAVICELLA_LIV2:
				if(collisioni_navicelle_liv2[valore_letto.cont] == false)
					stampaNavicelle(valore_letto, navicelle, n_vite_navicelle);
				break;

			case ID_BOMBA:
				if(collisioni_bombe[valore_letto.cont % DIM_STAMPA_BOMBE] == false)
					stampaBomba(valore_letto, bombe);
				if(bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x<=maxx/2-22)
					mvaddch(bombe[valore_letto.cont % DIM_STAMPA_BOMBE].y,
					        bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x, ' ');
				break;

			}

			/* controlla tutte le collisioni */
			collisioneMissiliNavicelle(missile_dw, navicelle, n_vite_navicelle, navicelle_liv2);
			collisioneMissiliNavicelle(missile_up, navicelle, n_vite_navicelle, navicelle_liv2);
			collisioniBombaAstronave(bombe, astronave, &n_vite_astronave);
			collisioneBombaMissile(bombe, missile_dw, &n_vite_astronave);
			collisioneBombaMissile(bombe, missile_up, &n_vite_astronave);
			collisioneNavicelle(navicelle, n_vite_navicelle);

			/*movimento del bordo inferiore*/
		for(i = 0; i < maxx+5; i++){
					pthread_mutex_lock(&mutex);
					mvprintw(y_sprite, x_sprite - i, "%c%", ' ');
					pthread_mutex_unlock(&mutex);
		}

		for(i = 0; i < maxx+5; i++) {
			if(x_sprite-i <= 0) {
					pthread_mutex_lock(&mutex);				
				mvprintw(y_sprite, (x_sprite - i)+maxx,"%c%",sprite[i]);
					pthread_mutex_unlock(&mutex);
			} else{
					pthread_mutex_lock(&mutex);
				mvprintw(y_sprite, x_sprite - i,"%c%",sprite[i]);
					pthread_mutex_unlock(&mutex);
			}

		}


		if(x_sprite <= 0) x_sprite = maxx;

		if(livello_count%livello_dim==0) x_sprite--;

		livello_count++;
		
		/*movimento bordo superiore*/
		
		for(i = 0; i < maxx+5; i++)
			mvprintw(y_sprite2, x_sprite2 - i, "%c%", ' ');


		for(i = 0; i < maxx+5; i++) {
			if(x_sprite2-i <= 0) {
					pthread_mutex_lock(&mutex);
				mvprintw(y_sprite2, (x_sprite2 - i)+maxx,"%c%",sprite2[i]);
					pthread_mutex_unlock(&mutex);
			} else{
					pthread_mutex_lock(&mutex);
				mvprintw(y_sprite2, x_sprite2 - i,"%c%",sprite2[i]);
					pthread_mutex_unlock(&mutex);
			}

		}


		if(x_sprite2 <= 0) x_sprite2 = maxx;

		if(livello_count2%livello_dim==0) x_sprite2--;

		livello_count2++;

			/* controlla se e' il momento di terminare il gioco */
		}while(!fineGioco(astronave, navicelle, n_vite_navicelle, n_vite_astronave));
}

/*************************************************************************************************
	    gestisce la stampa dell'oggetto astronave

	    parametri:
	                valore_letto -> valore letto dalla pipe
	                *astronave -> puntatore alla variabile in cui sono contenute le coordinate
	                              dell'astronave
*************************************************************************************************/
void stampaAstronave(Posizione valore_letto, Posizione *astronave) {
		int i, j;
		char sprite_astronave[DIM_SPRITE_ASTRONAVE + 1][DIM_SPRITE_ASTRONAVE + 1] = {"\\--/","/--\\"};

		/* vancella la posizione precedente */
		if (astronave->y >= 0) {
			for(i = 0; i < 2; i++)
				for(j = 0; j < DIM_SPRITE_ASTRONAVE; j++) {
					pthread_mutex_lock(&mutex);
					mvaddch(astronave->y - i, astronave->x + j, ' ');
					pthread_mutex_unlock(&mutex);
				}
		}

		/* salva la posizione da cancellare */
		*astronave = valore_letto;

		/* aggiunge il colore all'astronave */
		attron(COLOR_PAIR(COLORE_ASTRONAVE));

		/* stampa la posizione attuale */
		for(i = 0; i < 2; i++)
			for(j = 0; j < DIM_SPRITE_ASTRONAVE; j++) {
				pthread_mutex_lock(&mutex);
				mvaddch(valore_letto.y - i, valore_letto.x + j, sprite_astronave[i][j]);
				pthread_mutex_unlock(&mutex);
			}

		/* toglie il colore all'astronave */
		attroff(COLOR_PAIR(COLORE_ASTRONAVE));

		/* aggiorna lo schermo */
		pthread_mutex_lock(&mutex);
		curs_set(false);
		refresh();
		pthread_mutex_unlock(&mutex);
	}

/*************************************************************************************************
	    gestisce la stampa dell'oggetto missile

	    parametri:
	                valore_letto -> valore letto dalla pipe
	                missile[] -> vettoore in cui sono contenute le coordinate del missile
*************************************************************************************************/
void stampaMissile(Posizione valore_letto, Posizione missile[]) {

		/* cancella la posizione precedente */
		if (missile[valore_letto.cont % DIM_STAMPA_MISSILI].x != -1) {
			pthread_mutex_lock(&mutex);
			mvaddch(missile[valore_letto.cont % DIM_STAMPA_MISSILI].y,
			        missile[valore_letto.cont % DIM_STAMPA_MISSILI].x, ' ');
			pthread_mutex_unlock(&mutex);
		}

		/* aggiunge il colore al missile */
		attron(COLOR_PAIR(COLORE_MISSILI));

		/* stampa la posizione attuale */
		pthread_mutex_lock(&mutex);
		mvaddch(valore_letto.y, valore_letto.x, '*');
		pthread_mutex_unlock(&mutex);

		/* toglie il colore al missile */
		attroff(COLOR_PAIR(COLORE_MISSILI));

		/* salva nel vettore la posizione da cancellare */
		missile[valore_letto.cont % DIM_STAMPA_MISSILI] = valore_letto;

		/* aggiorna lo schermo */
		pthread_mutex_lock(&mutex);
		curs_set(0);
		refresh();
		pthread_mutex_unlock(&mutex);
	}


/*************************************************************************************************
	    gestisce la stampa dell'oggetto navicella di livello 1 e 2

	    parametri:
	                valore_letto -> valore letto dalla pipe
	                navicelle[] -> vettore in cui sono contenute le coordinate delle navicelle
	                nemiche di livello 1
	                n_vite -> vettore delle vite delle navicelle
*************************************************************************************************/
void stampaNavicelle(Posizione valore_letto, Posizione navicelle[], int n_vite[]) {
		char sprite_navicella_liv1[DIM_SPRITE_NAVICELLA_LIV1 + 1][DIM_SPRITE_NAVICELLA_LIV1 + 1] = {"\\\\\\", "///", "\\\\\\"},
		sprite_navicella_liv2[7][3] = {{"(<)"}, {"   "}, {"(>)"}, {"   "}, {"(<)"}, {"   "}, {"(>)"}},
		sprite_navicella_liv1_alt[DIM_SPRITE_NAVICELLA_LIV1 + 1][DIM_SPRITE_NAVICELLA_LIV1 + 1] = {"///", "\\\\\\", "///"},
		sprite_navicella_liv2_alt[7][3] = {{"(>)"}, {"   "}, {"(<)"}, {"   "}, {"(>)"}, {"   "}, {"(<)"}};
		int i, j;

		/* gestisce il caso della navicella di lvl 1 */
		if(valore_letto.id == ID_NAVICELLA_LIV1) {

			/* cancella la posizione precedente */
			if (navicelle[valore_letto.cont].x != -1) {
				for(i = 0; i <DIM_SPRITE_NAVICELLA_LIV1; i++)
					for(j = 0; j < DIM_SPRITE_NAVICELLA_LIV1; j++) {
						pthread_mutex_lock(&mutex);
						mvaddch(navicelle[valore_letto.cont % N_NAVICELLE].y + i, navicelle[valore_letto.cont % N_NAVICELLE].x + j, ' ');
						pthread_mutex_unlock(&mutex);
					}
			}

			/* aggiunge il colore alla navicella */
			attron(COLOR_PAIR(COLORE_NAVICELLE));

			flag_navicelle[valore_letto.cont] = !flag_navicelle[valore_letto.cont];
			/* stampa la posizione attuale */
			for(i = 0; i <DIM_SPRITE_NAVICELLA_LIV1; i++)
				for(j = 0; j < DIM_SPRITE_NAVICELLA_LIV1; j++) {
					if(flag_navicelle[valore_letto.cont]) {
						pthread_mutex_lock(&mutex);
						mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv1[i][j]);
						pthread_mutex_unlock(&mutex);
					} else {
						pthread_mutex_lock(&mutex);
						mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv1_alt[i][j]);
						pthread_mutex_unlock(&mutex);
					}
				}

			/* toglie il colore all'astronave */
			attroff(COLOR_PAIR(COLORE_NAVICELLE));

			/* salva nel vettore la posizione da cancellare */
			navicelle[valore_letto.cont % N_NAVICELLE] = valore_letto;

			/* aggiorna lo schermo */
			pthread_mutex_lock(&mutex);
			refresh();
			curs_set(0);
			pthread_mutex_unlock(&mutex);
		}

		/* gestisce il caso della navicella di lvl 2 */
		if(valore_letto.id == ID_NAVICELLA_LIV2) {

			/* cancello la posizione precedente */
			if(navicelle[valore_letto.cont].x != -1) {
				for(i = 0; i < 7; i++)
					for(j = 0; j < 3; j++) {
						pthread_mutex_lock(&mutex);
						mvaddch(navicelle[valore_letto.cont % N_NAVICELLE].y + i, navicelle[valore_letto.cont % N_NAVICELLE].x + j, ' ');
						pthread_mutex_unlock(&mutex);
					}
			}

			/* imposta in rosso la navicella di livello 2 nel caso abbia una vita e di giallo nel caso abbia due vite */
			if(n_vite[valore_letto.cont % N_NAVICELLE] == 1)
				attron(COLOR_PAIR(COLORE_BOMBE));
			else
				attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));

			flag_navicelle[valore_letto.cont] = !flag_navicelle[valore_letto.cont];
			/* stampa la posizione attuale */
			for(i = 0; i < 7; i++)
				for(j = 0; j < 3; j++) {
					if(flag_navicelle[valore_letto.cont]) {
						pthread_mutex_lock(&mutex);
						mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv2[i][j]);
						pthread_mutex_unlock(&mutex);
					} else {
						pthread_mutex_lock(&mutex);
						mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv2_alt[i][j]);
						pthread_mutex_unlock(&mutex);
					}
				}


			/* spegne il colore della navicella nel caso abbia una vita */
			if(n_vite[valore_letto.cont % N_NAVICELLE] == 1)
				attroff(COLOR_PAIR(COLORE_NAVICELLE_1UP));
			else
				attroff(COLOR_PAIR(COLORE_NAVICELLE));

			/* salva nel vettore la posizione da cancellare */
			navicelle[valore_letto.cont % N_NAVICELLE] = valore_letto;

			/* aggiorna lo schermo */
			pthread_mutex_lock(&mutex);
			curs_set(0);
			refresh();
			pthread_mutex_unlock(&mutex);

	}
}

/*************************************************************************************************
	    gestisce la stampa dell'oggetto bomba

	    parametri:
	                valore_letto -> valore letto dalla pipe
	                bombe[] ->vettore in cui sono contenute le coordinate delle bombe
*************************************************************************************************/
void stampaBomba(Posizione valore_letto, Posizione bombe[]) {

		/* cancella la posizione precedente */
		if (bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x != -1) {
			pthread_mutex_lock(&mutex);
			mvaddch(bombe[valore_letto.cont % DIM_STAMPA_BOMBE].y,
			bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x, ' ');
			pthread_mutex_unlock(&mutex);
		}

		/* aggiunge il colore della bomba */
		attron(COLOR_PAIR(COLORE_BOMBE));


		pthread_mutex_lock(&mutex);
		/* stampa la posizione attuale */
		mvaddch(valore_letto.y, valore_letto.x, 'o');
		pthread_mutex_unlock(&mutex);


		/* toglie il colore della bomba */
		attroff(COLOR_PAIR(COLORE_BOMBE));

		/* salva nel vettore la posizione da cancellare */
		bombe[valore_letto.cont % DIM_STAMPA_BOMBE] = valore_letto;

		/* aggiorna lo schermo */
		pthread_mutex_lock(&mutex);
		curs_set(0);
		refresh();
		pthread_mutex_unlock(&mutex);
}

/*************************************************************************************************
	    gestisce la stampa della scritta 'space defender' a sinistra e il numero delle vite mancanti a destra
	    parametri:
	                m_vite_astronave -> numero delle vite dell'astronave
*************************************************************************************************/
void stampaVite(int n_vite_astronave) {
		int i, j;


		char uno[6][45]= {
			" _  __     _____ _____  _     ",
			"/ | \\ \\   / /_ _|_   _|/ \\    ",
			"| |  \\ \\ / / | |  | | / _ \\   ",
			"| |   \\ V /  | |  | |/ ___ \\  ",
			"|_|    \\_/  |___| |_/_/   \\_\\ ",
			"                               "
		};

		char due[6][45]= {

			"____   __     _____ _____ _____   ",
			" |___ \\  \\ \\   / /_ _|_   _| ____|  ",
			"   __) |  \\ \\ / / | |  | | |  _|    ",
			"  / __/    \\ V /  | |  | | | |___   ",
			" |_____|    \\_/  |___| |_| |_____|  ",
			"                                      "
		};

		char tre[6][45]= {

			" _____ __     _____ _____ _____  ",
			"|___ / \\ \\   / /_ _|_   _| ____| ",
			"    |_ \\  \\ \\ / / | |  | | |  _|   ",
			"  ___)|   \\ V /  | |  | | | |___  ",
			" |____/    \\_/  |___| |_| |_____| ",
			"                                      "
		};

		char space[6][55]= {
			" ____  ____   _    ____ _____  ",
			"/ ___||  _ \\ / \\  / ___| ____| ",
			"\\___ \\| |_) / _ \\| |   |  _|   ",
			" ___) |  __/ ___ \\ |___| |___  ",
			"|____/|_| /_/   \\_\\____|_____| ",
			"                                "
		};

		char defe[6][45]= {
			" ____  _____ _____ _____ ",
			"|  _ \\| ____|  ___| ____| ",
			"| | | |  _| | |_  |  _|  ",
			"| |_| | |___|  _| | |___ ",
			"|____/|_____|_|   |_____| ",
			"                           "
		};

		char nder[6][45]= {
			" _   _ ____  _____ ____   ",
			"| \\ | |  _ \\| ____|  _ \\  ",
			"|  \\| | | | |  _| | |_) | ",
			"| |\\  | |_| | |___|  _ <  ",
			"|_| \\_|____/|_____|_| \\_\\ ",
			"                             "
		};

		/* stampa scritta 'space' */

		attron(COLOR_PAIR(COLORE_NAVICELLE));
		for(i=0; i<6; i++)
			for(j=0; j<strlen(space[i]); j++) {
				pthread_mutex_lock(&mutex);
				mvaddch(((maxy/2)-8 )+i, 1 +j, space[i][j]);
				pthread_mutex_unlock(&mutex);

			}


		/* stampa scritta 'defe' */

		attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));
		for(i=0; i<6; i++)
			for(j=0; j<strlen(defe[i]); j++) {
				pthread_mutex_lock(&mutex);
				mvaddch(((maxy/2)-2 )+i, 3+j, defe[i][j]);
				pthread_mutex_unlock(&mutex);
			}

		/* stampa scritta 'nder' */

		attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));
		for(i=0; i<6; i++)
			for(j=0; j<strlen(nder[i]); j++) {
				pthread_mutex_lock(&mutex);
				mvaddch(((maxy/2)+3 )+i,3+j, nder[i][j]);

				pthread_mutex_unlock(&mutex);
			}


		/*stampa vite in base alla variabile n_vite_astronave*/
		if(n_vite_astronave==3) {

			attron(COLOR_PAIR(COLORE_NAVICELLE));
			for(i=0; i<6; i++)
				for(j=0; j<strlen(tre[i]); j++) {

					pthread_mutex_lock(&mutex);

					mvaddch(((maxy/2)-3 )+i, maxx-strlen(tre[i]) + j, tre[i][j]);

					pthread_mutex_unlock(&mutex);
				}
		}

		if(n_vite_astronave==2) {


			for(i=0; i<6; i++)
				for(j=0; j<strlen(tre[i]); j++) {
					pthread_mutex_lock(&mutex);
					mvaddch(((maxy/2)-3 )+i, maxx-strlen(tre[i]) + j, ' ');
					pthread_mutex_unlock(&mutex);
				}


			attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));
			for(i=0; i<6; i++)
				for(j=0; j<strlen(due[i]); j++)
					mvaddch(((maxy/2)-3 )+i, maxx-strlen(due[i]) + j, due[i][j]);

		}


		if(n_vite_astronave==1) {


			for(i=0; i<6; i++)
				for(j=0; j<strlen(due[i]); j++){

					pthread_mutex_lock(&mutex);

					mvaddch(((maxy/2)-3 )+i, maxx-strlen(due[i]) + j, ' ');

					pthread_mutex_unlock(&mutex);
				}


			attron(COLOR_PAIR(COLORE_BOMBE));
			for(i=0; i<6; i++)
				for(j=0; j<strlen(uno[i]); j++){

					pthread_mutex_lock(&mutex);

					mvaddch(((maxy/2)-3 )+i, maxx-strlen(uno[i]) + j, uno[i][j]);

					pthread_mutex_unlock(&mutex);
				}
		}

}

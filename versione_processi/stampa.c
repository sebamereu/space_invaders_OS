#include "stampa.h"

/*************************************************************************************************
    gestisce le stampe di tutti gli oggetti presenti a schermo, sfruttando le funzioni apposite

    parametri:
                pipein -> pipe da cui leggere le posizioni degli oggetti
                pipeout -> pipe su cui scrivere
*************************************************************************************************/
void stampaGioco(int pipein, int pipeout) {
	Posizione valore_letto, astronave, missile_dw[DIM_STAMPA_MISSILI], missile_up[DIM_STAMPA_MISSILI],
	          navicelle[N_NAVICELLE], bombe[DIM_STAMPA_BOMBE];
	int i, livello_dim, livello_count2=0, livello_count=0,x_sprite, y_sprite,x_sprite2, y_sprite2, n_vite_astronave = N_VITE_ASTRONAVE, n_vite_navicelle[N_NAVICELLE], punteggio = 0;
	int prova=0;

	/*assegnazione cordinate bordo superiore*/
	x_sprite2=maxx+5;  
	y_sprite2=0;

	/*assegnazione cordinate bordo inferiore*/
	x_sprite=maxx+5;
	y_sprite=maxy-1;
	
	
	char sprite1[]= "*  ";

	
	/*creazione sprite della dimensione dello schermo */
	char sprite2[BORDO];

	for(i=0; i< BORDO ; i++) {
		strcat(sprite2, sprite1); 
	}

	/*creazione sprite della dimensione dello schermo */
	char sprite[BORDO];

	for(i=0; i< BORDO ; i++) {
		strcat(sprite, sprite1);
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

	for(i = 0; i < DIM_STAMPA_BOMBE; i++) {
		bombe[i].x = -1;
	}

	for (i = 0; i < N_NAVICELLE; i++) {
		n_vite_navicelle[i] = N_VITE_NAV_LIV2;
	}

	do {

		/* leggo dalla pipe il valore */
		read(pipein, &valore_letto, sizeof(valore_letto));

		/* in base all'id stampa uno degli oggetti */
		switch(valore_letto.id) {
		case ID_ASTRONAVE:

			stampaAstronave(valore_letto, &astronave);
			break;

		case ID_MISSILE_DW:
			stampaMissile(valore_letto, missile_dw);
			break;

		case ID_MISSILE_UP:
			stampaMissile(valore_letto, missile_up);
			break;

		case ID_NAVICELLA_LIV1:
			stampaNavicelle(valore_letto, navicelle, n_vite_navicelle);
			break;

		case ID_NAVICELLA_LIV2:
			stampaNavicelle(valore_letto, navicelle, n_vite_navicelle);
			break;

		case ID_BOMBA:
			stampaBomba(valore_letto, bombe);
			if(bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x<=maxx/2-25)
				mvaddch(bombe[valore_letto.cont % DIM_STAMPA_BOMBE].y,
				        bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x, ' ');
			break;
		}
		/* controlla tutte le collisioni */
		collisioneMissiliNavicelle(pipeout, missile_dw, navicelle, n_vite_navicelle, &punteggio);
		collisioneMissiliNavicelle(pipeout, missile_up, navicelle, n_vite_navicelle, &punteggio);
		collisioniBombaAstronave(bombe, astronave, &n_vite_astronave);
		collisioneBombaMissile(bombe, missile_dw, &n_vite_astronave);
		collisioneBombaMissile(bombe, missile_up, &n_vite_astronave);
		collisioneNavicelle(navicelle, n_vite_navicelle);

		/*movimento del bordo inferiore*/
		for(i = 0; i < maxx+5; i++)
			mvprintw(y_sprite, x_sprite - i, "%c%", ' ');


		for(i = 0; i < maxx+5; i++) {
			if(x_sprite-i <= 0) {
				mvprintw(y_sprite, (x_sprite - i)+maxx,"%c%",sprite[i]);
			} else
				mvprintw(y_sprite, x_sprite - i,"%c%",sprite[i]);

		}

		/* appena lo sprite originale raggiunge il limite sx dello schermo, verrà trasportato nel limite dx, stile effetto pac-man*/
		if(x_sprite <= 0) x_sprite = maxx;

		/*velocità di movimento del bordo*/
		if(livello_count%livello_dim==0) x_sprite--;

		/* incremento contatore per il movimento del bordo*/
		livello_count++;
		
		/*movimento bordo superiore*/
		
		for(i = 0; i < maxx+5; i++)
			mvprintw(y_sprite2, x_sprite2 - i, "%c%", ' ');


		for(i = 0; i < maxx+6; i++) {
			if(x_sprite2-i <= 0) {
				mvprintw(y_sprite2, (x_sprite2 - i)+maxx,"%c%",sprite2[i]);
			} else
				mvprintw(y_sprite2, x_sprite2 - i,"%c%",sprite2[i]);

		}


		/* appena lo sprite originale raggiunge il limite sx dello schermo, verrà trasportato nel limite dx, stile effetto pac-man*/
		if(x_sprite2 <= 0) x_sprite2 = maxx;

		/*velocità di movimento del bordo*/
		if(livello_count2%livello_dim==0) x_sprite2--;

		/* incremento contatore per il movimento del bordo*/
		livello_count2++;
		
		/*modalità di stampa diversa in base alla dimensione della finestra */
		if(maxx > 90 || maxy > 26) {
			stampaVite(n_vite_astronave);
		} else {
			mvprintw(1, maxx - strlen("Vite: %d"), "Vite: %d\n", n_vite_astronave);
		}
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


	attron(COLOR_PAIR(COLORE_ASTRONAVE));

	/* cancella la posizione precedente */
	if (astronave->y >= 0) {
		for(i = 0; i < 2; i++)
			for(j = 0; j <DIM_SPRITE_ASTRONAVE; j++)
				mvaddch(astronave->y - i, astronave->x + j, ' ');
	}

	/* salva nel vettore la posizione da cancellare */
	*astronave = valore_letto;

	attron(COLOR_PAIR(COLORE_ASTRONAVE));

	/* stampa la posizione attuale */
	for(i = 0; i < 2; i++)
		for(j = 0; j < DIM_SPRITE_ASTRONAVE; j++)
			mvaddch(valore_letto.y - i, valore_letto.x + j, sprite_astronave[i][j]);

	attroff(COLOR_PAIR(COLORE_ASTRONAVE));

	curs_set(false);
	refresh();
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
		mvaddch(missile[valore_letto.cont % DIM_STAMPA_MISSILI].y,
		        missile[valore_letto.cont % DIM_STAMPA_MISSILI].x, ' ');
	}

	attron(COLOR_PAIR(COLORE_MISSILI));

	/* stampa la posizione attuale */
	mvaddch(valore_letto.y, valore_letto.x, '*');

	attroff(COLOR_PAIR(COLORE_MISSILI));

	/* salva nel vettore la posizione da cancellare */
	missile[valore_letto.cont % DIM_STAMPA_MISSILI] = valore_letto;

	curs_set(0);
	refresh();
}


/*************************************************************************************************
    gestisce la stampa dell'oggetto navicella di livello 1 e 2

    parametri:
                valore_letto -> valore letto dalla pipe
                navicelle[] -> vettore in cui sono contenute le coordinate delle navicelle
                nemiche di livello 1
                n_vite[] -> vettore delle vite delle navicelle
*************************************************************************************************/
void stampaNavicelle(Posizione valore_letto, Posizione navicelle[], int n_vite[]) {
	char sprite_navicella_liv1[DIM_SPRITE_NAVICELLA_LIV1 + 1][DIM_SPRITE_NAVICELLA_LIV1 + 1] = {"\\\\\\", "///", "\\\\\\"},
	        sprite_navicella_liv1_alt[DIM_SPRITE_NAVICELLA_LIV1 + 1][DIM_SPRITE_NAVICELLA_LIV1 + 1] = {"///", "\\\\\\", "///"},
	sprite_navicella_liv2[7][3] = {{"(<)"}, {"   "}, {"(>)"}, {"   "}, {"(<)"}, {"   "}, {"(>)"}},
	sprite_navicella_liv2_alt[7][3] = {{"(>)"}, {"   "}, {"(<)"}, {"   "}, {"(>)"}, {"   "}, {"(<)"}};
	int i, j;

	/* gestisce il caso della navicella di lvl 1 */
	if(valore_letto.id == ID_NAVICELLA_LIV1) {

		/* cancella la posizione precedente */
		if (navicelle[valore_letto.cont].x != -1) {
			for(i = 0; i <DIM_SPRITE_NAVICELLA_LIV1; i++)
				for(j = 0; j < DIM_SPRITE_NAVICELLA_LIV1; j++)
					mvaddch(navicelle[valore_letto.cont % N_NAVICELLE].y + i, navicelle[valore_letto.cont % N_NAVICELLE].x + j, ' ');
		}

		attron(COLOR_PAIR(COLORE_NAVICELLE));

		flag_navicelle[valore_letto.cont] = !flag_navicelle[valore_letto.cont];
		/* stampa la posizione attuale */
		for(i = 0; i <DIM_SPRITE_NAVICELLA_LIV1; i++)
			for(j = 0; j < DIM_SPRITE_NAVICELLA_LIV1; j++) {
				if(flag_navicelle[valore_letto.cont])
					mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv1[i][j]);
				else
					mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv1_alt[i][j]);
			}


		attroff(COLOR_PAIR(COLORE_NAVICELLE));

		/* salva nel vettore la posizione da cancellare */
		navicelle[valore_letto.cont % N_NAVICELLE] = valore_letto;

		curs_set(0);
		refresh();
	}

	/* gestisce il caso della navicella di lvl 2 */
	if(valore_letto.id == ID_NAVICELLA_LIV2) {

		/* cancello la posizione precedente */
		if(navicelle[valore_letto.cont].x != -1) {
			for(i = 0; i < 7; i++)
				for(j = 0; j < 3; j++)
					mvaddch(navicelle[valore_letto.cont % N_NAVICELLE].y + i, navicelle[valore_letto.cont % N_NAVICELLE].x + j, ' ');
		}

		/* imposta in rosso la navicella di secondo livello nel caso abbia una vita e in giallo nel caso abbia due vite */
		if(n_vite[valore_letto.cont % N_NAVICELLE] == 1)
			attron(COLOR_PAIR(COLORE_BOMBE));
		else
			attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));


		flag_navicelle[valore_letto.cont] = !flag_navicelle[valore_letto.cont];

		/* stampa la posizione attuale */
		for(i = 0; i < 7; i++)
			for(j = 0; j < 3; j++) {
				if(flag_navicelle[valore_letto.cont])
					mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv2[i][j]);
				else
					mvaddch(valore_letto.y + i, valore_letto.x + j, sprite_navicella_liv2_alt[i][j]);
			}


		/* salva nel vettore la posizione da cancellare */
		navicelle[valore_letto.cont % N_NAVICELLE] = valore_letto;

		curs_set(0);
		refresh();
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
	if (bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x != -1 ) {
		mvaddch(bombe[valore_letto.cont % DIM_STAMPA_BOMBE].y,
		bombe[valore_letto.cont % DIM_STAMPA_BOMBE].x, ' ');
	}

	/* accende il colore della bomba */
	attron(COLOR_PAIR(COLORE_BOMBE));

	/* stampa la posizione attuale */
	mvaddch(valore_letto.y, valore_letto.x, 'o');

	/* spegne il colore della bomba */
	attroff(COLOR_PAIR(COLORE_BOMBE));

	/* salva nel vettore la posizione da cancellare */
	bombe[valore_letto.cont % DIM_STAMPA_BOMBE] = valore_letto;

	curs_set(0);
	refresh();
}


/*************************************************************************************************
    gestisce il ridimensionamento dello schermo per l'avvio della partita

*************************************************************************************************/
void dimensione_schermo() {
	char avviso[] = {"Per un miglior svolgimento del gioco\n"}, avviso2[] = {"ridimensiona schermo o premi SPAZIO per continuare.\n"},
	avviso3[] = {" \n"}, scelta;


	while(1) {
		getmaxyx(stdscr, maxy, maxx);

		if(maxx < 90 || maxy < 26) {
			clear();
			mvprintw(get_center_y(0), get_center_x(strlen(avviso)), "%s", avviso);
			mvprintw(get_center_y(-2), get_center_x(strlen(avviso2)), "%s", avviso2);
			refresh();
		} else{

			clear();
			getmaxyx(stdscr, maxy, maxx);
			break;

		}

		scelta = getch();
		if(scelta == BARRA_SPAZIO && maxy >= 20) {
			clear();
			break;
		} else if (scelta == BARRA_SPAZIO && maxy < 20) {
			mvprintw(maxy-2, get_center_x(strlen(avviso3)), "%s", avviso3);
			refresh();
			
		}

	}
}

/*************************************************************************************************
    restituisce il centro dello schermo in base alle dimensioni dell'oggetto

    parametri:
                x -> coordinata della prima occorrenza della stampa
*************************************************************************************************/
int get_center_x(int x) {
	return (maxx / 2) - (x / 2);
}


/*************************************************************************************************
    restituisce il centro dello schermo in base alle dimensioni dell'oggetto

    parametri:
                y -> coordinata della prima occorrenza della stampa
*************************************************************************************************/
int get_center_y(int y) {
	return (maxy / 2) - (y / 2);
}

/*************************************************************************************************
    stampa a video la scritta "space defender" a sinistra e il numero delle vite mancanti a destra

    parametri:
                n_vite_astronave -> numero delle vite dell'astronave
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
		for(j=0; j<strlen(space[i]); j++)
			mvaddch(((maxy/2)-8 )+i, 1 +j, space[i][j]);


	/* stampa scritta 'defe' */
	
		attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));
	for(i=0; i<6; i++)
		for(j=0; j<strlen(defe[i]); j++)
			mvaddch(((maxy/2)-2 )+i, 3+j, defe[i][j]);

	/* stampa scritta 'nder' */
	
		attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));
	for(i=0; i<6; i++)
		for(j=0; j<strlen(nder[i]); j++)
			mvaddch(((maxy/2)+3 )+i,3+j, nder[i][j]);

	
	
	/* stampa scritta delle vite in base alla variabile n_vite_astronave */
	if( n_vite_astronave==3) {

		attron(COLOR_PAIR(COLORE_NAVICELLE));
		for(i=0; i<6; i++)
			for(j=0; j<strlen(tre[i]); j++)
				mvaddch(((maxy/2)-3 )+i, maxx-strlen(tre[i]) + j, tre[i][j]);

	}

	if( n_vite_astronave==2) {


		for(i=0; i<6; i++)
			for(j=0; j<strlen(tre[i]); j++)
				mvaddch(((maxy/2)-3 )+i, maxx-strlen(tre[i]) + j, ' ');



		attron(COLOR_PAIR(COLORE_NAVICELLE_1UP));
		for(i=0; i<6; i++)
			for(j=0; j<strlen(due[i]); j++)
				mvaddch(((maxy/2)-3 )+i, maxx-strlen(due[i]) + j, due[i][j]);

	}


	if( n_vite_astronave==1) {


		for(i=0; i<6; i++)
			for(j=0; j<strlen(due[i]); j++)
				mvaddch(((maxy/2)-3 )+i, (maxx-strlen(due[i])) + j, ' ');



		attron(COLOR_PAIR(COLORE_BOMBE));
		for(i=0; i<6; i++)
			for(j=0; j<strlen(uno[i]); j++)
				mvaddch(((maxy/2)-3 )+i, (maxx-strlen(uno[i])) + j, uno[i][j]);

	}

}

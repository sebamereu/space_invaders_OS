#include "gestione.h"


/*************************************************************************************************
    controlla il movimento dell'astronave e ne scrive le coordinate sulla pipe

    parametri:
                pipeout -> pipe su cui scrivere le coordinate dell'astronave
*************************************************************************************************/
void creaAstronave(int pipeout) {
	int c, pid_missile_dw, pid_missile_up, timer_missile = 0;
	Posizione astronave, missile;

	//inizializzo la variabile astronave
	astronave.id = ID_ASTRONAVE;
	astronave.x = maxx/2 - INIZIO_ASTRONAVE;
	astronave.y = maxy/2;
	missile.cont = 0;

	//scrivo sulla pipe la posizione iniziale
	write(pipeout, &astronave, sizeof(astronave));

	while(true) {

		/* se il risultato e' diverso da 0 non incrementa per dare la possibilita' di sparare un missile */
		if(timer_missile % CICLI_MISSILI != 0)
			timer_missile++;

		/* imposto il limite dello schermo secondo la dimensione dello sprite */
		if(astronave.y >= maxy - DIM_SPRITE_ASTRONAVE)
			astronave.y = maxy - DIM_SPRITE_ASTRONAVE;

		switch(c = getch()) {
			/* nel caso si prema la freccia su, l'astronave fa un passo in meno sull'asse delle y */
		case KEY_UP:
			if(astronave.y >= MINY+ DIM_SPRITE_ASTRONAVE-1)
				astronave.y -= 1;
			break;

			/* nel caso si prema la freccia giu, l'astronave fa un passo in piu' sull'asse delle y*/
		case KEY_DOWN:
			if(astronave.y <= maxy)
				astronave.y += 1;
			break;

			/* nel caso si prema la barra spaziatrice, vengono creati due processi per la gestione dei missili */
		case BARRA_SPAZIO:
			/* se il risultato e' 0 allora si può sparare */
			if(timer_missile % CICLI_MISSILI == 0) {
				/* creazione processo missile basso */
				switch(pid_missile_dw = fork()) {
				case -1:
					perror("fork failed\n");
					break;

				case 0:
					missile.pid = getpid();
					movimentoMissile(ID_MISSILE_DW, pipeout, astronave, missile);
					kill(missile.pid, SIGHUP);
					_exit(1);
				}

				/* creazione processo missile sopra */
				switch(pid_missile_up = fork()) {
				case -1:
					perror("fork failed\n");
					exit(1);

				case 0:
					missile.pid = getpid();
					movimentoMissile(ID_MISSILE_UP, pipeout, astronave, missile);
					kill(missile.pid, SIGHUP);
					_exit(1);
				}
				missile.cont++;         //aumenta il contatore dei missili creati
				timer_missile++;
			}
			break;

		default:
			break;
		}

		//scrivo sulla pipe la posizione al momento della pressione di un tasto
		write(pipeout, &astronave, sizeof(astronave));
	}
}

/*************************************************************************************************
    crea il missile e aggiorna la sua posizione fino a quando non tocca il bordo superiore

    parametri:
                missile_id -> id del missile che rappresenta se dx o sx
                pipeout -> pipe su cui scrivere le coordinate dell'astronave
                astronave_x -> coordinata x dell'astronave
                missile -> missile su cui operare
*************************************************************************************************/
void movimentoMissile(int missile_id, int pipeout, Posizione astronave, Posizione missile) {

	/* inizializziamo variabile missile nel punto centrale dell'astronave */
	

	/* caso missile dw */
	if (missile_id == ID_MISSILE_DW) {
		missile.id = ID_MISSILE_DW;
		missile.x = astronave.x+2;
		missile.y = astronave.y;
		do {
			missile.x+=3;
			missile.y++;

			write(pipeout, &missile, sizeof(missile));
			usleep(50000);
		} while(missile.y >= 0);
	}

	/* caso missile up */
	if (missile_id == ID_MISSILE_UP) {
		missile.id = ID_MISSILE_UP;
		missile.x = astronave.x+2;
		missile.y = astronave.y-1;
		do {
			missile.x+=3;
			missile.y--;

			write(pipeout, &missile, sizeof(missile));
			usleep(50000);
		} while(missile.y >= 0);
	}
}

/*************************************************************************************************
    crea il movimento della navicella nemica di livello 1 e 2 nella posizione x, y
    assegnate tramite gli argomenti della funzione

    parametri:
                pipeout -> pipe su cui scrivere le coordinate della navicella di livello 1
                navicella -> navicella su cui operare
*************************************************************************************************/
void movimentoNavicella(int pipeout, Posizione navicella) {
	int diry, timer_discesa = 1, timer_bomba = 1, r_movimento,
	          r_bomba, limite[N_NAV_PER_COLONNA], i ;

	Posizione bomba;

	/* inizializzo il contatore di bombe a 0 */
	bomba.cont = navicella.cont;


	/*ciclo for per creare i vari limiti per ogni navicella*/
	for(i = 0; i < N_NAV_PER_COLONNA; i++)
		limite[i] = i * maxy / N_NAV_PER_COLONNA;
		
	limite[N_NAV_PER_COLONNA]=maxy;

	/* divido le navicelle su tre colonne */
	if(navicella.id == ID_NAVICELLA_LIV1) {
        if(navicella.cont>=0 && navicella.cont < N_NAV_PER_COLONNA)
            navicella.x = (maxx/2)+POS_COLONNA_1;
        else if(navicella.cont >= N_NAV_PER_COLONNA && navicella.cont < N_NAV_PER_COLONNA * 2)
            navicella.x = (maxx/2)+POS_COLONNA_2;
        else 
            navicella.x = (maxx/2)+POS_COLONNA_3;
				
			
	}

	write(pipeout, &navicella, sizeof(navicella));

	r_movimento = MIN_GEN_MOV + rand() % (MAX_GEN_MOV - MIN_GEN_MOV + 1);           //genera un numero random per il movimento del movimento
	r_bomba = MIN_GEN_BOMBA + rand() % (MAX_GEN_BOMBA - MIN_GEN_BOMBA + 1);         //genera un numero random per la generazione del timer bomba

	/* decide il movimento della navicella */
	if(r_movimento % 2 == 0 )
		diry = 1;
	else
		diry = -1;

	while(true) {
		/* sposta di una colonna la posizione delle navicelle */
		if (timer_discesa % CICLI_DISCESA == 0) {
			navicella.x -= DIMENSIONE_COLONNA;
		}

		/* limita il movimento delle navicelle ad una sezione specifica di schermo, garantendo un effetto
		   rimbalzo nel caso in cui due navicelle arrivassero allo stesso limite */
		switch (navicella.cont % N_NAV_PER_COLONNA) {
		case 0:
			if(navicella.id == ID_NAVICELLA_LIV1) {
				if (navicella.y + diry >= limite[1] - 
				DIM_SPRITE_NAVICELLA_LIV1 || navicella.y + diry <= MINY)
					diry *= -1;
			} else {
				if (navicella.y + diry >= limite[1] -
				        DIM_SPRITE_NAVICELLA_LIV2 || navicella.y + diry <= MINY)
					diry *= -1;
			}
			break;

		case 1:
			if(navicella.id == ID_NAVICELLA_LIV1) {

				if (navicella.y + diry >= maxy  - 
				DIM_SPRITE_NAVICELLA_LIV1 || navicella.y + diry < limite[1])
					diry *= -1;
			} else {
				if (navicella.y + diry >= maxy -
				DIM_SPRITE_NAVICELLA_LIV2 || navicella.y + diry < limite[1])
					diry *= -1;
			}
			break;

		case 2:
			if(navicella.id == ID_NAVICELLA_LIV1) {
				if (navicella.y + diry >= limite[3] - DIM_SPRITE_NAVICELLA_LIV1 || navicella.y + diry <= limite[2])
					diry *= -1;
			} else {
				if (navicella.y + diry >= limite[3] - DIM_SPRITE_NAVICELLA_LIV2 || navicella.y + diry <= limite[2])
					diry *= -1;
			}
			break;


		case 3:
			if(navicella.id == ID_NAVICELLA_LIV1) {
				if (navicella.y + diry >= maxy - DIM_SPRITE_NAVICELLA_LIV1 || navicella.y + diry <= limite[3])
					diry *= -1;
			} else {
				if (navicella.y + diry >= maxy - DIM_SPRITE_NAVICELLA_LIV2 || navicella.y + diry <= limite[3])
					diry *= -1;
			}
			break;
		default:
			break;
		}

		navicella.y += diry;        //incrementa la posizione sulle y

		write(pipeout, &navicella, sizeof(navicella));

		/* crea la bomba a distanza di r_bomba cicli */
		  if(timer_bomba % r_bomba == 0){
		      switch (fork()){
		          case -1:
		              perror("fork failed\n");
		              exit(1);

		          case 0:
		              bomba.pid = getpid();
		              movimentoBomba(pipeout, navicella, bomba);
		              kill(bomba.pid, SIGHUP);
		              _exit(1);
		      }
		  }

		timer_discesa++;
		timer_bomba++;
		usleep(DELAY_NAVICELLA );
	}
}

/*************************************************************************************************
    crea le navicelle di livello 1

    parametri:
                pipeout -> pipe su cui scrivere le coordinate della navicella di livello 1
*************************************************************************************************/
void navicelleLiv1(int pipeout) {
	Posizione navicella;
	pid_t pid_navicella;
	int i, a = 0, distanza, limite[N_NAV_PER_COLONNA], max_gen_distanza = maxy/N_NAV_PER_COLONNA;


	/*ciclo for per creare i vari limiti per ogni navicella*/
	for(i = 0; i < N_NAV_PER_COLONNA; i++)
		limite[i] = i * maxy / N_NAV_PER_COLONNA;
		
	limite[N_NAV_PER_COLONNA]=maxy;

	/* inizializzo variabile navicella di livello 1 */
	navicella.id = ID_NAVICELLA_LIV1;
	navicella.cont = 0;

	for(i = 0; i < N_NAVICELLE; i++) {
		switch(pid_navicella = fork()) {
		case -1:
			perror("fork failed\n");
			exit(1);

		case 0:
			srand(time(NULL) + getpid());       //inizializza il seed per la generazione casuale

			navicella.pid = getpid();

			/* assegna ad ogni navicella la propria posizione nello schermo */
			switch(navicella.cont % N_NAV_PER_COLONNA) {
			case 0:
				do {
					distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
					navicella.y = MINY + distanza - DIM_SPRITE_NAVICELLA_LIV1;
				} while(navicella.y <= MINY || navicella.y >= limite[1]);
				break;

			case 1:
				do {
					distanza = MIN_GEN_DISTANZA+ rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
					navicella.y = limite[1] + distanza + DIM_SPRITE_NAVICELLA_LIV1;
				} while(navicella.y < limite[1] || navicella.y > limite[2]);
				break;

			case 2:
				do {
					distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
					navicella.y = limite[2] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
				} while(navicella.y <= limite[2] || navicella.y > limite[3]);
				break;

			case 3:
				do {
					distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
					navicella.y = limite[3] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
				} while(navicella.y <= limite[3] || navicella.y > limite[4]);
				break;

			case 4:
				do {
					distanza = MIN_GEN_DISTANZA + rand() % (max_gen_distanza - MIN_GEN_DISTANZA + 1);
					navicella.y = limite[4] + distanza - DIM_SPRITE_NAVICELLA_LIV1;
				} while(navicella.y <= limite[4] || navicella.y >= maxy);
				break;
			}

			movimentoNavicella(pipeout, navicella);
			_exit(1);
		}

		navicella.cont++;       //incrementa il contatore delle navicelle create
	}
}


/*************************************************************************************************
    gestisce il movimento della bomba e lo scrive sulla pipe

    parametri:
                pipeout -> pipe su cui scrivere le coordinate della navicella di livello 1
                navicella -> posizione della navicella 
                 bomba -> posizione della bomba
*************************************************************************************************/
void movimentoBomba(int pipeout, Posizione navicella, Posizione bomba){
    bomba.id = ID_BOMBA;
    bomba.x = navicella.x -1;
    bomba.y = navicella.y + DIM_SPRITE_NAVICELLA_LIV1/2;

    do{
        bomba.x--;
        write(pipeout, &bomba, sizeof(bomba));

        usleep(170000);
    }while(bomba.x > maxx/2-30);
}

/*************************************************************************************************
    crea le navicelle di livello 2

    parametri:
                pipeout -> pipe su cui scrivere le coordinate della navicella di livello 1
                navicella -> navicella da passare alla funzione movimento
*************************************************************************************************/
void navicellaLiv2(int pipeout, Posizione navicella){
    pid_t pid_navicella;
    int limite[N_NAV_PER_COLONNA], i;

    navicella.id = ID_NAVICELLA_LIV2;       //assegna il giusto id alla navicella di livello 2

	/*ciclo for per creare i vari limiti per ogni navicella*/
    for(i = 0; i < N_NAV_PER_COLONNA; i++)
        limite[i] = i * maxy / N_NAV_PER_COLONNA;
		
	limite[N_NAV_PER_COLONNA]=maxy;		

    /* crea il processo per la navicella di livello 2 */
    switch (pid_navicella = fork()){
        case -1:
            perror("fork failed\n");
            exit(1);
        
        case 0:
            navicella.pid = getpid();       //prende il pid del processo
            
            /* imposta i nuovi limiti per la navicella di livello 2 */
            switch (navicella.cont % N_NAV_PER_COLONNA){
                case 0:
                    if(navicella.y + DIM_SPRITE_NAVICELLA_LIV2 > limite[1])
                        navicella.y = limite[1] - DIM_SPRITE_NAVICELLA_LIV2;
                    break;
                
                case 1:
                    if(navicella.y + DIM_SPRITE_NAVICELLA_LIV2 > maxy)
                        navicella.y = limite[2] - DIM_SPRITE_NAVICELLA_LIV2;
                    break;
                
                case 2:
                    if(navicella.y + DIM_SPRITE_NAVICELLA_LIV2 > limite[3])
                        navicella.y = limite[3] - DIM_SPRITE_NAVICELLA_LIV2;
                    break;
                
                case 3:
                    if(navicella.y + DIM_SPRITE_NAVICELLA_LIV2 > limite[4])
                        navicella.y = limite[4] - DIM_SPRITE_NAVICELLA_LIV2;
                    break;
                
                case 4:
                    if(navicella.y + DIM_SPRITE_NAVICELLA_LIV2 > maxy)
                        navicella.y = maxy - DIM_SPRITE_NAVICELLA_LIV2;
                    break;
                
                default:
                    break;
                }
            
            movimentoNavicella(pipeout, navicella);

            _exit(1);
    }
}

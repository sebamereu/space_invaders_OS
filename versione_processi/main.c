#include "gestione.h"
#include "stampa.h"

int main() {
	int p[2];
	pid_t pid_astronave, pid_gestione;  // inizializzazione PID 

	/* inizializza schermo */
	initscr(); 
	noecho();
	curs_set(false);
	keypad(stdscr, true);
	start_color();

	/* funzioni per inizializzazione dei colori */
	init_pair(COLORE_BOMBE, COLOR_RED, COLOR_BLACK);
	init_pair(COLORE_NAVICELLE, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLORE_NAVICELLE_1UP, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLORE_ASTRONAVE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLORE_MISSILI, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLORE_PUNTEGGIO, COLOR_WHITE, COLOR_BLACK);
	
	dimensione_schermo();  // invita l'utente a giocare con la finestra grande 
	
	getmaxyx(stdscr, maxy, maxx); 
	

	timeout(1);
	flushinp(); //pulisce il buffer input


	/* creazione p e controllo */
	if(pipe(p) == -1) {
		perror("p failed\n");
		exit(1);
	}

	/* forka il figlio astronave */
	switch (pid_astronave = fork()) {
	case -1:
		perror("fork failed\n");
		exit(1);

	case 0:
		close(p[0]);
		creaAstronave(p[1]);
		_exit(1);
		
	default:
		/* forka il figlio gestione */
		switch(pid_gestione = fork()) {
		case -1:
			perror("fork failed\n");
			exit(1);

		case 0:
			close(p[0]);
			navicelleLiv1(p[1]);
			_exit(1);

		default:
			stampaGioco(p[0], p[1]);
			break;
		}
		break;
	}

	/* uccide i processi figli */
	kill(pid_astronave, SIGHUP);
	kill(pid_gestione, SIGHUP);
	
	endwin();   //termina schermata
	return 0;
}

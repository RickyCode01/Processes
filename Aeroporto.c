#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include "Aeroporto.h"
#include "Torre.c"
#include "Hangar.c"

void sigHandler(/*int sig, siginfo_t *si, void *uap*/){
	//printf("sending process ID:%d\n", si->si_pid);
}

void print_Event(char* source, char* description, bool newline){
	//fflush(stdout);
	time_t now; // struttura di memorizzazione tempo attuale
	time(&now); // funzione per salvare tempo attuale nella struttura
	struct tm *pTm = localtime(&now);
	char format[] = "%02d:%02d:%02d %s:%s\n";
	if(!newline) strcpy(format, "%02d:%02d:%02d %s:%s"); 
	printf(format ,pTm->tm_hour, pTm->tm_min, pTm->tm_sec, source, description);
}

void setSig(sigset_t *pset, int signum1, int signum2, bool block){
	// check if signal is received by global flag and resend it if not 
	// BLOCK SIGUSR1 
	if(sigemptyset(pset) < 0)perror("errore sigempty:"); //reset all set of signal
	if(signum1 != 0)sigaddset(pset, signum1); // add signum1 to set
	if(signum2 != 0)sigaddset(pset, signum2); // add signum2 to set
	if(block){
		if(sigprocmask(SIG_BLOCK, pset, NULL) < 0)perror("errore sigmask:");
	} //union original with my set
	else{
		if(sigprocmask(SIG_UNBLOCK, pset, NULL) < 0)perror("errore sigmask:");
	}
}

void send_mex(struct message *pms, int source, char *text, int dest){
	pthread_mutex_trylock(&mutex);
	memset(pms, '\0', sizeof(struct message));
	pms->pid = source;
	strcpy(pms->mex, text);
	printf("%d -> %d mex:%s\n", pms->pid, dest, pms->mex);
	write(fdw, pms, sizeof(struct message));
	// kill(dest, SIGUSR1);
	pthread_mutex_unlock(&mutex);
}

void receive_mex(struct message *pms){
	memset(pms, '\0', sizeof(struct message));
	read(fdr, pms, sizeof(struct message));
}

int main(int argc, char const *argv[])
{	
	// char *myfifo = "/tmp/myfifo";
	unlink(myfifo); 
	/* S_IRWXU is a definition in stat.h in octal value to set
	fifo file permission for OS */
	if(mkfifo(myfifo, S_IRWXU) < 0) perror("errore fifo:"); 
	/* verificare errore apertura canali fifo ed eventualmente generare codice errore*/

	// if((fdr = open(myfifo, O_RDONLY | O_NONBLOCK)) < 0)perror("errore fdr:"); // open fifo for reading
	// if((fdw = open(myfifo, O_WRONLY)) < 0)perror("errore fdw:"); // open fifo to writing
	int ptorre, phangar;
	int *stat;

	struct sigaction sa; 
	memset(&sa, '\0', sizeof(struct sigaction)); 
	sa.sa_handler = &sigHandler; // pointer to function
	//sa.sa_flags = SA_SIGINFO;

	// set signal action to change behavior 
	sigaction(SIGALRM, &sa, NULL); // sig for timer
	sigaction(SIGUSR1, &sa, NULL); // sig for sync messaged
	sigaction(SIGUSR2, &sa, NULL); // sig for signaling end child proc

	//settings for mask signals (synchornization)
	//sigemptyset(&sigset); // reset all set of signal
	//sigaddset(&sigset, SIGUSR1); //add SIGUSR1 to set
	//sigprocmask(SIG_BLOCK, &sigset, NULL);
	//SIG_BLOCK = union of original blocked signals and your set

	ptorre = fork(); // processo torre
	if (ptorre == 0){ Torre(); exit(1);} 
	else{
		phangar = fork(); // processo hangar
		if(phangar == 0){ Hangar(); exit(1); }
	}

	waitpid(ptorre ,&stat, NULL);
	if(WIFEXITED(stat)){
		printf("closing...\n");
		close(fdr);
		close(fdw);
		unlink(myfifo);
	}		
	return 0;
}

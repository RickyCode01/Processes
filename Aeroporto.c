#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> // S_definitions
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include "Aeroporto.h"
#include "Torre.c"
#include "Hangar.c"

void sigHandler(/*int sig, siginfo_t *si, void *uap*/){
	//printf("sending process ID:%d\n", si->si_pid);
}

int child_n(char *name){ // return child number from child name
	while(*name){ // remember to use pointer value to read single char of string
		if(isdigit(*name))break; 
		*name++; // increment pointer
	} 
	//printf("test: %d\n", atoi(name));
	if(*name == '\0')return -1; // pointer to NULL value
	else return atoi(name); // return integer value of remaining strings number
}

void print_Event(char* source, char* description, bool newline){
	//fflush(stdout);
	time_t now; // struct to save current calendar time 
	time(&now); // fun to save in struct current calendar time 
	struct tm *pTm = localtime(&now); // take calendar time and return a pointer to struct containig time from epoch 
	char format[256] = "%02d:%02d:%02d"; // format to print time
	//printf("cld_n:%d\n", child_n("aereo 11"));
	if(!strcmp(source, "torre") || !strcmp(source, "hangar")) strcat(format, "\e[1m"); // if torre or hangar use bold
	else strcat(format, colours[child_n(source)%size]);
	strcat(format, " %s\033[0m:%s\n");
	if(!newline) format[strlen(format)-1] = '\0';
	//printf("string:%s", format);
	printf(format ,pTm->tm_hour, pTm->tm_min, pTm->tm_sec, source, description);
}

void setSig(sigset_t *pset, int signum1, int signum2, bool block){
	if(sigemptyset(pset) < 0)perror("errore sigempty:"); //reset all set of signal
	if(signum1 > 0)sigaddset(pset, signum1); // add signum1 to set
	if(signum2 > 0)sigaddset(pset, signum2); // add signum2 to set
	if(block){
		if(sigprocmask(SIG_BLOCK, pset, NULL) < 0)perror("errore sigmask:"); //union original with my set
	} 
	else{
		if(sigprocmask(SIG_UNBLOCK, pset, NULL) < 0)perror("errore sigmask:");
	}
}


int main(int argc, char const *argv[])
{	
	//char *myfifo = "/tmp/myfifo";
	/* S_IRWXU is a definition in stat.h in octal value to set
	fifo file permission for OS */
	if(mkfifo(myfifo, S_IRWXU) < 0) perror("errore fifo:"); // make named pipe 

	//if((fdr = open(myfifo, O_RDONLY | O_NONBLOCK)) < 0)perror("errore fdr:"); // open fifo for reading
	//if((fdw = open(myfifo, O_WRONLY)) < 0)perror("errore fdw:"); // open fifo to writing
	int ptorre, phangar;
	int stat;

	struct sigaction sa; //struct for set signals action 
	memset(&sa, '\0', sizeof(struct sigaction)); // reset structure  
	sa.sa_handler = &sigHandler; // pointer to fun called when signals are raised
  	// sa.sa_flags = SA_RESTART /*| SA_SIGINFO */; 

	// set signal action to change behavior 
	sigaction(SIGALRM, &sa, NULL); // sig for timer
	sigaction(SIGUSR1, &sa, NULL); // sig for sync messaged
	sigaction(SIGUSR2, &sa, NULL); // sig for signaling end child proc

	ptorre = fork(); // processo torre
	if (ptorre == 0){ Torre(); exit(1);} 
	else{
		phangar = fork(); // processo hangar
		if(phangar == 0){ Hangar(); exit(1); }
	}

waitpid(ptorre ,&stat, NULL); // wait for torre to finish
	if(WIFEXITED(stat)){
		printf("closing...\n");
		unlink(myfifo); // delete pipe 
	}		
	return 0;
}

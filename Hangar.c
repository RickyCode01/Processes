// processo Hangar Aeroporto

void send_mex(struct message *pms, int source, char *text, int dest){
	memset(pms, '\0', sizeof(struct message)); // clean structure
	pms->pid = source;
	strcpy(pms->mex, text);
	//printf("%d -> %d mex:%s\n", pms->pid, dest, pms->mex);
	if((write(fdw, pms, sizeof(struct message))) < 0)perror("errore write:");
}

char get_random(char min, char max){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec); // set the seed for random generation 
	unsigned char r = min + (rand() % (max-min));
	return r;
}

void Aereo(char *id, int ptorre){
	struct message ms;
	sigset_t sigset;

	print_Event(id, "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n", mypid);

	setSig(&sigset, SIGUSR1, SIGALRM, true);

	// waiting preparation 
	unsigned char myrand = get_random(3,8);
	print_Event(id, "preparazione ", false);
	printf("in %d secondi\n", myrand);
	alarm((unsigned int)myrand); // set alarm to awake aereo
	int sig = 0;
	while(sig != SIGALRM)sigwait(&sigset, &sig); // wait until SIGALRM is received
	print_Event(id, "pronto per decollo", true);
	//sigdelset(&sigset, SIGALRM); // remove sigalarm
	//printf("sig_check: %d\n", sigismember(&sigset, SIGALRM));

	send_mex(&ms, mypid, "ready", ptorre); // send ready to torre
	
	print_Event(id, "richiesta decollo inviata", true);

	sig = 0;
	sigwait(&sigset, &sig); // wait for signaling from torre
	//receive_mex(&ms);
	//printf("%d <- %d:%s\n", mypid, ms.pid, ms.mex);
	if(sig == SIGUSR1){ // check if signal is
		// prepaire to takeoff
		unsigned char myrand = get_random(5,15);
		print_Event(id, "decollo ", false);
		printf("in %d secondi\n", myrand);
		//setSig(&sigset, SIGALRM, true);
		alarm((unsigned int)myrand); // set alarm to awake aereo
		while(sig != SIGALRM)sigwait(&sigset, &sig); // wait to sigalarm
		send_mex(&ms, mypid, "takeoff", ptorre); // send takeoff to torre
		print_Event(id, "decollato", true);
		return;
	}else{
		printf("segnale ricevuto: %d", sig);
	}
}

void Hangar(){
	int pid[10];
	int status;
	int ptorre = getpid()-1;
	int w;
	char child_str[10];

	if((fdw = open(myfifo, O_WRONLY)) < 0)perror("fdr error:");

	print_Event("hangar", "avvio!", true);
	for(int i = 0; i < childs; i++){
		sprintf(child_str, "aereo %d", i);
		char child_event[25] = "creazione ";
		strcat(child_event, child_str);
		print_Event("hangar", child_event, true);
		pid[i]=fork();
		if(pid[i] == 0){
			Aereo(child_str, ptorre);
			exit(1); // chiude i figli
		}
		sleep(2);
	}

	for(w = 0; w < childs; w++){ // wait processes to finish
		waitpid(pid[w] ,&status, 0); 
	}
	
	if(WIFEXITED(status) && w==childs){
		print_Event("hangar", "fine", true);
		struct message mymex;
	 	send_mex(&mymex, getpid(), "end", ptorre);
	 	close(fdw);
	}

}


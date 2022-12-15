// processo Hangar Aeroporto

void send_mex(struct message *pms, int source, char *text, int dest){
	memset(pms, '\0', sizeof(struct message)); // clean structure
	pms->pid = source;
	strcpy(pms->mex, text);
	printf("%d -> %d mex:%s\n", pms->pid, dest, pms->mex);
	if((write(fdw, pms, sizeof(struct message))) < 0)perror("errore write:");
}

char get_random(char min, char max){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec); // set the seed for random generation 
	unsigned char r = min + (rand() % (max-min));
	return r;
}

void Aereo(int id, int ptorre){
	char sid[10];
	struct message ms;
	sigset_t sigset;

	sprintf(sid, "aereo %d", id);
	print_Event(sid, "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n", mypid);

	setSig(&sigset, SIGUSR1, SIGALRM, true);

	// waiting preparation 
	unsigned char myrand = get_random(3,8);
	print_Event(sid, "preparazione ", false);
	printf("in %d secondi\n", myrand);
	alarm((unsigned int)myrand); // set alarm to awake aereo
	int sig = 0;
	while(sig != SIGALRM)sigwait(&sigset, &sig); // wait until SIGALRM is received
	print_Event(sid, "pronto per decollo", true);
	//sigdelset(&sigset, SIGALRM); // remove sigalarm
	//printf("sig_check: %d\n", sigismember(&sigset, SIGALRM));

	send_mex(&ms, mypid, "ready", ptorre); // send ready to torre
	
	print_Event(sid, "richiesta decollo inviata", true);

	sig = 0;
	sigwait(&sigset, &sig); // wait for signaling from torre
	//receive_mex(&ms);
	//printf("%d <- %d:%s\n", mypid, ms.pid, ms.mex);
	if(sig == SIGUSR1){ // check if signal is 
		// prepaire to takeoff
		unsigned char myrand = get_random(5,15);
		print_Event(sid, "decollo ", false);
		printf("in %d secondi\n", myrand);
		//setSig(&sigset, SIGALRM, true);
		alarm((unsigned int)myrand); // set alarm to awake aereo
		while(sig != SIGALRM)sigwait(&sigset, &sig); // wait to sigalarm
		send_mex(&ms, mypid, "takeoff", ptorre); // send takeoff to torre
		print_Event(sid, "decollato", true);
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
	// const int proc = 10;

	if((fdw = open(myfifo, O_WRONLY)) < 0)perror("fdr error:");

	//while(read(fdr, ptorre, sizeof(int)) < 0);
	print_Event("hangar", "avvio!", true);
	for(int i = 0; i < childs; i++){
		pid[i]=fork();
		if(pid[i] == 0){
			Aereo(i, ptorre);
			exit(1); // chiude i figli
		}
	}

	for(w = 0; w < childs; w++){ // wait processes to finish
		waitpid(pid[w] ,&status, 0); 
	}
	
	if(WIFEXITED(status) && w==childs){
		struct message mymex;
	 	send_mex(&mymex, getpid(), "end", ptorre);
	 	close(fdw);
	}

}

